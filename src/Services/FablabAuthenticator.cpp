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


#include "FablabAuthenticator.h"
#include "../Database/UserAccess.h"
#include "Server/Authentication/User.h"
#include "Server/Authentication/DefaultAuthenticator.h"
#include "ServiceUserAccess.h"
#include <QDebug>

Q_GLOBAL_STATIC(FablabAuthenticator, fablabAuthenticator);

FablabAuthenticator::FablabAuthenticator(QObject* parent) : IAuthenticator(parent)
{
    QString servicePass = QProcessEnvironment::systemEnvironment().value("SERVICE_USER_PASS", "");
    QString maintainancePass = QProcessEnvironment::systemEnvironment().value("MAINTAINANCE_USER_PASS", "");


    if(!maintainancePass.isEmpty())
    {
        userPtr maintainanceUser =  DefaultAuthenticator::instance()->createUser("maintainance", maintainancePass);// new ServiceUser("service", servicePass, this);
        maintainanceUser->setUserPermission("isAdmin",true,false);
        maintainanceUser->setUserPermission("lab.admin", true);
        QVariantMap userData;
        userData["role"] = USER_ROLE_ADMIN_STRING;
        maintainanceUser->setUserData(userData);
        addUser("maintainance", iUserPtr(maintainanceUser));
    }
}

iUserPtr FablabAuthenticator::getUser(QString userID)
{
    fablabUserPtr user = UserAccess::instance()->getUserWithEMail(userID);
    if(!user.isNull())
        return qSharedPointerCast<IUser>(user);

    return _serviceUsers.value(userID, nullptr);

}

void FablabAuthenticator::addUser(QString userID, iUserPtr user)
{
    _serviceUsers.insert(userID, user);
}

FablabAuthenticator *FablabAuthenticator::instance()
{
    return fablabAuthenticator;
}
