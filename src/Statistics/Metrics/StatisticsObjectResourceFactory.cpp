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

#include "StatisticsObjectResourceFactory.h"
#include "MachineStatisticsObjectResource.h"
#include "MetricsObjectResource.h"


StatisticsObjectResourceFactory::StatisticsObjectResourceFactory(QObject *parent) :
    IResourceFactory(parent)
{
}

StatisticsObjectResourceFactory::~StatisticsObjectResourceFactory()
{
}

QString StatisticsObjectResourceFactory::getResourceID(QString descriptor, QString token) const
{
    Q_UNUSED(token)
    return descriptor;
}

QString StatisticsObjectResourceFactory::getResourceType() const
{
    return "object";
}

QString StatisticsObjectResourceFactory::getDescriptorPrefix() const
{
    return "statistics";
}

resourcePtr StatisticsObjectResourceFactory::createResource(QString token, QString descriptor, QObject *parent)
{
    Q_UNUSED(parent);
    iIdentityPtr user = AuthenticationService::instance()->validateToken(token);
    if(user.isNull() || !user->isAuthorizedTo(IS_ADMIN))
        return resourcePtr();

    if(descriptor.startsWith("statistics/metrics"))
        return resourcePtr(new MetricsObjectResource(parseParameters(descriptor)));

    if(descriptor.startsWith("statistics/resource"))
    {
        QString path;

        if(descriptor.contains("?"))
            path = descriptor.split("?").first();
        else if(descriptor.contains(":"))
            path = descriptor.split(":").first();
        else
            path = descriptor;

        QString resourceID = path.split("/").last();
        return resourcePtr(new MachineStatisticsObjectResource(resourceID, parseParameters(descriptor)));
    }

    return resourcePtr();
}
