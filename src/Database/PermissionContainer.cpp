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


#include "PermissionContainer.h"
#include "QtDebug"
#include <QDateTime>
#include "UserAccess.h"

PermissionContainer::PermissionContainer(QString parentID, QObject *parent) : NestedArrayContainer("users", parentID, "permissions", parent)
{
}

PermissionContainer::~PermissionContainer()
{
}

QVariantMap PermissionContainer::insert(QVariant data)
{
    Permission permission(data);
    permission._creationTimeStamp = QDateTime::currentDateTime();
    return _appendItem(permission.toVariant());
}

void PermissionContainer::_insert(QVariantList data)
{
    QListIterator<QVariant> it(data);
    while(it.hasNext())
    {
        QVariantMap item = it.next().toMap();
        qlonglong expiration = item["expirationDate"].toMap()["$date"].toLongLong();
        item["expirationDate"] = QDateTime::fromMSecsSinceEpoch(expiration);

        qlonglong creation = item["creationDate"].toMap()["$date"].toLongLong();
        item["creationDate"] = QDateTime::fromMSecsSinceEpoch(creation);
        _data << item;
    }
}

QVariantMap PermissionContainer::setProperty(int index, QString property, QVariant value)
{
    if(property == "expirationDate")
    {
        return _setProperty(index, property, value.toDateTime());
    }

    if(property == "expires" || property == "active"  || property == "type")
        return _setProperty(index, property, value);

    return QVariantMap();
}

bool PermissionContainer::canRead(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_SEE_USERS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);
}

bool PermissionContainer::canWrite(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_MODIFY_USERS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);
}

