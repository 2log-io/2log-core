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


#include "IResourcePermissionCheckService.h"
#include "ResourcePermissionReply.h"
#include "IResourcePermissionCheck.h"
#include "PluginManager.h"
#include "Services/AccessLogic.h"


Q_GLOBAL_STATIC(ResourcePermissionCheckService, resourcePermissionCheckService);

ResourcePermissionCheckService::ResourcePermissionCheckService(QObject *parent) : QObject(parent)
{
}

ResourcePermissionCheckService *ResourcePermissionCheckService::instance()
{
	return resourcePermissionCheckService;
}

IResourcePermissionReply *ResourcePermissionCheckService::checkPermission(QString cardID, QString resourceID, QString callbackID)
{
	QVariantMap accessResult = AccessLogic::checkPermission(cardID, resourceID).toMap();

	QString userID		= accessResult["userID"].toString();
	QString userName	= accessResult["userName"].toString();
    QString userRole	= accessResult["userRole"].toString();
	QString errorString	= accessResult["errorString"].toString();
	int errorCode		= accessResult["errorCode"].toInt();
	int balance			= accessResult["balance"].toInt();

    ResourcePermissionReply* reply = new ResourcePermissionReply(cardID, userID, userName, userRole, balance, resourceID, callbackID, this);


	ResourcePermissionErrorCode permissionErrorCode = getErrorCodeFromAccessLogicError(errorCode);

	reply->permissionChecked(permissionErrorCode, errorString, reply->getTicket() );

	if ( permissionErrorCode == ResourcePermissionErrorCode::PERMISSION_GRANTED )
	{
		QList<IResourcePermissionCheck*> permissionCheckers = PluginManager::getInstance()->getObjects<IResourcePermissionCheck>();

		QListIterator<IResourcePermissionCheck*> checkerIterator(permissionCheckers);
		while ( checkerIterator.hasNext() )
		{
			IResourcePermissionCheck* permissionChecker = checkerIterator.next();

			QPointer<IResourcePermissionReply> pointer = reply;

			permissionChecker->checkPermission(pointer);
		}
	}

	reply->commit();

	return reply;
}

ResourcePermissionErrorCode ResourcePermissionCheckService::getErrorCodeFromAccessLogicError(int errorCode)
{
	switch (errorCode)
	{
		case -1:
			return ResourcePermissionErrorCode::NO_DATA;
		case -2:
			return ResourcePermissionErrorCode::USER_DISABLED;
		case -3:
			return ResourcePermissionErrorCode::CARD_DISABLED;
		case -10:
			return ResourcePermissionErrorCode::PERMISSION_DISABLED;
		case -11:
			return ResourcePermissionErrorCode::PERMISSION_EXPIRED;
		case 0:
			return ResourcePermissionErrorCode::PERMISSION_GRANTED;
		default:
			return ResourcePermissionErrorCode::UNKNOWN_ERROR;
	}
}
