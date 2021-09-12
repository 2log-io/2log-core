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


#include "DevicePermissionHandler.h"
#include "Server/Authentication/AuthentificationService.h"
#include "Server/Authentication/IUser.h"
#include "Database/UserAccess.h"
#include <QDebug>

DevicePermissionHandler::DevicePermissionHandler(QObject *parent) : IDevicePermissionChecker(parent)
{
}

QStringList DevicePermissionHandler::getControlledDeviceTypes()
{
    QStringList controlledDevices;
    controlledDevices << "2log Switch" << "2log Dot" << "Controller/Monitoring" << "Controller/AccessControl";
    return controlledDevices;
}

IDevicePermissionChecker::PropertyPermission DevicePermissionHandler::checkPropertyPermission(QString token, DeviceHandle* handle, QString property)
{
    Q_UNUSED(handle);
    Q_UNUSED(property);
    PropertyPermission permission;
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull())
    {
        permission.canRead = false;
        permission.canWrite = false;
        return permission;
    }

    permission.canRead = user->isAuthorizedTo(LAB_SEE_DEVICES) || user->isAuthorizedTo(SERVICE);
    permission.canWrite = user->isAuthorizedTo(LAB_MODIFY_DEVICES) || user->isAuthorizedTo(SERVICE);
    return permission;
}

bool DevicePermissionHandler::checkRPCPermission(QString token, DeviceHandle* handle, QString rpc)
{
    Q_UNUSED(handle);
    Q_UNUSED(rpc);

    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_MODIFY_DEVICES);
}

bool DevicePermissionHandler::checkSetupPermission(QString token, DeviceHandle *handle)
{
    Q_UNUSED(handle);
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull())
        return false;
    return user->isAuthorizedTo(LAB_MODIFY_DEVICES);
}
