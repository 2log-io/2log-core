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


#include "ServiceUserAccess.h"
#include <QVariant>
#include "Database/UserAccess.h"
#include "Server/Authentication/User.h"
#include "Server/Authentication/DefaultAuthenticator.h"
#include "MailClient/MailManager.h"
#include <QDebug>



ServiceUserAccess::ServiceUserAccess(QObject *parent) : QObject(parent)
{
}



QVariantMap ServiceUserAccess::addServiceUser(QString userID,  QString userName, QString eMail, UserRole userLevel, QString token)
{
    QVariantMap answer;

    if(userLevel == USER_ROLE_UNDEFINED || eMail.isEmpty() || userID.isEmpty() || userName.isEmpty())
    {
        answer["command"] = "user:add:failed";
        answer["errrorcode"] = AuthenticationService::InvalidData;
        return answer;
    }

    AuthenticationService::ErrorCode err;

    QString password = getRandomString(8);

    QVariantMap eMailData;
    eMailData["userName"] = userName;
    eMailData["login"] = userID;
    eMailData["password"] = password;


    iUserPtr user = DefaultAuthenticator::instance()->createUser(userID, password, &err);


    if( err != AuthenticationService::NoError)
    {
        answer["command"] = "user:add:failed";
        answer["errrorcode"] = err;
        return answer;
    }


    // create a user object
    QSharedPointer<User> userObjPtr = qSharedPointerCast<User>(user);
    if(userObjPtr.isNull())
        return QVariantMap();


    // setup user object
    userObjPtr->setEMail(eMail);
    userObjPtr->setUserName(userName);



    setUserRole(userLevel, userObjPtr);

    // add user object
    DefaultAuthenticator::instance()->addUser(userObjPtr, token, &err);
    if(err  == AuthenticationService::NoError)
    {
        answer["command"] = "user:add:success";
        answer["errrorcode"] = err;
        qInfo()<<"User "+user->identityID()+" added successfully.";
        MailManager manager;
        manager.sendMailFromTemplate(eMail, "welcome-admin", eMailData);
    }
    else
    {
        answer["command"] = "user:add:failed";
        answer["errrorcode"] = err;
    }

    return answer;
}


