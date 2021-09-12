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


#ifndef RESOURCEPERMISSIONREPLY_H
#define RESOURCEPERMISSIONREPLY_H

#include <QObject>
#include <QSet>
#include <QMutex>
#include <QTimer>
#include "IResourcePermissionReply.h"

class ResourcePermissionReply : public IResourcePermissionReply
{
	Q_OBJECT
	public:

        explicit ResourcePermissionReply(QString cardID, QString userID, QString userName, QString userRole, int balance, QString resourceID, QString callbackID, QObject *parent = nullptr);

        virtual QString	getCardID(void) override;
        virtual QString	getUserID(void)  override;
        virtual QString	getResourceID(void) override;
        virtual QString	getUserRole(void) override;
        virtual int		getTicket(void) override;
        virtual void	permissionChecked(ResourcePermissionErrorCode errorCode, QString errorString, int ticket) override;
        virtual void	commit(void) override;

	private slots:

		void			emitPermissionResult(void);
		void			replyTimeout(void);

	private:

		void			finishReply(void);

		enum ReplyState
		{
			INITIALIZED,
			COMMITTED,
			FINISHED
		};

		QString						cardID;
		QString						userID;
		QString						userName;
        QString                     userRole;
		int							balance;
		QString						resourceID;
        QString						callbackID;
		int							currentTicket;
		QSet<int>					issuedTickets;
		QMutex						replyMutex;

		ResourcePermissionErrorCode	resultCode;
		QString						resultString;
		ReplyState					state;
		QTimer						signalDelayTimer;
		QTimer						timeoutTimer;
};

#endif // RESOURCEPERMISSIONREPLY_H
