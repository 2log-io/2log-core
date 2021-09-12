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

#ifndef GROUPACCESS_H
#define GROUPACCESS_H

#include <QObject>
#include <QVariant>
#include "Group.h"

typedef  QSharedPointer<Group> groupPtr;
class GroupAccess : public QObject
{
    Q_OBJECT

public:
    explicit GroupAccess(QObject *parent = nullptr);

    QList<groupPtr>         getGroups();
    groupPtr                getGroup(QString uuid);
    groupPtr                insertGroup(QVariantMap group);
    bool                    deleteGroup(QString id);
    static GroupAccess*     instance();
    int                     indexOf(groupPtr group) const;
    int                     indexOf(QString uuid) const;
    QString getUuidByName(QString name) const;

private:
    QMap<QString, groupPtr> _groupIDMap;
    QList<groupPtr> _groups;
    void insertGroup(groupPtr group);

signals:
    void groupInserted(groupPtr group);
    void groupDeleted(groupPtr group);


public slots:
};

#endif // GROUPACCESS_H
