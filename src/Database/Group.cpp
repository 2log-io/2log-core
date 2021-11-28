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


#include "Group.h"
#include "MongoDB.h"
#include "GroupAccess.h"
#include <QDebug>

Group::Group(QVariantMap groupData, QObject *parent) : QObject(parent)
{
    _description = groupData["description"].toString();
    _name = groupData["name"].toString();
    _uuid = groupData["_id"].toMap()["$oid"].toString();
    if(!_uuid.isEmpty())
    {
        _permissions = qSharedPointerCast<GroupEntityContainer> (NestedDataContainerManager::instance()->getNestedArrayData("groups", _uuid, "entities"));
        connect(_permissions.data(), &GroupEntityContainer::arrayDataChanged, this, &Group::entitiesChangedSlot);
        Q_EMIT entitiesChanged(_uuid, _permissions->getData());
    }
}

QVariantMap Group::toVariant() const
{
    QVariantMap map;
    map["description"] = _description;
    map["name"] = _name;
    map["uuid"] = _uuid;
    if(!_permissions.isNull())
        map["entities"] = permissions();
    else
        map["entities"] = QVariantList();
    return map;
}

QString Group::description() const
{
    return _description;
}

bool Group::setDescription(const QString &description)
{
    if(_description == description)
        return false;

    _description = description;
    if(MongoDB::instance()->changeProperty("groups", _uuid, "description", description))
    {
        Q_EMIT descriptionChanged(_uuid, description);
        return true;
    }
    return false;
}

QString Group::name() const
{
    return _name;
}

bool Group::setName(const QString &name)
{
    if(name == _name || !GroupAccess::instance()->getUuidByName(name).isEmpty())
        return false;

    MongoDB::instance()->changeProperty("groups", _uuid, "name", name);
    _name = name;
    Q_EMIT nameChanged(_uuid, name);
    return true;
}

void Group::setUUID(QString uuid)
{
    if(uuid == _uuid)
        return;

    _uuid = uuid;
    _permissions = qSharedPointerCast<GroupEntityContainer> (NestedDataContainerManager::instance()->getNestedArrayData("groups", uuid, "entities"));
    connect(_permissions.data(), &GroupEntityContainer::arrayDataChanged, this, &Group::entitiesChangedSlot);
    Q_EMIT entitiesChanged(_uuid, _permissions->getData());
}

QString Group::getUUID()
{
    return _uuid;
}

QVariantList Group::permissions() const
{
    QVariantList entities;
    auto tempData = _permissions->getData();
    QListIterator<QVariant> it(tempData);
    while(it.hasNext())
    {
        entities.append(it.next().toMap().value("data"));

    }
    return entities;

}

bool Group::hasPermission(QString entityID, QString type)
{
    if(_permissions.isNull())
        return false;

    return _permissions->hasEntity(entityID, type);
}

void Group::entitiesChangedSlot()
{
    Q_EMIT entitiesChanged(_uuid, permissions());
}

