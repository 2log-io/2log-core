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


#ifndef LOGRESOURCEFACTORY_H
#define LOGRESOURCEFACTORY_H

#include <QObject>
#include "Server/Resources/ResourceManager/IResourceFactory.h"


class LogResourceFactory : public IResourceFactory
{
public:
    LogResourceFactory(QObject* parent = nullptr);

    virtual ~LogResourceFactory() override;

    virtual QString getResourceID(QString descriptor, QString token = "") const override;
    virtual QString getResourceType() const override;
    virtual QString getDescriptorPrefix() const override;

private:
    virtual resourcePtr createResource( QString token, QString descriptor, QObject* parent = nullptr) override;
};

#endif // LOGRESOURCEFACTORY_H
