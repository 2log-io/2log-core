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


#ifndef QHFABLABCONTROLPLUGIN_H
#define QHFABLABCONTROLPLUGIN_H

#include "IPlugin.h"
#include <QObject>
#include "2logcoreplugin_global.h"

class QHFABLABCONTROLSHARED_EXPORT _2logCorePlugin : public IPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID IPlugin_iid FILE "src/2logCorePlugin.json")
    Q_INTERFACES(IPlugin)

public:
    _2logCorePlugin(QObject* parent = nullptr);
    virtual bool init(QVariantMap parameters) override;
    virtual bool shutdown() override;
    virtual QSet<QString> requires() override;
    virtual QString getPluginName() override;
};


#endif // QHFABLABCONTROLPLUGIN_H