QString ServiceUserAccess::getRandomString(int count)
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
   QString randomString;
   for(int i=0; i < count; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

bool ServiceUserAccess::setUserRole(ServiceUserAccess::UserRole role, userPtr user)
{
    if(user.isNull())
        return false;

    QVariantMap userData;
    switch(role)
    {
        case USER_ROLE_ADMIN:
        {
            userData["role"] = USER_ROLE_ADMIN_STRING;
            user->setUserData(userData);
            user->setUserPermission(IS_ADMIN,            true, false);
            user->setUserPermission(LAB_ADMIN,           true, false);
            user->setUserPermission(SEE_DEVICES,         true, false);
            user->setUserPermission(MANAGE_DEVICES,      true, false);

            user->setUserPermission(LAB_MODIFY_DEVICES,   true, false);
            user->setUserPermission(LAB_MODIFY_USERS,     true, false);
            user->setUserPermission(LAB_MODIFY_GROUPS,    true, false);

            user->setUserPermission(LAB_SEE_LOGS,         true, false);
            user->setUserPermission(LAB_SEE_USERS,        true, false);
            user->setUserPermission(LAB_SEE_DEVICES,      true, false);
            user->setUserPermission(LAB_SEE_GROUPS,       true, false);
            user->setUserPermission(LAB_SEND_BILLS,      true, false);
            user->setUserPermission(LAB_SEE_LOGS,         true, true);
            return true;
        }

        case USER_ROLE_EMPLOYEE:// Rüdiger
        {
            userData["role"] = USER_ROLE_EMPLOYEE_STRING;
            user->setUserData(userData);
            user->setUserPermission(IS_ADMIN,            false, false);
            user->setUserPermission(LAB_ADMIN,            true, false);
            user->setUserPermission(MANAGE_DEVICES,       true, false);
            user->setUserPermission(SEE_DEVICES,          true, false);



            user->setUserPermission(LAB_MODIFY_DEVICES,   true, false);
            user->setUserPermission(LAB_MODIFY_USERS,     true, false);
            user->setUserPermission(LAB_MODIFY_GROUPS,    true, false);
            user->setUserPermission(LAB_SEE_USERS,        true, false);
            user->setUserPermission(LAB_SEE_DEVICES,      true, false);
            user->setUserPermission(LAB_SEE_GROUPS,       true, false);
            user->setUserPermission(LAB_SEND_BILLS,      true, false);
            user->setUserPermission(LAB_SEE_LOGS,         true, true);
            return true;
        }


        case USER_ROLE_OFFICE: // Dagmar
        {
            userData["role"] = USER_ROLE_OFFICE_STRING;
            user->setUserData(userData);
            user->setUserPermission(IS_ADMIN,           false, false); //registrieren neuer 2log Nutzer User
            user->setUserPermission(MANAGE_DEVICES,     false, false);
            user->setUserPermission(SEE_DEVICES ,       false, false);
            user->setUserPermission(READ_DEVICES,       true,  false);
            user->setUserPermission(LAB_ADMIN,          false, false);

            user->setUserPermission(LAB_MODIFY_DEVICES, false, false);
            user->setUserPermission(LAB_MODIFY_USERS,   true,  false);
            user->setUserPermission(LAB_MODIFY_GROUPS,  false, false);
            user->setUserPermission(LAB_SEE_USERS,      true,  false);
            user->setUserPermission(LAB_SEE_GROUPS,     true,  false);
            user->setUserPermission(LAB_SEE_LOGS,       false, false);
            user->setUserPermission(LAB_SEND_BILLS,      true, false);
            user->setUserPermission(LAB_SEE_DEVICES,    false, true);
            return true;
        }


        case USER_ROLE_CASHIER: // Dagmar
        {
            userData["role"] = USER_ROLE_CASHIER_STRING;
            user->setUserData(userData);
            user->setUserPermission(IS_ADMIN,            false, false); //registrieren neuer 2log Nutzer User
            user->setUserPermission(MANAGE_DEVICES,      true, false);
            user->setUserPermission(SEE_DEVICES ,        false, false);
            user->setUserPermission(READ_DEVICES,        false, false);
            user->setUserPermission(LAB_ADMIN,           false, false);

            user->setUserPermission(LAB_MODIFY_DEVICES,  false, false);
            user->setUserPermission(LAB_MODIFY_USERS,    false,  false);
            user->setUserPermission(LAB_MODIFY_GROUPS,   false, false);
            user->setUserPermission(LAB_SEE_USERS,       true,  false);
            user->setUserPermission(LAB_SEE_GROUPS,      false,  false);
            user->setUserPermission(LAB_SEE_LOGS,        false, false);
            user->setUserPermission(LAB_SEND_BILLS,      true, true);
            return true;
        }

        default: return false;
    }
}

bool ServiceUserAccess::changeUserLevel(ServiceUserAccess::UserRole role, QString userID)
{
    userPtr user = qSharedPointerCast<User>(DefaultAuthenticator::instance()->getUser(userID));
    if(!user.isNull())
    {
        return setUserRole(role, user);
    }
    return false;
}

ServiceUserAccess::UserRole ServiceUserAccess::getRole(QString string)
{
    UserRole role;
    role = ServiceUserAccess::USER_ROLE_UNDEFINED;
    if(string == USER_ROLE_ADMIN_STRING)
        role = ServiceUserAccess::USER_ROLE_ADMIN;

    if( string ==  USER_ROLE_EMPLOYEE_STRING) // Rüdiger
        role = ServiceUserAccess::USER_ROLE_EMPLOYEE;

    if( string ==  USER_ROLE_OFFICE_STRING) // Rüdiger
        role = ServiceUserAccess::USER_ROLE_OFFICE;

    if( string ==  USER_ROLE_CASHIER_STRING) // Rüdiger
        role = ServiceUserAccess::USER_ROLE_CASHIER;
    return role;
}

void ServiceUserAccess::setRightLevels()
{
    QVector<userPtr> users = DefaultAuthenticator::instance()->getUsers();
    for(int i = 0; i < users.count(); i++)
    {
        userPtr user = users.at(i);
        QString role = user->getUserData()["role"].toString();
        setUserRole(getRole(role), user);
    }
}
