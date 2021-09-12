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


#include "UserResourceFactory.h"
#include "UserResource.h"

UserResourceFactory::UserResourceFactory(QObject* parent) : IResourceFactory(parent)
{
}

UserResourceFactory::~UserResourceFactory()
{
}

QString UserResourceFactory::getResourceID(QString descriptor, QString token) const
{
    Q_UNUSED(token)
    return descriptor;
}

QString UserResourceFactory::getResourceType() const
{
    return "synclist";
}

QString UserResourceFactory::getDescriptorPrefix() const
{
    return "labcontrol/users";
}

resourcePtr UserResourceFactory::createResource(QString token, QString descriptor, QObject *parent)
{
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
      if(user.isNull() || !( user->isAuthorizedTo(LAB_ADMIN) || user->isAuthorizedTo(IS_ADMIN) || user->isAuthorizedTo(LAB_SEE_USERS)))
        return nullptr;

    Q_UNUSED(parent)
    return resourcePtr(new UserResource(parseParameters(descriptor)));
}
