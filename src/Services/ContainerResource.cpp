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


#include "ContainerResource.h"
#include "../Database/UserAccess.h"
#include "../Database/PermissionContainer.h"
#include "Server/Authentication/AuthentificationService.h"


ContainerResource::ContainerResource(QSharedPointer<NestedArrayContainer> container, QObject *parent) : ListResource(nullptr, parent)
{
    setDynamicContent(false);
    setAllowUserAccess(false);
    _containerPtr = container;
    connect(container.data(), &NestedArrayContainer::itemAppended, this, &ContainerResource::newItem);
    connect(container.data(), &NestedArrayContainer::propertyChanged, this, &ContainerResource::permisionPropertyChanged);
}

ContainerResource::~ContainerResource()
{
}

IResource::ModificationResult ContainerResource::setProperty(QString property, QVariant data, int index, QString uuid, QString token)
{
    Q_UNUSED(uuid)
    ModificationResult result;
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);

    if(!_containerPtr->canWrite(user))
    {
        result.error = PERMISSION_DENIED;
        return result;
    }

    if(index < 0 && index > _containerPtr->count())
    {
        result.error = INVALID_PARAMETERS;
        return result;
    }

    QVariantMap resultMap = _containerPtr->setProperty(index, property, data);
    result.data = resultMap;
    if(resultMap.isEmpty())
        result.error = STORAGE_ERROR;

    return result;
}

QVariantList ContainerResource::getListData() const
{
    return _containerPtr->getData();

}

void ContainerResource::newItem(QVariant data)
{
    Q_EMIT itemAppended(data.toMap(), iUserPtr());
}

void ContainerResource::permisionPropertyChanged(int index, QString property, QVariant data)
{
    Q_EMIT propertySet(property, data, index, "-1", iIdentityPtr(), 0);
}

ContainerResource::ModificationResult ContainerResource::appendItem(QVariant data, QString token)
{
    ModificationResult result;
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);

    if(!_containerPtr->canWrite(user))
    {
        result.error = PERMISSION_DENIED;
        return result;
    }

    QVariantMap insertedItem = _containerPtr->insert(data);
    result.data = insertedItem;
    return  result;
}

IResource::ModificationResult ContainerResource::appendList(QVariantList data, QString token)
{
    ModificationResult result;
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);

    if(!_containerPtr->canWrite(user))
    {
        result.error = PERMISSION_DENIED;
        return result;
    }

    QVariantList appendedItems;
    QListIterator<QVariant> it(data);
    while(it.hasNext())
    {
        appendedItems << _containerPtr->insert(it.next());
    }

    result.data = appendedItems;
    return result;

}

IResource::ModificationResult ContainerResource::removeItem(QString uuid, QString token, int index)
{
    Q_UNUSED(uuid)
    ModificationResult result;
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);

    if(!_containerPtr->canWrite(user))
    {
        result.error = PERMISSION_DENIED;
        return result;
    }

    if(!_containerPtr->removeItem(index))
        result.error = STORAGE_ERROR;

    return result;
}
