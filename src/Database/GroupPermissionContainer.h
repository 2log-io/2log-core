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

#ifndef GROUPCONTAINER_H
#define GROUPCONTAINER_H

#include <QObject>
#include "NestedArrayContainer.h"
#include <QSharedPointer>
#include <QDateTime>

class GroupPermissionContainer : public NestedArrayContainer
{
    Q_OBJECT

public:
    struct Group
    {
        QString     _groupID;
        int         _permissionType = 0;
        bool        _expires = true;
        bool        _enabled = true;
        QDateTime   _expirationTime;
        QDateTime   _creationTimeStamp;

        Group(QVariant data)
        {
            QVariantMap map = data.toMap();
            _expirationTime = map["expirationDate"].toDateTime();
            _creationTimeStamp = map["creationDate"].toDateTime();
            _expires = map["expires"].toBool();
            _enabled  = map["active"].toBool();
            _groupID = map["groupID"].toString();
            _permissionType = map["type"].toInt();
        }

        QVariantMap toVariant()
        {
            QVariantMap data;
            data["expirationDate"] = _expirationTime;
            data["creationDate"] = _creationTimeStamp;
            data["expires"] = _expires;
            data["active"] =  _enabled;
            data["groupID"] =  _groupID;
            data["type"] =  _permissionType;
            return data;
        }
    };

    explicit GroupPermissionContainer(QString parentID, QObject *parent = nullptr);
    ~GroupPermissionContainer() override;
    QVariantMap setProperty(int index, QString property, QVariant value) override;
    QVariantMap insert(QVariant data) override;
    bool canRead(iIdentityPtr user) const override;
    bool canWrite(iIdentityPtr user) const override;

private:
    void _insert(QVariantList data) override;
};


class GroupContainerFactory : public IContainerFactory
{
    Q_OBJECT

public:
    explicit GroupContainerFactory(QObject* parent = nullptr) : IContainerFactory("users","groups", parent){}
    virtual QSharedPointer<NestedArrayContainer> createInsance(QString parentID){return QSharedPointer<NestedArrayContainer>(new GroupPermissionContainer(parentID));}

};

#endif // GROUPCONTAINER_H
