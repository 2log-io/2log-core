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


#ifndef IRESOURCEPERMISSIONREPLY_H
#define IRESOURCEPERMISSIONREPLY_H

#include <QObject>
#include <QVariantMap>

#include "ResourcePermissionErrorCodes.h"

class IResourcePermissionReply : public QObject
{
	Q_OBJECT
	public:

		explicit		IResourcePermissionReply(QObject *parent = nullptr);

		virtual QString	getCardID(void) = 0;
		virtual QString	getUserID(void) = 0;
		virtual QString	getResourceID(void) = 0;
        virtual QString	getUserRole(void) = 0;
		virtual int		getTicket(void) = 0;

		/**
		 * @brief permissionChecked is called from every IResourcePermissionCheck to deliver the permission result
		 * @param errorCode
		 * @param errorString
		 * @param ticket
		 */
		virtual void	permissionChecked(ResourcePermissionErrorCode errorCode, QString errorString, int ticket) = 0;

		/**
		 * @brief commit has to be called after all IResourcePermissionChecks has been attached to the reply. If all
		 * permissions are already checked commit will immedeatly trigger the signal.
		 */
		virtual void	commit(void) = 0;

	signals:

		void			permissionResult(QVariantMap result, QString cbID);
};

#endif // IRESOURCEPERMISSIONREPLY_H
