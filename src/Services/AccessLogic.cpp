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


#include <QJsonDocument>
#include <QDebug>
#include "AccessLogic.h"
#include "../Database/LogAccess.h"
#include "../Database/UserAccess.h"
#include "../Database/GroupAccess.h"
#include "../Database/PermissionContainer.h"
#include "../Database/GroupPermissionContainer.h"
#include "../Database/CardContainer.h"
#include "Database/MongoDB.h"
#include "MailClient/MailManager.h"
#include "Server/Authentication/AuthentificationService.h"

AccessLogic::AccessLogic(QObject *parent) : QObject(parent)
{
}


QVariant AccessLogic::checkPermission(QString cardID, QString resourceID)
{
    QVariantMap returnValue;
    returnValue["errorCode"] = -1;
    returnValue["errorString"] = "No permission";

    QVariantMap query;
    query["cards.cardID"] = cardID;
   // query["permissions.resourceID"] = resourceID;
    QVariantList result = MongoDB::instance()->select("users", MongoDB::queryFromVariant(query).view());

    if(result.count() != 1)
    {
        returnValue["errorCode"] = -1;
        returnValue["errorString"] = "No data available.";
        return returnValue;
    }

    QVariantMap user = result.at(0).toMap();
    QString userID = user["_id"].toMap()["$oid"].toString();
    QString userName = user["name"].toString()+" "+user["surname"].toString();
    int balance = user["balance"].toInt();
    QString userRole = user["role"].toString();

    returnValue["userID"] = userID;
    returnValue["userName"] = userName;
    returnValue["balance"] = balance;
    returnValue["userRole"] = userRole;

    if(user["state"].toInt() == FablabUser::DISABLED)
    {
        returnValue["errorCode"] = -2;
        returnValue["errorString"] = "User is disabled.";
        return returnValue;
    }

    QVariantList cards = user["cards"].toList();
    QListIterator<QVariant> it(cards);
    while(it.hasNext())
    {
        CardContainer::Card card(it.next());
        if(card._cardID == cardID)
        {
            if(!card._enabled)
            {
                returnValue["errorCode"] = -3;
                returnValue["errorString"] = "Card is disabled.";
                return returnValue;
            }

            break;
        }
    }


    bool success = false;


    // check groups

    QVariantList groups = user["groups"].toList();
    QListIterator<QVariant> it3(groups);
    while(it3.hasNext())
    {
        QVariantMap groupPermVariant = it3.next().toMap();
        GroupPermissionContainer::Group groupPermission(groupPermVariant);
        groupPtr groupObj = GroupAccess::instance()->getGroup(groupPermission._groupID);
        if(!groupObj.isNull())
        {
            if(groupObj->hasPermission(resourceID, "device"))
            {
                if (!groupPermission._enabled)
                {
                    returnValue["errorCode"] = -10;
                    returnValue["errorString"] = "Group permission is disabled.";
                    break;
                }
                QDateTime expirationTime = QDateTime::fromMSecsSinceEpoch(groupPermVariant["expirationDate"].toMap()["$date"].toLongLong());
                if(groupPermission._expires && (expirationTime.toMSecsSinceEpoch() < QDateTime::currentDateTime().toMSecsSinceEpoch()))
                {
                    returnValue["errorCode"] = -11;
                    returnValue["errorString"] = "Group permission is expired.";
                    break;
                }

                success = true;
            }
        }
    }


    // check single permissions
    QVariantList permissions = user["permissions"].toList();
    QListIterator<QVariant> it2(permissions);
    while(it2.hasNext())
    {
        QVariantMap permissionVariant = it2.next().toMap();
        PermissionContainer::Permission perm(permissionVariant);
        if(perm._resourceID == resourceID)
        {
            if (!perm._enabled)
            {
                returnValue["errorCode"] = -10;
                returnValue["errorString"] = "Permission is disabled.";
                success = false;
                break;
            }

           QDateTime expirationTime = QDateTime::fromMSecsSinceEpoch(permissionVariant["expirationDate"].toMap()["$date"].toLongLong());
            qDebug()<<expirationTime<<"   "<<QDateTime::currentDateTime();
            if(perm._expires && (expirationTime.toMSecsSinceEpoch() < QDateTime::currentDateTime().toMSecsSinceEpoch()))
            {
                returnValue["errorCode"] = -11;
                returnValue["errorString"] = "Permission is expired.";
                success = false;
                break;
            }

            success = true;
            break;
        }
    }

    if(success)
    {
        returnValue["errorCode"] = 0;
		returnValue["errorString"] = "";
        UserAccess::instance()->setLastLogin(userID);
    }
    return returnValue;
}

