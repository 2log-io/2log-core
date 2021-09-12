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


#include <QDebug>
#include "2logCorePlugin.h"
#include "Statistics/Metrics/StatisticsObjectResourceFactory.h"
#include "Server/Resources/ResourceManager/ResourceManager.h"
#include "Services/LabService.h"
#include "Server/Services/ServiceManager.h"
#include "Services/LogResourceFactory.h"
#include "Services/UserResourceFactory.h"
#include "Services/ContainerResourceFactory.h"
#include "Services/UserObjectResourceFactory.h"
#include "Services/GroupResourceFactory.h"
#include "Database/GroupPermissionContainer.h"
#include "Database/UserAccess.h"
#include "Database/PermissionContainer.h"
#include "Database/GroupEntityContainer.h"
#include "Database/CardContainer.h"
#include "Database/NestedDataContainerManager.h"
#include "Services/FablabAuthenticator.h"
#include "Services/ServiceUserAccess.h"
#include "MailClient/smtp.h"
#include "DevicePermissionHandler/DevicePermissionHandler.h"
#include "Server/Devices/DevicePermissionManager.h"

_2logCorePlugin::_2logCorePlugin(QObject* parent) : IPlugin (parent)
{
}

bool _2logCorePlugin::init(QVariantMap parameters)
{
    Q_UNUSED(parameters)
    LabService* service = new LabService(this);
    NestedDataContainerManager::instance()->registerArrayFactory(new PermissionContainerFactory());
    NestedDataContainerManager::instance()->registerArrayFactory(new GroupContainerFactory());
    NestedDataContainerManager::instance()->registerArrayFactory(new CardContainerFactory());
    NestedDataContainerManager::instance()->registerArrayFactory(new GroupEntityContainerFactory());
    ServiceManager::instance()->registerService(service);
    ResourceManager::instance()->addResourceFactory(new LogResourceFactory(this));
    ResourceManager::instance()->addResourceFactory(new UserResourceFactory(this));
    ResourceManager::instance()->addResourceFactory(new GroupResourceFactory(this));
    ResourceManager::instance()->addResourceFactory(new UserObjectResourceFactory(this));
    ResourceManager::instance()->addResourceFactory(new ContainerResourceFactory("users","permissions", this));
    ResourceManager::instance()->addResourceFactory(new ContainerResourceFactory("users", "cards", this));
    ResourceManager::instance()->addResourceFactory(new ContainerResourceFactory("users", "groups", this));
    ResourceManager::instance()->addResourceFactory(new ContainerResourceFactory("groups", "entities", this));
    ResourceManager::instance()->addResourceFactory(new StatisticsObjectResourceFactory(this));
    AuthenticationService::instance()->registerAuthenticator(new FablabAuthenticator(this));
    DevicePermissionManager::instance()->registerPermissionChecker(devicePermissionCheckerPtr(new DevicePermissionHandler()));
    ServiceUserAccess::setRightLevels();
    return true;
}

bool _2logCorePlugin::shutdown()
{
    return true;
}

QSet<QString> _2logCorePlugin::requires()
{
    QSet<QString> set;
    set << "QHCore";
    return set;
}

QString _2logCorePlugin::getPluginName()
{
    return "2log-core";
}
