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


#include "GroupEntityContainer.h"
#include "UserAccess.h"

GroupEntityContainer::GroupEntityContainer(QString parentID, QObject* parent) : NestedArrayContainer ("groups", parentID, "entities", parent)
{

}

QVariantMap GroupEntityContainer::setProperty(int index, QString property, QVariant value)
{
    return _setProperty(index, property, value);
}

QVariantMap GroupEntityContainer::insert(QVariant data)
{
    GroupEntity entity(data);
    return _appendItem(entity.toVariant());
}

bool GroupEntityContainer::hasEntity(QString entityID, QString entityType)
{
    QListIterator<QVariant> it(_data);
    while(it.hasNext())
    {
        QVariantMap map = it.next().toMap();
        if(map["entityID"] == entityID && map["entityType"] == entityType)
            return true;
    }
    return false;
}

bool GroupEntityContainer::canRead(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_SEE_GROUPS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);

}

bool GroupEntityContainer::canWrite(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_MODIFY_GROUPS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);
}