QVariant AccessLogic::getUserForCardID(QString cardID)
{
    QVariantMap returnValue;

    returnValue["errorCode"] = 0;
    QVariantMap query;
    query["cards.cardID"] = cardID;
    QVariantList result = MongoDB::instance()->select("users", MongoDB::queryFromVariant(query).view());

    if(result.count() != 1)
    {
        returnValue["errorCode"] = -1;
        returnValue["errorString"] = "No data available.";
        return returnValue;
    }


    QVariantMap user = result.at(0).toMap();
    QString userID = user["_id"].toMap()["$oid"].toString();
    QString userName = user["name"].toString();
    int balance = user["balance"].toInt();

    returnValue["userID"] = userID;
    returnValue["userName"] = userName;
    returnValue["balance"] = balance;
    return returnValue;
}

QVariant AccessLogic::addOrUpdateUser(QVariantMap user, QVariantList permissions, QVariantMap card, QVariantList groups)
{
    QString eMail = user["mail"].toString();
    QString course = user["course"].toString();
    QString role = user["role"].toString();
    QString name = user["name"].toString();
    int balance = user["balance"].toInt();
    QString surname = user["surname"].toString();

    QString cardID = card["cardID"].toString();

    QVariantMap returnValue;
    returnValue["errorCode"] = 1;

    fablabUserPtr userObj;
    bool foundWithCard = false;
    userObj = UserAccess::instance()->getUserWithEMail(eMail);

    if(userObj.isNull())
    {
        userObj = UserAccess::instance()->getUserWithCard(cardID);
        foundWithCard = true;
    }

    if(!userObj.isNull())
    {
        // user already exists
        if(userObj->getEMail() == eMail || (userObj->getSurname() == user["surname"].toString() && userObj->getName() == user["name"].toString()))
        {
            if(!eMail.isEmpty())
                userObj->setEMail(eMail);

            if(!name.isEmpty())
                userObj->setName(name);

            if(!surname.isEmpty())
                userObj->setSurname(surname);

            if(!role.isEmpty())
                userObj->setRole(role);

            if(!course.isEmpty())
                userObj->setCourse(course);

            if(balance != 0)
            {
                balance = balance + userObj->getBalance();
                userObj->setBalance(balance);
            }
        }



        if(permissions.count() > 0)
        {
            nestedArrayPtr permissionContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", userObj->getUuid(), "permissions");
            if(permissionContainer.isNull())
            {
                returnValue["errorCode"] = -20;
                returnValue["errorString"] = "Backend error!";
                return returnValue;
            }

            //check if user has already the permission
            foreach (QVariant modifiedPermissions, permissions)
            {

                QVariantMap modifiedPermission = modifiedPermissions.toMap();
                for(int i = 0; i < permissionContainer->count(); i++)
                {
                    bool found = false;
                    QVariantMap group = permissionContainer->getItem(i)["data"].toMap();
                    //update group data when user has the group already
                    if(group["resourceID"].toString() == modifiedPermission["resourceID"].toString())
                    {

                        permissionContainer->setProperty(i,"expirationDate", modifiedPermission["expirationDate"]);
                        permissionContainer->setProperty(i,"expires", modifiedPermission["expires"]);
                        permissionContainer->setProperty(i,"active", modifiedPermission["active"]);
                        found = true;
                        break;
                    }

                    if(!found)
                        permissionContainer->insert(modifiedPermission);
                }
            }
        }

        //when user is not connected with the card and the card isn't already in the system add the card to the user
        QVariantMap query;
        query["cards.cardID"] = cardID;
        if(!foundWithCard && MongoDB::instance()->select("users", MongoDB::queryFromVariant(query).view()).count() <= 0)
        {

            nestedArrayPtr cardContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", userObj->getUuid(), "cards");
            if(cardContainer.isNull())
            {
                returnValue["errorCode"] = -30;
                returnValue["errorString"] = "Backend error!";
                return returnValue;
            }

            cardContainer->insert(card);
        }


        if(groups.count() > 0)
        {

            nestedArrayPtr groupContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", userObj->getUuid(), "groups");
            if(groupContainer.isNull())
            {
                returnValue["errorCode"] = -40;
                returnValue["errorString"] = "Backend error!";
                return returnValue;
            }

            //check if user has already the group
            foreach (QVariant modifiedGroup, groups)
            {
                QVariantMap modifiedGroupMap = modifiedGroup.toMap();
                bool found = false;
                for(int x = 0; x < groupContainer->count(); x++)
                {
                    QVariantMap group = groupContainer->getItem(x)["data"].toMap();
                    //update group data when user has the group already
                    if(group["groupID"].toString() == modifiedGroupMap["groupID"].toString())
                    {
                        groupContainer->setProperty(x,"expirationDate", modifiedGroupMap["expirationDate"]);
                        groupContainer->setProperty(x,"expires", modifiedGroupMap["expires"]);
                        groupContainer->setProperty(x,"active", modifiedGroupMap["active"]);
                        found = true;
                        break;
                    }
                }
                if(!found)
                    groupContainer->insert(modifiedGroupMap);
            }
        }
    }
    else
    {
        return addUser( user,  permissions,  card,  groups);
    }

    return returnValue;
}

