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


#ifndef RESOURCEPERMISSIONCHECKSERVICE_H
#define RESOURCEPERMISSIONCHECKSERVICE_H

#include <QObject>
#include "ResourcePermissionErrorCodes.h"

class IResourcePermissionReply;

class ResourcePermissionCheckService : public QObject
{
	Q_OBJECT
	public:

		explicit ResourcePermissionCheckService(QObject *parent = nullptr);

		static ResourcePermissionCheckService*	instance();
		IResourcePermissionReply*				checkPermission(QString cardID, QString resourceID, QString callbackID);

	private:

		ResourcePermissionErrorCode				getErrorCodeFromAccessLogicError(int errorCode);
};

#endif // RESOURCEPERMISSIONCHECKSERVICE_H
