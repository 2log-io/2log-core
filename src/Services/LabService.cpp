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


#include "LabService.h"
#include <QtDebug>
#include <QJsonDocument>
#include "../Database/LogAccess.h"
#include "../Database/UserAccess.h"
#include "../Database/PermissionContainer.h"
#include "../Database/CardContainer.h"
#include "../Services/ServiceUserAccess.h"
#include "Database/MongoDB.h"
#include "AccessLogic.h"

#include "../Database/GroupAccess.h"
#include "ResourcePermissions/IResourcePermissionCheckService.h"
#include "ResourcePermissions/IResourcePermissionReply.h"
#include <QFuture>
#include <QtConcurrent>
#include "MailClient/MailManager.h"

LabService::LabService(QObject *parent) : IService(parent)
{
}

QString LabService::getServiceName()
{
    return "lab";
}

QStringList LabService::getServiceCalls()
{
    QStringList calls;
    calls << "addLog"
          << "getLogs"
          << "resetPassword"
          << "addUser"
          << "deleteUser"
          << "sendSession"
          << "logout"
          << "login"
          << "getUserForCard"
          << "changeUserLevel"
          << "addSystemUser"
          << "hasPermission"
          << "transferMoney"
          << "addGroup"
          << "addOrUpdateUser";

    return calls;
}

bool LabService::call(QString call, QString token, QString cbID, QVariant argument)
{

    iIdentityPtr executiveUser = AuthenticationService::instance()->validateToken(token);
	QVariantMap argMap = argument.toMap();


	if(call == "hasPermission")
	{

        if (executiveUser.isNull() || !executiveUser->isAuthorizedTo(LAB_SERVICE))
			return false;

		QString cardID = argMap["cardID"].toString();
		QString resourceID = argMap["resourceID"].toString();

		IResourcePermissionReply *reply = ResourcePermissionCheckService::instance()->checkPermission(cardID, resourceID, cbID);
		connect(reply, &IResourcePermissionReply::permissionResult, this, &LabService::permissionResult);

		return true;
	}

    else if(call == "getLogs")
    {
        if(executiveUser.isNull())
            return false;

        QVariantMap filter = argMap["filter"].toMap();
        QFutureWatcher<QVariantList>* watcher = new QFutureWatcher<QVariantList>(this);
        _futures.insert(watcher, cbID);
        connect(watcher, &QFutureWatcher<QVariantList>::finished,this, &LabService::futureResult);
        QFuture<QVariantList> future = QtConcurrent::run(LogAccess::getLogsWithUser, filter);
        watcher->setFuture(future);
        return true;
    }

	else
	{
		QVariant answer = syncCalls(call, token, argument);
		if(answer.isValid())
		{
			Q_EMIT response(cbID, answer);
			return true;
		}
	}

    return false;
}

