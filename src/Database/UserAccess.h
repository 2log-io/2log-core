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


#ifndef USERACCESS_H
#define USERACCESS_H

#include <QObject>
#include "FablabUser.h"
#include "Database/MongoDB.h"
#include <QSharedPointer>
#include "NestedArrayContainer.h"


#define LAB_ADMIN               "lab.admin"
#define LAB_SERVICE             "lab.service"

#define SEE_OWN_USER_DATA       "lab.rOwn"

#define LAB_SEE_USERS           "lab.canSeeUsers"
#define LAB_MODIFY_USERS        "lab.canEditUsers"

#define LAB_SEE_DEVICES         "lab.canSeeDevices"
#define LAB_MODIFY_DEVICES      "lab.canEditDevices"

#define LAB_SEE_GROUPS          "lab.canSeeGroups"
#define LAB_MODIFY_GROUPS       "lab.canModifyGroups"

#define LAB_SEE_LOGS            "lab.canSeeLogs"

#define LAB_SEND_BILLS          "lab.canSendBills"


class IContainerFactory;

typedef  QSharedPointer<FablabUser> fablabUserPtr;
//typedef  QSharedPointer<NestedArrayContainer> nestedArrayPtr;

class UserAccess : public QObject
{
    friend class FablabUser;
    friend class NestedArrayContainer;

    Q_OBJECT

public:
    struct AddUserResult
    {
        enum ERRCODE
        {
           OK = 0,
           ALREADY_EXISTS = -1,
           UNKNOWN_ERROR = -2,
           INVALID_DATA = -3,
        };

        QString uuid;
        ERRCODE errCode = UNKNOWN_ERROR;

        QVariantMap toMap()
        {
            QVariantMap result;
            result["uuid"] = uuid;
            result["errorCode"] = errCode;
            switch(errCode)
            {
                case ALREADY_EXISTS: result["errString"] = "User with given eMail address already exists"; break;
                case UNKNOWN_ERROR: result["errString"] = "Unknown error."; break;
                case INVALID_DATA: result["errString"] = "Inavild or incomplete data."; break;
                default: break;
            }
            return result;
        }
    };

    explicit UserAccess(QObject *parent = nullptr);
    static UserAccess* instance();

    QList<fablabUserPtr>    getUsers(Query filter =  document{}<<finalize);
    fablabUserPtr           getUser(QString userID);
    fablabUserPtr           getUserWithCard(QString cardID);
    fablabUserPtr           getUserWithEMail(QString eMail);
    bool                    deleteUser(QString userID);
    AddUserResult           insertUser(QVariantMap user);
    void                    setLastLogin(QString userID);


signals:
    void userInserted(fablabUserPtr user);
    void userDeleted(fablabUserPtr user);
    void propertyChanged(QString uuid, QString property, QVariant value);

private:
    fablabUserPtr getOrAddUser(QVariantMap userData);
    QMap<QString, QWeakPointer<FablabUser> > _users;

private slots:
    void userObjectDestroyed(QString uuid);

public slots:
};

#endif // USERACCESS_H
