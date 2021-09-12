/*   2log.io
 *   Copyright (C) 2021 - 2log.io | mail@2log.io,  mail@friedemann-metzger.de
 *   
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "MongoDB.h"
#include <QDebug>

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <mongocxx/instance.hpp>

#include <mongocxx/logger.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <QDateTime>
#include <QProcessEnvironment>
//#include <optional>


Q_GLOBAL_STATIC(MongoDB, mongoDB);

MongoDB::MongoDB() : QObject (nullptr)
{
    class noop_logger : public mongocxx::logger {
           public:
            virtual void operator()(mongocxx::log_level,
                                    bsoncxx::stdx::string_view,
                                    bsoncxx::stdx::string_view) noexcept {}
        };

        auto a =
            bsoncxx::stdx::make_unique<mongocxx::instance>(bsoncxx::stdx::make_unique<noop_logger>());

        QString mongoDBHost = QProcessEnvironment::systemEnvironment().value("MONGODB_HOST", "localhost:27017");
        _dbName = QProcessEnvironment::systemEnvironment().value("MONGODB_NAME", "fablabcontrol");
        qInfo()<<"Connect to MongoDB: mongodb://"+mongoDBHost;
        configure(std::move(a),
                           bsoncxx::stdx::make_unique<mongocxx::pool>(mongocxx::uri{"mongodb://"+mongoDBHost.toStdString()}));

}

MongoDB::MongoDB(MongoDB&){}

QVariantList MongoDB::toVariants(cursor &cursor) const
{
    QVariantList result;

    for(auto doc : cursor)
    {
        QVariant jsonDoc;
        QByteArray data = QByteArray::fromStdString(bsoncxx::to_json(doc));
        QJsonParseError error;
        jsonDoc = QJsonDocument::fromJson(data, &error).toVariant();
        result.append(jsonDoc);
    }
    return result;
}

MongoDB* MongoDB::instance()
{
    return mongoDB;
}

void MongoDB::configure(std::unique_ptr<mongocxx::instance> mongodbInstance,
               std::unique_ptr<mongocxx::pool> connectionPool) {
    this->mongodbInstance = std::move(mongodbInstance);
    this->connectionPool = std::move(connectionPool);
}


Connection MongoDB::getConnection() const
{
    return connectionPool->acquire();
}

QString MongoDB::insertDocument(QString collectionName, QVariantMap document)
{
   // auto jsonString = QJsonDocument::fromVariant(document).toJson(QJsonDocument::Compact).toStdString();
    auto con = connectionPool->acquire();
    auto mappedDocument = queryFromVariant(document);//bsoncxx::from_json(jsonString);
    auto result = (*con)[_dbName.toStdString()][collectionName.toStdString()].insert_one(mappedDocument.view());
    if(result)
    {
        bsoncxx::oid id = result->inserted_id().get_oid().value;
         return QString::fromStdString(id.to_string());
    }
    return "";
}

QString MongoDB::insertDocument(QString collectionName, const Document &document)
{
   auto con = connectionPool->acquire();
   auto result = (*con)[_dbName.toStdString()][collectionName.toStdString()].insert_one(document);
   if(result)
   {
       bsoncxx::oid id = result->inserted_id().get_oid().value;
       return QString::fromStdString(id.to_string());
   }
   return "";

}

int MongoDB::removeDocument(QString collectionName, QString uuid) const
{
    auto con = connectionPool->acquire();
    auto filter = document{};
    filter << "_id" << bsoncxx::oid(uuid.toStdString());
    return (*con)[_dbName.toStdString()][collectionName.toStdString()].delete_many(filter.view())->deleted_count();
}

QJsonDocument MongoDB::selectSingle(QString collectionName, Query query) const
{
    auto con = connectionPool->acquire();
    auto document = (*con)[_dbName.toStdString()][collectionName.toStdString()].find_one(query);

    QJsonDocument jsonDoc;
    if (document)
    {
        QByteArray data = QByteArray::fromStdString(bsoncxx::to_json(*document));
        jsonDoc.fromJson(data);
    }

    return jsonDoc;
}

QVariantList MongoDB::select(QString collectionName, Query query) const
{
    auto con = connectionPool->acquire();
    auto cursor = (*con)[_dbName.toStdString()][collectionName.toStdString()].find(query);
    return toVariants(cursor);
}

QVariantList MongoDB::selectProjection(QString collectionName, Query opts, Query query) const
{
    auto con = connectionPool->acquire();
    mongocxx::options::find options{};
    options.projection(opts);
    auto cursor = (*con)[_dbName.toStdString()][collectionName.toStdString()].find(query, options);
	return toVariants(cursor);
}

QVariantList MongoDB::selectProjection(QString collectionName, QString query, QString opts) const
{
	bsoncxx::document::value filter = bsoncxx::from_json( query.toStdString() );
	bsoncxx::document::value project = bsoncxx::from_json( opts.toStdString() );

	return selectProjection(collectionName, project.view(), filter.view() );
}

QVariantList MongoDB::selectPipeline(QString collectionName, int limit, QVariantMap match, QVariantMap sort, QVariantMap project, QVariantMap group1, QVariantMap group2, QVariantMap project2, QString count)
{
    auto con = connectionPool->acquire();
    mongocxx::pipeline p{};
    p.match(queryFromVariant(match).view());
    if(!project.isEmpty())
        p.project(queryFromVariant(project).view());
    if(limit > 0)
        p.limit(limit);
    if(!group1.isEmpty())
        p.group(queryFromVariant(group1).view());
    if(!project2.isEmpty())
        p.project(queryFromVariant(project2).view());
    if(!group2.isEmpty())
        p.group(queryFromVariant(group2).view());
    if(!sort.isEmpty())
        p.sort(queryFromVariant(sort).view());
    if(!count.isEmpty())
        p.count(count.toStdString());


    auto cursor = (*con)[_dbName.toStdString()][collectionName.toStdString()].aggregate(p, mongocxx::options::aggregate{});
    return toVariants(cursor);
}

QVariantList MongoDB::selectUnwindPipeline(QString collectionName, QVariantMap match,  QString unwind, QVariantMap project, QVariantMap group1, QVariantMap group2, QVariantMap project2)
{
    auto con = connectionPool->acquire();
    mongocxx::pipeline p{};
    p.match(queryFromVariant(match).view());
    if(!unwind.isEmpty())
    {
        p.unwind(unwind.toStdString());
    }
    if(!project.isEmpty())
        p.add_fields(queryFromVariant(project).view());
    if(!group1.isEmpty())
        p.group(queryFromVariant(group1).view());
    if(!project2.isEmpty())
        p.project(queryFromVariant(project2).view());
    if(!group2.isEmpty())
        p.group(queryFromVariant(group2).view());

    auto cursor = (*con)[_dbName.toStdString()][collectionName.toStdString()].aggregate(p, mongocxx::options::aggregate{});
    return toVariants(cursor);
}

bool MongoDB::updateDocument(QString collectionName, QString uuid, QVariantMap properties)
{
    auto changes = document{};
    changes    << "$set" <<  queryFromVariant(properties);
    auto filter = document{};
     filter << "_id" << bsoncxx::oid(uuid.toStdString());
    return updateDocument(collectionName, filter.view(), changes.view());
}

bool MongoDB::updateDocument(QString collectionName, Query filter, Query changes, bool upsert)
{
    auto con = connectionPool->acquire();
    auto options = mongocxx::options::update();
    options.upsert(upsert);
    return (*con)[_dbName.toStdString()][collectionName.toStdString()].update_one(filter, changes, options)->modified_count() == 1;
}

int MongoDB::update(QString collectionName, QString jsonQuery, QString jsonUpdate)
{
	bsoncxx::document::value query = bsoncxx::from_json( jsonQuery.toStdString() );
	bsoncxx::document::value update = bsoncxx::from_json( jsonUpdate.toStdString() );
    auto con = connectionPool->acquire();
    return (*con)[_dbName.toStdString()][ collectionName.toStdString() ].update_many( query.view(), update.view() )->modified_count();
}

int MongoDB::removeDocumentsByQuery(QString collectionName, QString jsonQuery)
{
	bsoncxx::document::value query = bsoncxx::from_json( jsonQuery.toStdString() );
    auto con = connectionPool->acquire();
    return (*con)[_dbName.toStdString()][collectionName.toStdString()].delete_many( query.view() )->deleted_count();
}

QVariantList MongoDB::JoinWithPipeline(QString collectionName, int limit, QVariantMap match, QVariantMap sort, QVariantMap project, QVariantMap group, QString foreignCollection, QString ownField, QString foreignField, QString as, QVariantMap addFields)
{
    auto con = connectionPool->acquire();
    mongocxx::pipeline p{};
    p.match(queryFromVariant(match).view());
    if(!project.isEmpty())
        p.project(queryFromVariant(project).view());
    if(!sort.isEmpty())
        p.sort(queryFromVariant(sort).view());
    if(limit > 0)
        p.limit(limit);
    if(!group.isEmpty())
        p.group(queryFromVariant(group).view());
    if(!addFields.isEmpty())
        p.add_fields(queryFromVariant(addFields).view());

    auto doc = document{};

    doc << "from" << foreignCollection.toStdString() << "localField" << ownField.toStdString() << "foreignField" << foreignField.toStdString() << "as" << as.toStdString();
    p.lookup(doc.view());

    auto cursor = (*con)[_dbName.toStdString()][collectionName.toStdString()].aggregate(p, mongocxx::options::aggregate{});
    return toVariants(cursor);
}


bool MongoDB::changePropertyOfNestedArrayItem(QString collection, QString parentID, QString field, int index, QString property, QVariant value)
{
    auto filter = document{};
    filter << "_id" << bsoncxx::oid(parentID.toStdString());
    auto changes = document{};
    QString fieldKey = field +"."+ QString::number(index)+"."+property;
    QVariantMap data;
    data[property] = value;
    changes << "$set" <<  kvpFromVariant(fieldKey, value)  ;
    return updateDocument(collection, filter.view(), changes.view());
}

bool MongoDB::addNestedArrayItem(QString collection, QString parentID, QString field, QVariantMap data)
{
    auto changes = document{};
    changes    << "$push" << open_document << field.toStdString() << MongoDB::queryFromVariant(data) << close_document;
    auto filter = document{};
    filter << "_id" << bsoncxx::oid(parentID.toStdString());
    updateDocument(collection, filter.view(), changes.view());
    return true;
}

bool MongoDB::removeNestedArrayItem(QString collection, QString parentID, QString field, int index)
{
       auto changes1 = document{};
       auto filter = document{};
       changes1    << "$unset" << open_document << field.toStdString()+"."+std::to_string(index) <<  1 << close_document;
       filter << "_id" << bsoncxx::oid(parentID.toStdString());
       updateDocument(collection, filter.view(), changes1.view());
       auto changes2 = document{};
       changes2    << "$pull" << open_document << field.toStdString() << bsoncxx::types::b_null() << close_document;
       return updateDocument(collection, filter.view(), changes2.view());
}

bool MongoDB::changeProperty(QString collection, QString uuid, QString property, QVariant value)
{
    QVariantMap data;
    data[property] = value;
    return MongoDB::instance()->updateDocument(collection, uuid, data);
}

document MongoDB::kvpFromVariant(QString key, QVariant value)
{
    auto doc = document{};
    QVariant val = value;
    switch(val.type())
    {
        case QVariant::Int: doc << key.toStdString() << val.toInt();
        break;
        case QVariant::Double: doc << key.toStdString() << bsoncxx::types::b_double{val.toDouble()};
        break;
        case QVariant::Bool: doc << key.toStdString() << bsoncxx::types::b_bool{val.toBool()};
        break;
        case QVariant::DateTime:
        {
            qint64 millis = val.toDateTime().toMSecsSinceEpoch();
            doc << key.toStdString() << bsoncxx::types::b_date{std::chrono::milliseconds(millis)};
        }
        break;
        //case QVariant::LongLong: doc << key.toStdString() << val.toLongLong();
        //break;
        case QVariant::String: doc << key.toStdString() << val.toString().toStdString();
        break;
        default: qWarning()<< "Variant to BSON -> Unsupported type:" << val.type();
    }
    return doc;
}

document MongoDB::queryFromVariant(QVariantMap variant)
{
    QMapIterator<QString, QVariant> it(variant);
    auto doc = document{};
    while(it.hasNext())
    {
        it.next();
        QVariant val = it.value();
        switch(val.type())
        {
            case QVariant::Int: doc << it.key().toStdString() << val.toInt();
            break;
            case QVariant::Double: doc << it.key().toStdString() << bsoncxx::types::b_double{val.toDouble()};
            break;
            case QVariant::Bool: doc << it.key().toStdString() << bsoncxx::types::b_bool{val.toBool()};
            break;
            case QVariant::DateTime:
            {
                qint64 millis = val.toDateTime().toMSecsSinceEpoch();
                doc << it.key().toStdString() << bsoncxx::types::b_date{std::chrono::milliseconds(millis)};
            }
            break;
               case QVariant::String: doc << it.key().toStdString() << val.toString().toStdString();
            break;
            case QVariant::Map: doc << it.key().toStdString() << queryFromVariant(val.toMap());
            break;
            case QVariant::List:
            {
                // holy shit, this little piece was so ugly to write
                QVariantList list = val.toList();
                QListIterator<QVariant> listIt(list);
                auto array = doc << it.key().toStdString() << open_array;
                while(listIt.hasNext())
                {
                    QVariant next = listIt.next();
                    if(next.type() == QVariant::Map)
                        array = array  << queryFromVariant(next.toMap());
                    else if (next.type() == QVariant::Int)
                        array = array << next.toInt();
                    else
                        array = array << next.toString().toStdString();
                }
                array << close_array;
            }
            break;
            case QVariant::Invalid:
            doc << it.key().toStdString() << bsoncxx::types::b_null{};
            break;
        default:
           break;
        }
    }

    return doc;
}

 bsoncxx::types::b_date MongoDB::fromDateTime(QDateTime date)
{
    qint64 millis = date.toMSecsSinceEpoch();
    return bsoncxx::types::b_date{std::chrono::milliseconds(millis)};
}

 int MongoDB::clearCollection(QString collectionName)
 {
     auto con = connectionPool->acquire();
     return (*con)[_dbName.toStdString()][collectionName.toStdString()].delete_many( document{}.view() )->deleted_count();
 }

 void MongoDB::dropCollection(QString collectionName)
 {
	 qDebug() << "dropCollection(" << collectionName << ")";
     auto con = connectionPool->acquire();
     (*con)[_dbName.toStdString()][collectionName.toStdString()].drop();
 }
