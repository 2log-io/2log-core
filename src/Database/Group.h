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

#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include "GroupEntityContainer.h"

class Group : public QObject
{
    Q_OBJECT

public:
    explicit Group(QVariantMap groupData, QObject *parent = nullptr);

    QVariantMap toVariant() const;
    QString description() const;
    bool setDescription(const QString &description);

    QString name() const;
    bool setName(const QString &name);

    void setUUID(QString uuid);
    QString getUUID();
    QVariantList permissions() const;
    bool hasPermission(QString entityID, QString type);

private:
    QString _description;
    QString _name;
    QString _uuid;
    QSharedPointer<GroupEntityContainer> _permissions;

private slots:
    void entitiesChangedSlot();

signals:
    void deleted(QString uuid);
    void nameChanged(QString uuid, QString groupName);
    void descriptionChanged(QString uuid, QString decriptio);
    void entitiesChanged(QString uuid, QVariantList entities);
    void typeChanged(QString uuid, QString type);
};

#endif // GROUP_H
