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


#include "LogResourceFactory.h"
#include "LogResource.h"
#include "Database/UserAccess.h"

LogResourceFactory::LogResourceFactory(QObject* parent) : IResourceFactory(parent)
{
}

LogResourceFactory::~LogResourceFactory()
{
}

QString LogResourceFactory::getResourceID(QString descriptor, QString token) const
{
    Q_UNUSED(token)
    return descriptor;
}

QString LogResourceFactory::getResourceType() const
{
    return "synclist";
}

QString LogResourceFactory::getDescriptorPrefix() const
{
    return "labcontrol/logs";
}

resourcePtr LogResourceFactory::createResource(QString token, QString descriptor, QObject *parent)
{
    Q_UNUSED(parent)
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull() ||  !(user->isAuthorizedTo(LAB_SERVICE) || user->isAuthorizedTo(LAB_ADMIN) || user->isAuthorizedTo(LAB_SEE_LOGS) ))
        return nullptr;

    return resourcePtr(new LogResource(parseParameters(descriptor)));
}
