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


#ifndef GROUPRESOURCE_H
#define GROUPRESOURCE_H

#include <QObject>
#include <QVariant>
#include "Server/Resources/ListResource/ListResource.h"
#include "../Database/GroupAccess.h"

class GroupResource : public ListResource
{
    Q_OBJECT

public:
    explicit GroupResource(QVariantMap parameters, QObject *parent = nullptr);
    virtual QVariantList       getListData() const override;
    virtual bool               isPermittedToRead(QString token) const override;
    virtual bool               isPermittedToWrite(QString token) const override;
    virtual int                getCount() const override;
    QVariant                   getItem(int idx, QString uuid) const override;
    virtual ModificationResult setProperty(QString property, QVariant data, int index, QString uuid, QString token) override;
    virtual ModificationResult appendItem(QVariant data, QString token) override;
    virtual ModificationResult removeItem(QString uuid, QString token, int index = -1) override;

private:
    QList<groupPtr>             _groups;
    QVariantMap createTemplate(QVariantMap data) const;
    void registerGroup(groupPtr group);

public slots:
    void groupInserted(groupPtr group);
    void groupDeleted(groupPtr group);

    void descriptionChanged(QString name, QString decription);
    void typeChanged(QString name, QString type);
    void entitiesChanged(QString name, QVariantList entities);
};

#endif // GROUPRESOURCE_H
