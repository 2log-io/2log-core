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

#ifndef GROUPENTITYCONTAINER_H
#define GROUPENTITYCONTAINER_H

#include <QObject>
#include "NestedArrayContainer.h"
#include <QSharedPointer>

class GroupEntityContainer : public NestedArrayContainer
{

public:
    struct GroupEntity
    {
        QString entityID;
        QString entityType;

        GroupEntity(QVariant data)
        {
            QVariantMap map = data.toMap();
            entityID = map["entityID"].toString();
            entityType = map["entityType"].toString();

        }

        QVariantMap toVariant()
        {
            QVariantMap map;
            map["entityID"] = entityID;
            map["entityType"] = entityType;
            return map;
        }
    };

    explicit GroupEntityContainer(QString parentID, QObject* parent = nullptr);
    QVariantMap setProperty(int index, QString property, QVariant value) override;
    QVariantMap insert(QVariant data) override;

    bool hasEntity(QString entityID, QString entityType);
    bool canRead(iIdentityPtr user) const override;
    bool canWrite(iIdentityPtr user) const override;

};

class GroupEntityContainerFactory : public IContainerFactory
{
    Q_OBJECT

public:
    explicit GroupEntityContainerFactory(QObject* parent = nullptr) : IContainerFactory("groups","entities", parent){}
    virtual QSharedPointer<NestedArrayContainer> createInsance(QString parentID){return QSharedPointer<NestedArrayContainer>(new GroupEntityContainer(parentID));}

};


#endif // GROUPENTITYCONTAINER_H
