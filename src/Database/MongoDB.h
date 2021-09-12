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


#ifndef MONGODB_H
#define MONGODB_H

#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <QJsonDocument>
#include <QVariantMap>
#include <QList>
#include <QObject>
#include "NestedDataContainerManager.h"

using Connection = mongocxx::pool::entry;
using Database = mongocxx::database;
using Query = bsoncxx::document::view_or_value;
using Document = bsoncxx::document::view_or_value;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using mongocxx::v_noabi::cursor;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::array_context;
using bsoncxx::builder::concatenate_doc;


class MongoDB :  public QObject
{
    Q_OBJECT

public:
    explicit MongoDB();
    static MongoDB* instance();

    void configure(std::unique_ptr<mongocxx::instance> instance, std::unique_ptr<mongocxx::pool> pool);
    Connection getConnection() const;

    QString                 insertDocument(QString collectionName, QVariantMap document);
    QString                 insertDocument(QString collectionName, const Document& document);
    int                     removeDocument(QString collectionName, QString uuid) const;
	int						clearCollection(QString collectionName);
	void					dropCollection(QString collectionName);
    QJsonDocument           selectSingle(QString collectionName, Query query) const;
    QVariantList            select(QString collectionName, Query query = document{}<<finalize) const;
    QVariantList            selectProjection(QString collectionName, Query opts, Query query = document{}<<finalize) const;
	QVariantList            selectProjection(QString collectionName, QString query, QString opts) const;
    QVariantList            selectPipeline(QString collectionName, int limit, QVariantMap match, QVariantMap sort = QVariantMap(), QVariantMap project = QVariantMap(), QVariantMap group1 = QVariantMap(), QVariantMap group2 = QVariantMap(), QVariantMap project2 = QVariantMap(), QString count = QString());
    bool                    updateDocument(QString collectionName, QString uuid, QVariantMap properties);
    bool                    updateDocument(QString collectionName, Query filter, Query content, bool upsert = false);

	int						update(QString collectionName, QString jsonQuery, QString jsonUpdate);
	int						removeDocumentsByQuery(QString collectionName, QString jsonQuery);

    bool                    changeProperty(QString collection, QString uuid,  QString property, QVariant value );

    bool                    changePropertyOfNestedArrayItem(QString collection, QString parentID, QString field, int index, QString property, QVariant value);
    bool                    addNestedArrayItem(QString collection, QString parentID, QString field, QVariantMap data);
    bool                    removeNestedArrayItem(QString collection, QString parentID, QString field,int index);


    bsoncxx::builder::stream::document kvpFromVariant(QString key, QVariant value);

    static bsoncxx::builder::stream::document queryFromVariant(QVariantMap variant);
    static bsoncxx::types::b_date fromDateTime(QDateTime date);

    QVariantList JoinWithPipeline(QString collectionName, int limit, QVariantMap match, QVariantMap sort, QVariantMap project, QVariantMap group, QString foreignCollection, QString ownField, QString foreignField, QString as, QVariantMap addFields);
    QVariantList toVariants(cursor &cursor) const;
    QVariantList selectUnwindPipeline(QString collectionName, QVariantMap match, QString unwind, QVariantMap project, QVariantMap group1, QVariantMap group2, QVariantMap project2);

private :
    MongoDB(MongoDB&);
    QString _dbName;
    QVariantList $date(cursor& cursor) const;
    std::unique_ptr<mongocxx::instance> mongodbInstance = nullptr;
    std::unique_ptr<mongocxx::pool> connectionPool = nullptr;
};

#endif // MONGODB_H
