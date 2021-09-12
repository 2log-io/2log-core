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


#include "CardContainer.h"
#include "QtDebug"
#include <QDateTime>
#include "UserAccess.h"

CardContainer::CardContainer(QString parentID, QObject *parent) : NestedArrayContainer("users", parentID, "cards", parent)
{
}

CardContainer::~CardContainer()
{
}

QVariantMap CardContainer::insert(QVariant data)
{
    Card card(data);
    return _appendItem(card.toVariant());
}

void CardContainer::_insert(QVariantList data)
{
    QListIterator<QVariant> it(data);
    while(it.hasNext())
    {
        QVariantMap item = it.next().toMap();
        qlonglong expiration = item["expirationDate"].toMap()["$date"].toLongLong();
        item["expirationDate"] = QDateTime::fromMSecsSinceEpoch(expiration);

        qlonglong creation = item["lastUsage"].toMap()["$date"].toLongLong();
        item["lastUsage"] = QDateTime::fromMSecsSinceEpoch(creation);
        _data << item;
    }
}

QVariantMap CardContainer::setProperty(int index, QString property, QVariant value)
{

    if(property == "expirationDate" || property == "expires" || property == "active"  || property == "type" || property == "lastUsage")
        return _setProperty(index, property, value);

    return QVariantMap();
}

bool CardContainer::canRead(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_SEE_USERS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);
}

bool CardContainer::canWrite(iIdentityPtr user) const
{
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_MODIFY_USERS) || user->isAuthorizedTo("isAdmin") || user->isAuthorizedTo(LAB_ADMIN);
}

