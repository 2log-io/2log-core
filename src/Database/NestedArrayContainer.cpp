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


#include "NestedArrayContainer.h"
#include <QDebug>
#include "MongoDB.h"

NestedArrayContainer::NestedArrayContainer(QString collection, QString parentID, QString itemType, QObject *parent) : QObject(parent),
    _parentID(parentID),
    _field(itemType),
    _collection(collection)
{
}

void NestedArrayContainer::_insert(QVariantList data)
{
    _data.append(data);
}

NestedArrayContainer::~NestedArrayContainer()
{
    Q_EMIT destroyed(_collection, _parentID, _field);
}

QVariantMap NestedArrayContainer::_setProperty(int index, QString name, QVariant data)
{
    QVariantMap item;
    if(index >= 0 && index < _data.count())
    {
        item = _data.at(index).toMap();
        item[name] = data;
        _data.replace(index, item);
        MongoDB::instance()->changePropertyOfNestedArrayItem(_collection, _parentID, _field, index, name, data);
        Q_EMIT propertyChanged(index, name, _createTemplate(item));
        Q_EMIT arrayDataChanged();
    }
    return item;
}

QVariantList NestedArrayContainer::getData()
{
    QVariantList data;
    QListIterator<QVariant>it(_data);
    while(it.hasNext())
        data << _createTemplate(it.next().toMap());

    return data;
}

QVariantMap NestedArrayContainer::getItem(int index)
{
    if(index >= 0 && index < _data.count())
        return _createTemplate(_data.at(index).toMap());

    return QVariantMap();
}

int NestedArrayContainer::count() const
{
    return _data.count();
}

bool NestedArrayContainer::load()
{
    if(_parentID.isEmpty())
        return false;

    auto projection = document{};
    projection << _field.toStdString() << 1;
    document query = document{};
    query << "_id"<< bsoncxx::oid(_parentID.toStdString());
    QVariantList result = MongoDB::instance()->selectProjection(_collection, projection.view(), query.view());

    if(result.count() == 1)
    {
        _insert(result.at(0).toMap()[_field].toList());
        return true;
    }

    return false;
}

QVariantMap NestedArrayContainer::_appendItem(QVariantMap data)
{
    MongoDB::instance()->addNestedArrayItem(_collection, _parentID, _field, data);
    _data.append(data);
    QVariantMap insertedItem = _createTemplate(data);
    Q_EMIT itemAppended(insertedItem);
    Q_EMIT arrayDataChanged();
    return insertedItem;
}

bool NestedArrayContainer::removeItem(int index)
{
    if(index < 0 || index >= _data.count())
        return false;

     MongoDB::instance()->removeNestedArrayItem(_collection, _parentID, _field, index);
    _data.removeAt(index);
    Q_EMIT itemRemoved(index);
    Q_EMIT arrayDataChanged();
    return true;
}

QVariantMap NestedArrayContainer::_createTemplate(QVariantMap data) const
{
    QVariantMap item;
    item["data"] = data;
    item["uuid"] = data.value("uuid", -1);
    return item;
}
