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



#ifndef ACCESSLOGIC_H
#define ACCESSLOGIC_H

#include <QObject>
#include <QVariant>

class AccessLogic : public QObject
{
    Q_OBJECT

public:
    explicit AccessLogic(QObject *parent = nullptr);
    static QVariant checkPermission(QString cardID, QString resourceID);
    static QVariant getUserForCardID(QString cardID);
    static QVariant addUser(QVariantMap user, QVariantList permissions, QVariantMap cardID, QVariantList gorups = QVariantList());
    static QVariant addOrUpdateUser(QVariantMap user, QVariantList permissions, QVariantMap card, QVariantList groups);
    static QVariant deleteUser(QString userID);
    static bool resetPassword(QString userID);
    static QString getRandomString(int count);
};

#endif // ACCESSLOGIC_H
