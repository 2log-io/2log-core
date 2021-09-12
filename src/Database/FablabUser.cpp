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


#include "FablabUser.h"
#include "UserAccess.h"
#include "QDebug"
#include <QReadLocker>

QVariantMap FablabUser::toVariant() const
{
    QReadLocker locker(&_lock);
    QVariantMap user;
    user["name"] = _name;
    user["mail"] = _eMail;
    user["balance"] = _balance;
    user["lastLogin"] = _lastLogin;
    user["creation"] = _creation;
    user["state"] = _state;
    user["role"] = _role;
    user["surname"] = _surname;
    user["course"] = _course;
    if(!_uuid.isEmpty())
        user["uuid"] = _uuid;
    return user;
}

QString FablabUser::getName() const
{
    QReadLocker locker(&_lock);
    return _name;
}

void FablabUser::setName(const QString &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;
    MongoDB::instance()->changeProperty("users", _uuid, "name", value);
    _name = value;
    Q_EMIT nameChanged(_uuid, _name);
    Q_EMIT userPropertyChanged("name", value);
}

QString FablabUser::getSurname() const
{
    QReadLocker locker(&_lock);
    return _surname;
}

void FablabUser::setSurname(const QString &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "surname", value);
    _surname = value;
    Q_EMIT surnameChanged(_uuid, _surname);
    Q_EMIT userPropertyChanged("surname", value);
}

QString FablabUser::getCourse() const
{
    QReadLocker locker(&_lock);
    return _course;
}

void FablabUser::setCourse(const QString &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "course", value);
    _course = value;
    Q_EMIT courseChanged(_uuid, _course);
    Q_EMIT userPropertyChanged("course", value);
}

void FablabUser::setRole(const QString &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "role", value);
    _role = value;
    Q_EMIT roleChanged(_uuid, _role);
    Q_EMIT userPropertyChanged("role", value);
}

QString FablabUser::getRole() const
{
    QReadLocker locker(&_lock);
    return _role;
}

QString FablabUser::getEMail() const
{
    QReadLocker locker(&_lock);
    return _eMail;
}

void FablabUser::setEMail(const QString &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "mail", value);
    _eMail = value;
    Q_EMIT eMailChanged(_uuid, value);
    Q_EMIT userPropertyChanged("mail", value);
}

QString FablabUser::getUuid() const
{
    QReadLocker locker(&_lock);
    return _uuid;
}


int FablabUser::getBalance() const
{
    QReadLocker locker(&_lock);
    return _balance;
}

void FablabUser::setBalance(int value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "balance", value);
    _balance = value;
    Q_EMIT balanceChanged(_uuid, _balance);
    Q_EMIT userPropertyChanged("balance", value);
}

QDateTime FablabUser::getLastLogin() const
{
    QReadLocker locker(&_lock);
    return _lastLogin;
}

void FablabUser::setLastLogin(const QDateTime &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "lastLogin", value);
    _lastLogin = value;
    Q_EMIT lastLoginChanged(_uuid, _lastLogin);
    Q_EMIT userPropertyChanged("lastLogin", value);
}

QDateTime FablabUser::getCreation() const
{
    QReadLocker locker(&_lock);
    return _creation;
}

bool FablabUser::setPassword(QString newPassword)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return false;

    QString hash = generateHash(newPassword);

    MongoDB::instance()->changeProperty("users", _uuid, "passHash", hash);
    _passwordHash = hash;
    return true;
}

void FablabUser::setCreation(const QDateTime &value)
{
    QWriteLocker locker(&_lock);
    _creation = value;
}

FablabUser::UserState FablabUser::getState() const
{
    QReadLocker locker(&_lock);
    return _state;
}

void FablabUser::setState(const FablabUser::UserState &value)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    MongoDB::instance()->changeProperty("users", _uuid, "state", value);
    _state = value;
    Q_EMIT stateChanged(_uuid, _state);
    Q_EMIT userPropertyChanged("state", value);
}

bool FablabUser::checkPassword(QString password)
{
    QReadLocker locker(&_lock);
    if(password.isEmpty() || _passwordHash.isEmpty())
        return false;

    return generateHash(password) == _passwordHash;
}

bool FablabUser::isAuthorizedTo(QString permission)
{
    QReadLocker locker(&_lock);
    if(_state == DELETED)
        return false;

    return permission == SEE_OWN_USER_DATA;
}

bool FablabUser::deleteUser()
{
    this->setState(DELETED);
    return true;
}


QString FablabUser::identityID() const
{
    QReadLocker locker(&_lock);
    return _uuid;
}

QString FablabUser::userLogin() const
{
    return _eMail;
}


void FablabUser::setUuid(QString uuid)
{
    QWriteLocker locker(&_lock);
    if(_state == DELETED)
        return;

    _uuid = uuid;
    Q_EMIT uuidChanged(uuid);
}

IUser::UserData FablabUser::userData() const
{
    UserData user;
    user.name = _name+" "+_surname;
    user.email = _eMail;
    user.userID = _eMail;
    user.lastActivity = lastActivity();
    QVariantMap userData;
    userData["role"] = "member";
    user.userData = userData;
    return user;
}

FablabUser::FablabUser(QVariantMap userMap, QObject *parent) : IUser(parent)
{
    _name = userMap["name"].toString();
    _surname = userMap["surname"].toString();
    _course = userMap["course"].toString();
    _role = userMap["role"].toString();
    _eMail = userMap["mail"].toString() ;
    _balance = userMap["balance"].toInt() ;
    _passwordHash = userMap["passHash"].toString() ;
    _state = static_cast<UserState>(userMap["state"].toInt());

    // TODO Refactor - remove MongoDB specific code; should be wrapped somewhere else
    _uuid = userMap["_id"].toMap()["$oid"].toString();
    _lastLogin = QDateTime::fromMSecsSinceEpoch(userMap["lastLogin"].toMap()["$date"].toLongLong());
    _creation = QDateTime::fromMSecsSinceEpoch(userMap["creation"].toMap()["$date"].toLongLong());
}

FablabUser::~FablabUser()
{
    Q_EMIT deleted(_uuid);
}