QVariant LabService::syncCalls(QString call, QString token, QVariant argument)
{


    QVariantMap argMap = argument.toMap();
    if(call == "resetPassword")
    {
        return  AccessLogic::resetPassword(argMap["userID"].toString());
    }

    iIdentityPtr executiveUser = AuthenticationService::instance()->validateToken(token);
    if(executiveUser.isNull())
        return false;


    if(call == "addLog")
    {
        if(!executiveUser->isAuthorizedTo(LAB_ADMIN) ||
           !executiveUser->isAuthorizedTo(LAB_SERVICE))
            return false;

		Log logToAdd = Log(argMap);
		LogAccess::instance()->insertLog(logToAdd);
        return true;
    }

    if(call == "addUser")
    {
        if(!(     executiveUser->isAuthorizedTo(LAB_ADMIN)      ||
                  executiveUser->isAuthorizedTo(LAB_MODIFY_USERS)
            ))
            return false;
        return AccessLogic::addUser( argMap["user"].toMap(),
                                argMap["permissions"].toMap().values(),
                                argMap["card"].toMap(),
                                argMap["groups"].toMap().values());
    }


    if(call == "addOrUpdateUser")
    {
        if(!(     executiveUser->isAuthorizedTo(LAB_ADMIN)      ||
                  executiveUser->isAuthorizedTo(LAB_MODIFY_USERS)
            ))
            return false;
        return AccessLogic::addOrUpdateUser(argMap["user"].toMap(),
                                argMap["permissions"].toMap().values(),
                                argMap["card"].toMap(),
                                argMap["groups"].toMap().values());
    }


    if(call == "deleteUser")
    {
        if(!(     executiveUser->isAuthorizedTo(LAB_ADMIN)      ||
                  executiveUser->isAuthorizedTo(LAB_MODIFY_USERS)
            ))
            return false;

        qDebug()<<"Delete " << argMap["userID"].toString();
        return AccessLogic::deleteUser(argMap["userID"].toString());
    }

    if(call == "sendSession" || call == "sendBill") // todo remove
    {
        if( !executiveUser->isAuthorizedTo(LAB_ADMIN) ||
            !executiveUser->isAuthorizedTo(LAB_SERVICE))
            return false;

        return doLog(argMap, Log::BILL, executiveUser);
    }

    if (call == "sendJob")
    {
        if(!executiveUser->isAuthorizedTo(LAB_ADMIN)
        || !executiveUser->isAuthorizedTo(LAB_SERVICE))
            return false;

        return doLog(argMap, Log::JOB, executiveUser);
    }

    if(call == "chargeUser" || call =="withdrawMoney")
    {
        // only services are allowed to change money without producing a log
        if (!executiveUser->isAuthorizedTo(LAB_SERVICE))
            return false;

        if(!argMap.contains("userID") || !argMap.contains("value"))
            return false;

       fablabUserPtr ptr = UserAccess::instance()->getUser(argMap["userID"].toString());
       if(!ptr.isNull())
       {
           int balance = ptr->getBalance();
           int val = argMap["value"].toInt();
           ptr->setBalance(balance + val);
           return true;
       }
       return false;
    }

    if(call == "addGroup")
    {

        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN) ||
             executiveUser->isAuthorizedTo(LAB_SERVICE) ||
             executiveUser->isAuthorizedTo(LAB_MODIFY_GROUPS)))
            return false;

        return !GroupAccess::instance()->insertGroup(argMap).isNull();
    }


    if(call == "transferMoney")
    {
        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN)
          || executiveUser->isAuthorizedTo(LAB_SERVICE)))
            return false;

       if(!argMap.contains("userID") || !argMap.contains("value"))
           return false;

       QString userID       = argMap["userID"].toString();
       int val              = argMap["value"].toInt();

       fablabUserPtr ptr = UserAccess::instance()->getUser(userID);
       if(ptr.isNull())
           return false;

       // TODO: refactor this to a mutexed, atomar operation
       int balance = ptr->getBalance();
       ptr->setBalance(balance + val);

        QVariantMap summary;
        summary["total"] = val;
        summary["username"] = ptr->getName();
        summary["balance"] = ptr->getBalance();
        MailManager().sendMailFromTemplate(ptr->getEMail(),"member-transaction", summary);

       Log log;
       log.userID = userID;
       log.executive = executiveUser->identityID();
       log.price = val;
       log.event = Log::TRANSFER;
       log.description = argMap["description"].toString();
       LogAccess::instance()->insertLog(log);
       return true;
    }

    if(call == "getLogs")
    {
        QVariantMap filter = argMap["filter"].toMap();
        QElapsedTimer timer;
        timer.start();
        auto result = LogAccess::instance()->getLogsWithUser(filter);
        qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
        return result;
    }

    if(call == "getUserForCard")
    {
        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN)
          || executiveUser->isAuthorizedTo(LAB_SERVICE)
          || executiveUser->isAuthorizedTo(LAB_SEE_USERS)))
            return false;
        QString cardID = argMap["cardID"].toString();
        return AccessLogic::getUserForCardID(cardID);
    }


    if(call == "addSystemUser")
    {
        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN) ||
             executiveUser->isAuthorizedTo(IS_ADMIN)
          || executiveUser->isAuthorizedTo(LAB_SERVICE)))
            return false;

        QString level       = argMap["level"].toString();
        QString userId      = argMap["userID"].toString();
        QString eMail       = argMap["eMail"].toString();
        QString name        = argMap["name"].toString();

        ServiceUserAccess::UserRole  role = ServiceUserAccess::getRole(level);
        return ServiceUserAccess::addServiceUser(userId, name, eMail, role, token);
    }

    if(call == "changeUserLevel")
    {
        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN) ||
             executiveUser->isAuthorizedTo("isAdmin")
          || executiveUser->isAuthorizedTo(LAB_SERVICE)))
            return false;

        QString level       = argMap["level"].toString();
        QString userId      = argMap["userID"].toString();

        ServiceUserAccess::UserRole  role = ServiceUserAccess::getRole(level);
        return ServiceUserAccess::changeUserLevel( role, userId);
    }


    if(call == "getUserForCard")
    {
        if(!(executiveUser->isAuthorizedTo(LAB_ADMIN)
          || executiveUser->isAuthorizedTo(LAB_SERVICE)
          || executiveUser->isAuthorizedTo(LAB_SEE_USERS)))
            return false;

        QString cardID = argMap["cardID"].toString();
        return AccessLogic::getUserForCardID(cardID);
    }


    if(call == "login")
    {
        return QVariant();
    }


    if(call == "logout")
    {
        return QVariant();
    }


    return QVariant();
}


bool LabService::doLog(QVariantMap data, Log::EventType type, iIdentityPtr executive)
{
    QString userID      = data["userID"].toString();
    QString resourceID  = data["resourceID"].toString();
    QString cardID      = data["cardID"].toString();
    int units           = data["units"].toInt();
    int price           = data["price"].toInt();
    QDateTime start     = data["start"].toDateTime();
    QDateTime end       = data["end"].toDateTime();
    QString description = data["description"].toString();

    Log log;
    fablabUserPtr user;

    if(!userID.isEmpty())
    {
        user = UserAccess::instance()->getUser(userID);
        if(user.isNull())
            return false;

        log.userName   = user->getName() +" "+user->getSurname();
    }

    log.userID     = userID;
    log.resourceID = resourceID;
    log.cardID     = cardID;
    log.event      = type;
    log.units      = units;
    log.price      = price;
    log.start      = start;
    log.end        = end;
    log.description= description;
    log.executive = executive->identityID();
    LogAccess::instance()->insertLog(log);
	return true;
}

void LabService::permissionResult(QVariantMap result, QString cbID)
{
	QObject *reply = sender();
	reply->deleteLater();

    Q_EMIT response(cbID, result);
}

void LabService::futureResult()
{
    QFutureWatcher<QVariantList>* watcher = static_cast<QFutureWatcher<QVariantList>*>(sender());
    if(watcher)
    {
        QString cbID = _futures.value(watcher);
        Q_EMIT response(cbID, watcher->result());
        _futures.remove(watcher);
        delete watcher;
        watcher = 0;
    }
}