QVariant AccessLogic::addUser(QVariantMap user, QVariantList permissions, QVariantMap card, QVariantList groups)
{
    QVariantMap returnValue;
    returnValue["errorCode"] = 0;
    QString cardID = card["cardID"].toString();
    if(!cardID.isEmpty())
    {
        QVariantMap query;
        query["cards.cardID"] = cardID;
        if(MongoDB::instance()->select("users", MongoDB::queryFromVariant(query).view()).count() > 0)
        {
            returnValue["errorCode"] = -10;
            returnValue["errorString"] = "Card already exists!";
            return returnValue;
        }
    }

    UserAccess::AddUserResult result = UserAccess::instance()->insertUser(user);
    if(result.errCode == 0)
    {
        nestedArrayPtr permissionContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", result.uuid, "permissions");
        if(permissionContainer.isNull())
        {
            returnValue["errorCode"] = -20;
            returnValue["errorString"] = "Backend error!";
            return returnValue;
        }

        QListIterator<QVariant> permissionIt(permissions);
        while(permissionIt.hasNext())
        {
            permissionContainer->insert(permissionIt.next());
        }

        if(!cardID.isEmpty())
        {
            nestedArrayPtr cardContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", result.uuid, "cards");
            if(cardContainer.isNull())
            {
                returnValue["errorCode"] = -30;
                returnValue["errorString"] = "Backend error!";
                return returnValue;
            }
            cardContainer->insert(card);
        }

        nestedArrayPtr groupContainer =  NestedDataContainerManager::instance()->getNestedArrayData("users", result.uuid, "groups");
        if(groupContainer.isNull())
        {
            returnValue["errorCode"] = -40;
            returnValue["errorString"] = "Backend error!";
            return returnValue;
        }

        QListIterator<QVariant> groupIt(groups);
        while(groupIt.hasNext())
        {
            groupContainer->insert(groupIt.next());
        }
    }

    return result.toMap();
}

QVariant AccessLogic::deleteUser(QString userID)
{
    qInfo()<<"Delete User with "+userID;
    if(UserAccess::instance()->deleteUser(userID))
    {
        return true;
    }
    return false;
}

bool AccessLogic::resetPassword(QString userID)
{
    iUserPtr user = AuthenticationService::instance()->getUserForUserID(userID);

    if(user.isNull())
    {
        iUserPtr user = AuthenticationService::instance()->getUserForUserID(userID);
        qDebug()<<"Invalid UserID";
        return false;
    }

    QString newPass = getRandomString(8);
    user->setPassword(newPass);
    QVariantMap data;
    data["userName"] = user->userData().name;
    data["login"] = user->userLogin();
    data["password"] = newPass;
    MailManager().sendMailFromTemplate(user->getEMail(),"reset-password", data);
    return true;
}


QString AccessLogic::getRandomString(int count)
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


