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


#include "UserAccess.h"
#include <QDebug>
#include "NestedArrayContainer.h"
#include "../Database/PermissionContainer.h"
#include "../Database/CardContainer.h"

Q_GLOBAL_STATIC(UserAccess, userAccess);

UserAccess::UserAccess(QObject *parent) : QObject(parent)
{
}

UserAccess *UserAccess::instance()
{
    return userAccess;
}

QList<fablabUserPtr> UserAccess::getUsers(Query filter)
{
    auto projection = document{};

    //setup query - opt-out some results which aren't relevant for the user info
    projection << "permissions" << 0 << "transactions" << 0 << "cards" << 0 << "groups" << 0;
    QVariantList users = MongoDB::instance()->selectProjection("users", projection.view(), filter);
    QListIterator<QVariant> it(users);
    QList<fablabUserPtr> userList;

    //iterate ove results
    while(it.hasNext())
    {
       userList << getOrAddUser(it.next().toMap());
    }

    return userList;
}

fablabUserPtr UserAccess::getUser(QString userID)
{
    fablabUserPtr ptr;
    if(_users.contains(userID))
    {
         ptr = _users.value(userID).toStrongRef();
         if(!ptr.isNull())
         {
             return ptr;
         }
    }

    auto projection = document{};
    projection << "permissions" << 0 << "transactions" << 0 << "cards" << 0;

    document filter = document{};

    filter << "_id"<< bsoncxx::oid(userID.toStdString());
    QVariantList users = MongoDB::instance()->selectProjection("users", projection.view(), filter.extract());

    if(users.count() > 0)
    {
        ptr = fablabUserPtr(new FablabUser(users[0].toMap(), this));
        connect(ptr.data(), &FablabUser::deleted, this, &UserAccess::userObjectDestroyed);
        _users.insert(userID, ptr.toWeakRef());
    }

    return ptr;
}

fablabUserPtr UserAccess::getUserWithCard(QString cardID)
{
    fablabUserPtr ptr;
    auto projection = document{};
    projection << "permissions" << 0 << "transactions" << 0 ;

    QVariantMap query;
    query["cards.cardID"] = cardID;
    document filter = document{};
    QVariantList users = MongoDB::instance()->selectProjection("users", projection.view(), MongoDB::queryFromVariant(query).view());

    if(users.count() > 1 || users.count() == 0)
        return ptr;

    return getOrAddUser(users.first().toMap());
}

fablabUserPtr UserAccess::getUserWithEMail(QString eMail)
{
    fablabUserPtr ptr;
    auto projection = document{};
    projection << "permissions" << 0 << "transactions" << 0 <<"cards" << 0 ;

    QVariantMap query;
    query["mail"] = eMail;
    document filter = document{};
    QVariantList users = MongoDB::instance()->selectProjection("users", projection.view(), MongoDB::queryFromVariant(query).view());

    if(users.count() > 1 || users.count() == 0)
        return ptr;

    return getOrAddUser(users.first().toMap());
}

bool UserAccess::deleteUser(QString userID)
{
	if ( userID.isEmpty() )
		return false;

    fablabUserPtr ptr;
    if(_users.contains(userID))
    {
         ptr = _users.value(userID).toStrongRef();
         if(!ptr.isNull())
         {
             ptr->deleteUser();
         }
         _users.remove(userID);
    }
	else
	{
		// if user is not in _users, ptr is null and the userDeleted signal emmits a nullptr
		// for the signal to be meaningful in this case, we temporarily have to load the user
		ptr = getUser(userID);
		if ( ptr != nullptr)
		{
			ptr->deleteUser();
		}
	}

    int deletedCount = MongoDB::instance()->removeDocument("users", userID);

    if(deletedCount > 0)
    {
		if ( ptr != nullptr )
		{
			Q_EMIT userDeleted(ptr);
		}

        return true;
    }
    return false;
}

void UserAccess::setLastLogin(QString userID)
{
    fablabUserPtr userPtr = _users.value(userID);
    if(!userPtr.isNull())
    {
        userPtr->setLastLogin(QDateTime::currentDateTime());
    }
    else
    {
        MongoDB::instance()->changeProperty("users", userID, "lastLogin", QDateTime::currentDateTime());
    }
}


UserAccess::AddUserResult UserAccess::insertUser(QVariantMap user)
{
    AddUserResult result;
    QVariantMap query;
    query["mail"] = user["mail"].toString();

	if ( !user["mail"].toString().isEmpty() )
	{
		if(MongoDB::instance()->select("users", MongoDB::queryFromVariant(query).view()).length() != 0)
		{
			result.errCode = AddUserResult::ALREADY_EXISTS;
			qWarning()<<"User already exists.";
			return result;
		}
	}
    else
    {
        result.errCode = AddUserResult::INVALID_DATA;
        qWarning()<<"Invalid or incomplete Data";
        return result;
    }

    fablabUserPtr userObj = fablabUserPtr(new FablabUser(user, this));
    userObj->setCreation(QDateTime::currentDateTime());
    QString uuid =  MongoDB::instance()->insertDocument("users", userObj->toVariant());
    userObj->setUuid(uuid);

    if(!uuid.isEmpty())
    {
        result.uuid = uuid;
        result.errCode = AddUserResult::OK;
        connect(userObj.data(), &FablabUser::deleted, this, &UserAccess::userObjectDestroyed);
        _users.insert(uuid, userObj.toWeakRef());
        Q_EMIT userInserted(userObj);
    }

    return result;
}



fablabUserPtr UserAccess::getOrAddUser(QVariantMap userData)
{
    fablabUserPtr ptr;
    QString uuid = userData["_id"].toMap()["$oid"].toString();
    if(_users.contains(uuid))
    {
        ptr = _users.value(uuid).toStrongRef();
        if(!ptr.isNull())
        {
            return ptr;
        }
    }

    ptr = fablabUserPtr(new FablabUser(userData, this));
    connect(ptr.data(), &FablabUser::deleted, this, &UserAccess::userObjectDestroyed);
    _users.insert(uuid, ptr.toWeakRef());
    return ptr;
}

void UserAccess::userObjectDestroyed(QString uuid)
{
    _users.remove(uuid);
}
