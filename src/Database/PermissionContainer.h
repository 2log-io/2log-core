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


#ifndef PERMISSIONCONTAINER_H
#define PERMISSIONCONTAINER_H

#include <QObject>
#include "NestedArrayContainer.h"
#include <QSharedPointer>
#include <QDateTime>

class PermissionContainer : public NestedArrayContainer
{
    Q_OBJECT

 public:
    struct Permission
    {
        QString     _resourceID;
        int         _permissionType = 0;
        bool        _expires = true;
        bool        _enabled = true;
        QDateTime   _expirationTime;
        QDateTime   _creationTimeStamp;

        Permission(QVariant data)
        {
            QVariantMap map = data.toMap();
            _expirationTime = map["expirationDate"].toDateTime();
            _creationTimeStamp = map["creationDate"].toDateTime();
            _expires = map["expires"].toBool();
            _enabled  = map["active"].toBool();
            _resourceID = map["resourceID"].toString();
            _permissionType = map["type"].toInt();
        }

        QVariantMap toVariant()
        {
            QVariantMap data;
            data["expirationDate"] = _expirationTime;
            data["creationDate"] = _creationTimeStamp;
            data["expires"] = _expires;
            data["active"] =  _enabled;
            data["resourceID"] =  _resourceID;
            data["type"] =  _permissionType;
            return data;
        }
    };

    QVariantMap setProperty(int index, QString property, QVariant value) override;
    explicit PermissionContainer(QString parentID, QObject *parent = nullptr);
    ~PermissionContainer() override;
    QVariantMap insert(QVariant data) override;
    bool canRead(iIdentityPtr user) const override;
    bool canWrite(iIdentityPtr user) const override;


private:
    void _insert(QVariantList data) override;

signals:

public slots:
};



class PermissionContainerFactory : public IContainerFactory
{
    Q_OBJECT

public:
    explicit PermissionContainerFactory(QObject* parent = nullptr) : IContainerFactory("users", "permissions", parent){}
    virtual QSharedPointer<NestedArrayContainer> createInsance(QString parentID){return QSharedPointer<NestedArrayContainer>(new PermissionContainer(parentID));}


};

#endif // PERMISSIONCONTAINER_H
