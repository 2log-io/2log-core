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


#include "UserObjectResource.h"
#include <QDebug>

UserObjectResource::UserObjectResource(fablabUserPtr user, QObject* parent) : ObjectResource(nullptr, parent),
    _userPtr(user)
{
    setDynamicContent(true);
    connect(_userPtr.data(), &FablabUser::userPropertyChanged, this, &UserObjectResource::userPropertyChanged);
}

QVariantMap UserObjectResource::getObjectData() const
{
    if(_userPtr.isNull())
        return QVariantMap();
    QVariantMap data;

    QVariantMap user =  _userPtr->toVariant();
    QMapIterator<QString, QVariant>it(user);
    while(it.hasNext())
    {
        it.next();
        QVariantMap prop;
        prop["data"] = it.value();
        data.insert(it.key(), prop );
    }

    return data;
}

IResource::ModificationResult UserObjectResource::setProperty(QString name, const QVariant &value, QString token)
{
    ModificationResult result;
    iIdentityPtr executiveUser = AuthenticationService::instance()->validateToken(token);

    if(executiveUser.isNull() || ! (executiveUser->isAuthorizedTo(LAB_SERVICE) || executiveUser->isAuthorizedTo(LAB_ADMIN) || executiveUser->isAuthorizedTo(LAB_MODIFY_USERS)))
    {
        result.error = ResourceError::PERMISSION_DENIED;
        return result;
    }

    QVariantMap data;
    data["data"] = value;
    data["lastupdate"] = QDateTime::currentMSecsSinceEpoch();
    result.data = data;

    if(name == "name")
    {
        _userPtr->setName(value.toString());
        return result;
    }
    if(name == "course")
    {
        _userPtr->setCourse(value.toString());
        return result;
    }

    if(name == "surname")
    {
        _userPtr->setSurname(value.toString());
        return result;
    }

    if(name == "role")
    {
        _userPtr->setRole(value.toString());
        return result;
    }

    if(name == "mail")
    {
        _userPtr->setEMail(value.toString());
        return result;
    }

    result.error = INVALID_PARAMETERS;
    return result;
}

void UserObjectResource::userPropertyChanged(QString prop, QVariant value)
{
    Q_EMIT propertyChanged(prop, value, nullptr);
}
