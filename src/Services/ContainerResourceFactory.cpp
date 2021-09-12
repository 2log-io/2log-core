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


#include "ContainerResourceFactory.h"
#include "ContainerResource.h"
#include "../Database/PermissionContainer.h"
#include "Database/UserAccess.h"
#include <QDebug>
#include "../Database/NestedDataContainerManager.h"

ContainerResourceFactory::ContainerResourceFactory(QString collection, QString fieldName, QObject* parent) : IResourceFactory(parent),
    _fieldName(fieldName),
    _collection(collection)
{
}

ContainerResourceFactory::~ContainerResourceFactory()
{
}

QString ContainerResourceFactory::getResourceID(QString descriptor, QString token) const
{
    Q_UNUSED(token)
    return descriptor;
}

QString ContainerResourceFactory::getResourceType() const
{
    return "synclist";
}

QString ContainerResourceFactory::getDescriptorPrefix() const
{
    return "labcontrol/"+_collection+"/"+ _fieldName;
}

resourcePtr ContainerResourceFactory::createResource(QString token, QString descriptor, QObject *parent)
{
    Q_UNUSED(parent)

    QStringList tokens = descriptor.split("/");
    if(tokens.count() < 4)
        return resourcePtr();

    QString userID = tokens.at(3);
    if(userID.isEmpty())
        return resourcePtr();

    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    QSharedPointer<NestedArrayContainer> arr = NestedDataContainerManager::instance()->getNestedArrayData(_collection, userID, _fieldName);

     if(arr.isNull() || !arr->canRead(user))
        return resourcePtr();

    if(arr.isNull())
    {
        qWarning() << "Cast to " + _fieldName + " container has failed!";
        return resourcePtr();
    }

    return resourcePtr(new ContainerResource(arr));
}
