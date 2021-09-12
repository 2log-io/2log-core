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


#ifndef SERVICEUSERACCESS_H
#define SERVICEUSERACCESS_H

#include <QObject>
#include "Server/Authentication/User.h"
#include "Server/Authentication/DefaultAuthenticator.h"

#define USER_ROLE_OFFICE_STRING "mngmt"
#define USER_ROLE_ADMIN_STRING "admin"
#define USER_ROLE_EMPLOYEE_STRING "empl"
#define USER_ROLE_CASHIER_STRING "cash"

class ServiceUserAccess : public QObject
{
    Q_OBJECT


public:
    enum UserRole
    {
        USER_ROLE_UNDEFINED,
        USER_ROLE_OFFICE,
        USER_ROLE_ADMIN,
        USER_ROLE_EMPLOYEE,
        USER_ROLE_CASHIER
    };

    explicit ServiceUserAccess(QObject *parent = nullptr);
    static QVariantMap addServiceUser(QString userID,  QString userName, QString eMail, UserRole role, QString token);
    static QString getRandomString(int count);
    static bool setUserRole(UserRole role, userPtr user);
    static bool changeUserLevel(UserRole role, QString userID);
    static UserRole getRole(QString string);
    static void setRightLevels();
};

#endif // SERVICEUSERACCESS_H
