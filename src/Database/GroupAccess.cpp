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


#include "GroupAccess.h"
#include "MongoDB.h"
#include <QDebug>

Q_GLOBAL_STATIC(GroupAccess, groupAccess);

GroupAccess::GroupAccess(QObject *parent) : QObject(parent)
{
    auto projection = document{};
    projection << "entities" << 0;
    QVariantList groups = MongoDB::instance()->selectProjection("groups", projection.view());
    QListIterator<QVariant> it(groups);

    //iterate ove results
    while(it.hasNext())
    {
        groupPtr groupObj = groupPtr(new Group(it.next().toMap(), this));
        insertGroup(groupObj);
    }
}

QList<groupPtr> GroupAccess::getGroups()
{
    return _groups;
}

groupPtr GroupAccess::getGroup(QString uuid)
{
    return _groupIDMap.value(uuid, groupPtr());
}

groupPtr GroupAccess::insertGroup(QVariantMap group)
{
    QString groupName = group["name"].toString();
    if(groupName.isEmpty() ||  !getUuidByName(groupName).isEmpty())
    {
        return groupPtr();
    }

    groupPtr groupObj = groupPtr(new Group(group, this));
    QString uuid =  MongoDB::instance()->insertDocument("groups", groupObj->toVariant());
    groupObj->setUUID(uuid);
    insertGroup(groupObj);

    Q_EMIT groupInserted(groupObj);
    return groupObj;
}

bool GroupAccess::deleteGroup(QString id)
{
    groupPtr group = _groupIDMap.value(id);
    if(group.isNull())
        return false;
    MongoDB::instance()->removeDocument("groups", id);

    _groupIDMap.remove(group->getUUID());
    _groups.removeAll(group);
    // TODO remove all group permissions of Users
    Q_EMIT groupDeleted(group);
    return true;

   // return false;
}

GroupAccess *GroupAccess::instance()
{
    return groupAccess;
}

void GroupAccess::insertGroup(groupPtr group)
{
    QString groupName = group->name();
    _groupIDMap.insert(group->getUUID(), group);
    _groups << group;
}


QString GroupAccess::getUuidByName(QString name) const
{
    foreach(groupPtr group, _groups)
    {
        if(group->name().toLower() == name.toLower())
            return group->getUUID();

    }
    return "";
}


int GroupAccess::indexOf(groupPtr group) const
{
    return _groups.indexOf(group);
}

int GroupAccess::indexOf(QString uuid) const
{
    return _groups.indexOf(_groupIDMap.value(uuid));
}

