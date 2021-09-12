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


#include "UserResource.h"
#include "Database/UserAccess.h"
#include "Database/MongoDB.h"

UserResource::UserResource(QVariantMap parameters, QObject *parent) : ListResource(nullptr, parent)
{
    setFilter(parameters);
    setDynamicContent(true);
    setAllowUserAccess(false);
    connect(UserAccess::instance(), &UserAccess::userInserted, this, &UserResource::newUserInserted);
    connect(UserAccess::instance(), &UserAccess::userDeleted, this, &UserResource::userDeleted);
}

bool UserResource::setFilter(QVariantMap filter)
{
    QListIterator<fablabUserPtr> it(_users);

    _filter = filter;
    while(it.hasNext())
       disconnectUser(it.next());

    QList<fablabUserPtr> users;
    if(filter.contains("balanceLessThen"))
    {
        int balanceFilter = filter["balanceLessThen"].toInt();
        users =UserAccess::instance()->getUsers(make_document(kvp("balance", make_document(kvp("$lt", balanceFilter)))));
    }
    else if(filter.contains("activeFrom") || filter.contains("activeUntil"))
    {
        /*
        auto query = document{} << "lastlogin" << open_document <<
                                   "$gt" << 50 <<
                                   "$lte" << 100
                                 << close_document << finalize;
        */
    }
    else
    {
        users =  UserAccess::instance()->getUsers(MongoDB::queryFromVariant(filter).view());
    }

    Q_EMIT listDeleted(iUserPtr());

    _users = users;
    QListIterator<fablabUserPtr> it2(_users);

    while(it2.hasNext())
       connectUser(it2.next());

    reIndex();
    Q_EMIT reset();
   //  Q_EMIT listAppended(getListData(), iUserPtr());
    return true;
}

QVariantList UserResource::getListData() const
{
    _mutex.lockForRead();
    QVariantList items;
    QListIterator<fablabUserPtr> it(_users);

    while(it.hasNext())
    {
        fablabUserPtr user = it.next();
        if(!user.isNull())
            items << createTemplate(user->toVariant());
    }
    _mutex.unlock();
    return items;
}

bool UserResource::isPermittedToRead(QString token) const
{
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
     if(user.isNull() || !( user->isAuthorizedTo(LAB_ADMIN) || user->isAuthorizedTo(IS_ADMIN) || user->isAuthorizedTo(LAB_SEE_USERS)))
        return false;

    return true;
}

int UserResource::getCount() const
{
    return  _users.count();
}

QVariant UserResource::getItem(int idx, QString uuid) const
{
    Q_UNUSED(uuid)
    _mutex.lockForRead();
    QVariant item = createTemplate(_users.at(idx)->toVariant());
    _mutex.unlock();
    return item;
}

QVariantMap UserResource::createTemplate(QVariantMap data) const
{
    QVariantMap item;
    item["data"] = data;
    item["uuid"] = data["uuid"];
    return item;
}

void UserResource::newUserInserted(fablabUserPtr user)
{
    if(_filter.contains("balanceLessThen"))
    {
        int balanceFilter = _filter["balanceLessThen"].toInt();
        if(user->getBalance() >= balanceFilter)
            return;
    }
    _users.prepend(user);
    connectUser(user);
    reIndex();
    Q_EMIT itemInserted(createTemplate(user->toVariant()),0, iUserPtr());
}

void UserResource::userDeleted(fablabUserPtr user)
{
    if(user.isNull())
    {
        qDebug()<<"is nullptr";
        return;
    }

    int index = indexOf(user.data());

    if(index < 0)
    {
        qDebug()<<"not in list";
        return;
    }

    Q_EMIT itemRemoved(index, nullptr, iUserPtr());
    qDebug()<<"deleted! "<<index ;
    _users.removeAll(user);
    reIndex();
}

void UserResource::reIndex()
{
    _indexMap.clear();
    for(int i = 0; i < _users.size(); i++)
    {
        _indexMap.insert(_users[i].data(), i);
    }
}

void UserResource::connectUser(fablabUserPtr user)
{
    connect(user.data(), &FablabUser::nameChanged, this, &UserResource::nameChanged);
    connect(user.data(), &FablabUser::balanceChanged, this, &UserResource::balanceChanged);
    connect(user.data(), &FablabUser::eMailChanged, this, &UserResource::eMailChanged);
    connect(user.data(), &FablabUser::stateChanged, this, &UserResource::stateChanged);
    connect(user.data(), &FablabUser::lastLoginChanged, this, &UserResource::lastLoginChanged);
    connect(user.data(), &FablabUser::roleChanged, this, &UserResource::roleChanged);
    connect(user.data(), &FablabUser::surnameChanged, this, &UserResource::surnameChanged);
    connect(user.data(), &FablabUser::courseChanged, this, &UserResource::courseChanged);
}

void UserResource::disconnectUser(fablabUserPtr user)
{
    disconnect(user.data(), &FablabUser::nameChanged, this, &UserResource::nameChanged);
    disconnect(user.data(), &FablabUser::balanceChanged, this, &UserResource::balanceChanged);
    disconnect(user.data(), &FablabUser::eMailChanged, this, &UserResource::eMailChanged);
    disconnect(user.data(), &FablabUser::stateChanged, this, &UserResource::stateChanged);
    disconnect(user.data(), &FablabUser::lastLoginChanged, this, &UserResource::lastLoginChanged);
    disconnect(user.data(), &FablabUser::roleChanged, this, &UserResource::roleChanged);
    disconnect(user.data(), &FablabUser::surnameChanged, this, &UserResource::surnameChanged);
    disconnect(user.data(), &FablabUser::courseChanged, this, &UserResource::courseChanged);
}

int UserResource::indexOf(QObject *ptr)
{
    int idx = _indexMap.value(ptr, -1);
    if(idx < 0 || _users[idx].data() != ptr)
        reIndex();
    else
        return  idx;

    return _indexMap.value(ptr, -1);
}

void UserResource::nameChanged(QString uuid, QString name)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("name", name, idx,  uuid, iUserPtr(), 0);
}

void UserResource::eMailChanged(QString uuid, QString mail)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("mail", mail, idx,  uuid, iUserPtr(), 0);
}

void UserResource::lastLoginChanged(QString uuid, QDateTime lastLogin)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("lastLogin", lastLogin, idx,  uuid, iUserPtr(), 0);
}

void UserResource::balanceChanged(QString uuid, int balance)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("balance", QVariant(balance), idx,  uuid, iUserPtr(), 0);
}

void UserResource::stateChanged(QString uuid, FablabUser::UserState state)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("state", state, idx,  uuid, iUserPtr(), 0);
}

void UserResource::surnameChanged(QString uuid, QString surname)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("surname", surname, idx,  uuid, iUserPtr(), 0);
}
void UserResource::roleChanged(QString uuid, QString role)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("role", role, idx,  uuid, iUserPtr(), 0);
}
void UserResource::courseChanged(QString uuid, QString course)
{
    int idx = indexOf(sender());
    if(idx >= 0)
        Q_EMIT propertySet("course", course, idx,  uuid, iUserPtr(), 0);
}
