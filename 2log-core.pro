#   2log.io
#   Copyright (C) 2021 - 2log.io | mail@2log.io,  mail@friedemann-metzger.de
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.


TARGET = 2log-core

#QT       -= gui
QT      += network concurrent
TEMPLATE = lib
DEFINES += QHFABLABCONTROL_LIBRARY

INCLUDEPATH += ../quickhub-core/src
INCLUDEPATH += src
LIBS += -lQHCore -L"../bin/plugins"
DESTDIR = ../bin/plugins
CONFIG      += plugin
INCLUDEPATH += ../quickhub-pluginsystem/src
LIBS += -lQHPluginSystem -L../bin/lib

include(src/MailClient/SmtpClient-for-Qt/SMTPEmail.pri)
INCLUDEPATH += /usr/local/include/bsoncxx/v_noabi
DEPENDPATH += /usr/local/include/bsoncxx/v_noabi

INCLUDEPATH += /usr/local/include/mongocxx/v_noabi
DEPENDPATH += /usr/local/include/mongocxx/v_noabi

LIBS += -L/usr/local/lib -lmongocxx -lbsoncxx

SOURCES += \
    src/2logCorePlugin.cpp \
    src/Database/Group.cpp \
    src/Database/GroupAccess.cpp \
    src/Database/GroupEntityContainer.cpp \
    src/Database/GroupPermissionContainer.cpp \
    src/Database/NestedDataContainerManager.cpp \
    src/DevicePermissionHandler/DevicePermissionHandler.cpp \
    src/MailClient/MailService.cpp \
    src/MailClient/smtp.cpp \
    src/ResourcePermissions/IResourcePermissionCheck.cpp \
    src/ResourcePermissions/IResourcePermissionCheckService.cpp \
    src/ResourcePermissions/IResourcePermissionReply.cpp \
    src/ResourcePermissions/ResourcePermissionReply.cpp \
    src/Services/ContainerResourceFactory.cpp \
    src/Services/GroupResource.cpp \
    src/Services/GroupResourceFactory.cpp \
    src/Services/LogResource.cpp \
    src/Services/LogResourceFactory.cpp \
    src/Database/MongoDB.cpp \
    src/Database/LogAccess.cpp \
    src/Database/UserAccess.cpp \
    src/Database/FablabUser.cpp \
    src/Services/ServiceUserAccess.cpp \
    src/Services/UserResource.cpp \
    src/Services/UserResourceFactory.cpp \
 #  Database/PermissionAccess.cpp \
 #  Database/UserPermission.cpp \
 #  Database/UserPermissionContainer.cpp \
    src/Database/NestedArrayContainer.cpp \
    src/Database/PermissionContainer.cpp \
    src/Database/CardContainer.cpp \
    src/Services/ContainerResource.cpp \
    src/Services/LabService.cpp \
    src/Services/AccessLogic.cpp \
    src/Services/UserObjectResource.cpp \
    src/Services/UserObjectResourceFactory.cpp \
    src/Services/FablabAuthenticator.cpp \
    src/Statistics/Database/StatisticQueries.cpp \
    src/Statistics/Metrics/MachineStatisticsObjectResource.cpp \
    src/Statistics/Metrics/MetricsObjectResource.cpp \
    src/Statistics/Metrics/StatisticsObjectResourceFactory.cpp

HEADERS += \
    src/2logCorePlugin.h \
    src/2logcoreplugin_global.h \
    src/Database/Group.h \
    src/Database/GroupAccess.h \
    src/Database/GroupEntityContainer.h \
    src/Database/GroupPermissionContainer.h \
    src/Database/NestedDataContainerManager.h \
    src/DevicePermissionHandler/DevicePermissionHandler.h \
    src/MailClient/MailService.h \
    src/MailClient/smtp.h \
    src/ResourcePermissions/IResourcePermissionCheck.h \
    src/ResourcePermissions/IResourcePermissionCheckService.h \
    src/ResourcePermissions/IResourcePermissionReply.h \
    src/ResourcePermissions/ResourcePermissionErrorCodes.h \
    src/ResourcePermissions/ResourcePermissionReply.h \
    src/Services/ContainerResourceFactory.h \
    src/Services/GroupResource.h \
    src/Services/GroupResourceFactory.h \
    src/Services/ServiceUserAccess.h \
    src/Services/LogResource.h \
    src/Services/LogResourceFactory.h \
    src/Database/MongoDB.h \
    src/Database/LogAccess.h \
    src/Database/UserAccess.h \
    src/Database/Log.h \
    src/Database/FablabUser.h \
    src/Services/UserResource.h \
    src/Services/UserResourceFactory.h \
  #  Database/PermissionAccess.h \
  #  Database/UserPermission.h \
  #  Database/UserPermissionContainer.h \
    src/Database/NestedArrayContainer.h \
    src/Database/PermissionContainer.h \
    src/Database/CardContainer.h \
    src/Services/ContainerResource.h \
    src/Services/LabService.h \
    src/Services/AccessLogic.h \
    src/Services/UserObjectResource.h \
    src/Services/UserObjectResourceFactory.h \
    src/Services/FablabAuthenticator.h \
    src/Statistics/Database/StatisticQueries.h \
    src/Statistics/Metrics/MachineStatisticsObjectResource.h \
    src/Statistics/Metrics/MetricsObjectResource.h \
    src/Statistics/Metrics/StatisticsObjectResourceFactory.h


unix {
#    target.path = /usr/lib
#    INSTALLS += target
}
CONFIG += c++14

DISTFILES += \
    src/2logCorePlugin.json
