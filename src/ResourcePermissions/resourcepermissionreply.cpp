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


#include "ResourcePermissionReply.h"

#include <QMutexLocker>
#include <QDebug>

ResourcePermissionReply::ResourcePermissionReply(QString cardID, QString userID, QString userName, QString userRole, int balance, QString resourceID, QString callbackID, QObject *parent)
    : IResourcePermissionReply(parent),
      cardID(cardID),
      userID(userID),
      userName(userName),
      userRole(userRole),
      balance(balance),
      resourceID(resourceID),
      callbackID(callbackID),
      currentTicket(0),
      replyMutex(QMutex::Recursive),
      resultCode(ResourcePermissionErrorCode::PERMISSION_GRANTED),
      state(INITIALIZED)
{
	signalDelayTimer.setSingleShot(true);
	timeoutTimer.setSingleShot(true);
}

QString ResourcePermissionReply::getCardID()
{
	return cardID;
}

QString ResourcePermissionReply::getUserID()
{
	return userID;
}

QString ResourcePermissionReply::getResourceID()
{
    return resourceID;
}

QString ResourcePermissionReply::getUserRole()
{
    return userRole;
}

int ResourcePermissionReply::getTicket()
{
	QMutexLocker locker(&replyMutex);

	if (state != INITIALIZED)
	{
		return -1;
	}

	currentTicket++;

	issuedTickets.insert(currentTicket);

	return currentTicket;
}

void ResourcePermissionReply::permissionChecked(ResourcePermissionErrorCode errorCode, QString errorString, int ticket)
{
	QMutexLocker locker(&replyMutex);

	if ( state != FINISHED )
	{
		resultCode = resultCode | errorCode;

		if ( resultString.isEmpty() )
		{
			resultString = errorString;
		}
		else
		{
			resultString = resultString + "\n" + errorString;
		}

		issuedTickets.remove(ticket);

		if ( state == COMMITTED && issuedTickets.size() == 0 )
		{
			finishReply();
		}
	}
}

void ResourcePermissionReply::commit()
{
	QMutexLocker locker(&replyMutex);

	state = COMMITTED;

	if ( issuedTickets.size() == 0 )
	{
		finishReply();
	}
	else
	{
		connect(&timeoutTimer, &QTimer::timeout, this, &ResourcePermissionReply::replyTimeout);
		timeoutTimer.start(5 * 1000);
	}
}

void ResourcePermissionReply::emitPermissionResult()
{
	QVariantMap result;

	// Compatibility code:
	// convert the internal bitfield to integer values
	// according to the current interface of the hasPermission function
	// in the future this interface should be refactored to also use the bitmask

	int errorCode = -50;

	if ( resultCode == ResourcePermissionErrorCode::PERMISSION_GRANTED)
	{
		errorCode = 0;
	}

	if ( resultCode & ResourcePermissionErrorCode::NO_DATA )
	{
		errorCode = -1;
	}

	if ( resultCode & ResourcePermissionErrorCode::USER_DISABLED )
	{
		errorCode = -2;
	}

	if ( resultCode & ResourcePermissionErrorCode::CARD_DISABLED )
	{
		errorCode = -3;
	}

	if ( resultCode & ResourcePermissionErrorCode::PERMISSION_DISABLED )
	{
		errorCode = -10;
	}

	if ( resultCode & ResourcePermissionErrorCode::PERMISSION_EXPIRED )
	{
		errorCode = -11;
	}

	result["userID"]		= userID;
    result["userRole"]		= userRole;
	result["userName"]		= userName;
	result["balance"]		= balance;
	result["errorString"]	= resultString;
	result["errorCode"]		= errorCode;

	Q_EMIT permissionResult(result, callbackID);
}

void ResourcePermissionReply::replyTimeout()
{
	qDebug() << "replyTimeout";
	resultString = "Permission check timed out";
	resultCode = ResourcePermissionErrorCode::TIMEOUT;
	finishReply();
}

void ResourcePermissionReply::finishReply()
{
	replyMutex.lock();
		state = FINISHED;
	replyMutex.unlock();

	connect(&signalDelayTimer, &QTimer::timeout, this, &ResourcePermissionReply::emitPermissionResult);
	signalDelayTimer.start();
}
