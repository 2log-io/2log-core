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


#include "GroupResource.h"
#include "Database/UserAccess.h"


GroupResource::GroupResource(QVariantMap parameters, QObject *parent) : ListResource(nullptr, parent)
{
    Q_UNUSED(parameters)
    setDynamicContent(false);
    setAllowUserAccess(false);
    connect(GroupAccess::instance(), &GroupAccess::groupInserted, this, &GroupResource::groupInserted);
    connect(GroupAccess::instance(), &GroupAccess::groupDeleted, this, &GroupResource::groupDeleted);
    auto tempGroups = GroupAccess::instance()->getGroups();
    QListIterator<groupPtr> it(tempGroups);
    while(it.hasNext())
    {
        registerGroup(it.next());
    }
}

QVariantList GroupResource::getListData() const
{
    QVariantList list;
    auto groups = GroupAccess::instance()->getGroups();
    QListIterator<groupPtr> it(groups);
    while(it.hasNext())
    {
        list << createTemplate(it.next()->toVariant());
    }

    return list;
}

bool GroupResource::isPermittedToRead(QString token) const
{
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull() || !( user->isAuthorizedTo(LAB_ADMIN) || user->isAuthorizedTo(IS_ADMIN) || user->isAuthorizedTo(LAB_SEE_GROUPS)))
        return false;

    return true;
}

bool GroupResource::isPermittedToWrite(QString token) const
{
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull() || !( user->isAuthorizedTo(LAB_ADMIN) || user->isAuthorizedTo(IS_ADMIN) || user->isAuthorizedTo(LAB_MODIFY_GROUPS)))
        return false;

    return true;

}

int GroupResource::getCount() const
{
    return GroupAccess::instance()->getGroups().count();
}

QVariant GroupResource::getItem(int idx, QString uuid) const
{
    Q_UNUSED(uuid)
    return createTemplate(GroupAccess::instance()->getGroups().at(idx)->toVariant());
}

IResource::ModificationResult GroupResource::setProperty(QString property, QVariant data, int index, QString uuid, QString token)
{
    Q_UNUSED(index)
    ModificationResult result;
    if(!isPermittedToWrite(token))
    {
        result.error =  PERMISSION_DENIED;
        return result;
    }

    groupPtr group = GroupAccess::instance()->getGroup(uuid);
    if(group.isNull())
    {
        result.error = UNKNOWN_ITEM;
        return result;
    }

    bool success = false;
    if(property == "description")
        success = group->setDescription(data.toString());

    if(property == "name")
        success = group->setName(data.toString());


    if(success)
        result.data = createTemplate(group->toVariant());
    else
        result.error = STORAGE_ERROR;

    return result;
}

QVariantMap GroupResource::createTemplate(QVariantMap data) const
{
    QVariantMap item;
    item["data"] = data;
    item["uuid"] = data["uuid"];
    return item;
}

void GroupResource::registerGroup(groupPtr group)
{
    connect(group.data(), &Group::descriptionChanged, this, &GroupResource::descriptionChanged);
    connect(group.data(), &Group::entitiesChanged, this, &GroupResource::entitiesChanged);
    connect(group.data(), &Group::typeChanged, this, &GroupResource::typeChanged);
}

void GroupResource::groupInserted(groupPtr group)
{
    registerGroup(group);
    Q_EMIT itemInserted(createTemplate(group->toVariant()),_groups.count()-1 , iUserPtr());
}

void GroupResource::groupDeleted(groupPtr group)
{
    int idx = GroupAccess::instance()->indexOf(group);
    Q_EMIT itemRemoved(idx, nullptr, iUserPtr());
}

void GroupResource::descriptionChanged(QString name, QString decription)
{
    int idx = GroupAccess::instance()->indexOf(name);
    Q_EMIT propertySet("description", decription, idx,  name, iUserPtr(), 0);
}

void GroupResource::typeChanged(QString name, QString type)
{
    int idx = GroupAccess::instance()->indexOf(name);
    Q_EMIT propertySet("type", type, idx,  name, iUserPtr(), 0);
}

void GroupResource::entitiesChanged(QString name, QVariantList entities)
{
    int idx = GroupAccess::instance()->indexOf(name);
    Q_EMIT propertySet("entities", entities, idx,  name, iUserPtr(), 0);
}

IResource::ModificationResult GroupResource::appendItem(QVariant data, QString token)
{
    ModificationResult result;
    if(!isPermittedToWrite(token))
    {
        result.error =  PERMISSION_DENIED;
        return result;
    }
    groupPtr ptr = GroupAccess::instance()->insertGroup(data.toMap());
    if(!ptr.isNull())
    {
        result.data = createTemplate(ptr->toVariant());
        return result;
    }

    result.error = UNKNOWN_ERROR;
    return result;
}

IResource::ModificationResult GroupResource::removeItem(QString uuid, QString token, int index)
{

    Q_UNUSED(index);
    ModificationResult result;
    if(!isPermittedToWrite(token))
    {
        result.error =  PERMISSION_DENIED;
        return result;
    }

    if(!GroupAccess::instance()->deleteGroup(uuid))
    {
         result.error = UNKNOWN_ERROR;
    }

    return result;
}

