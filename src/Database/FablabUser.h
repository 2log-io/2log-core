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

#ifndef FABLABUSER_H
#define FABLABUSER_H

#include <QString>
#include <QDateTime>
#include <QVariant>
#include "Server/Authentication/IUser.h"
#include <QReadWriteLock>

//class UserAccess;
class FablabUser : public IUser
{
    Q_OBJECT
    friend class UserAccess;

public:
    enum UserState
    {
        IDLE = 0,
        ACTIVE,
        DISABLED,
        DELETED
    };

   ~FablabUser() override;
    QVariantMap toVariant() const;

    QString getName() const;
    void setName(const QString &value);

    QString getSurname() const;
    void setSurname(const QString &value);

    QString getCourse() const;
    void setCourse(const QString& value);

    void setRole(const QString& value);
    QString getRole() const;

    QString getEMail() const override;
    void setEMail(const QString &value) override;

    QString getUuid() const;

    int getBalance() const;
    void setBalance(int value);

    QDateTime getLastLogin() const;
    void setLastLogin(const QDateTime &value);

    QDateTime getCreation() const;

    bool setPassword(QString newPassword) override;

    QString identityID() const override;
    QString userLogin() const override;


    FablabUser::UserState getState() const;
    void setState(const UserState &value);

    UserData userData() const override;

    bool checkPassword(QString password) override;
    bool isAuthorizedTo(QString permission) override;
    bool deleteUser();

private:
    FablabUser(QVariantMap userMap, QObject* parent = nullptr);
    void        setCreation(const QDateTime &value);
    void         setUuid(QString uuid);
    QString     _name;
    QString     _surname;
    QString     _eMail;
    QString     _uuid;
    QString     _role;
    QString     _course;
    QString     _passwordHash;
    int         _balance;
    QDateTime   _lastLogin;
    QDateTime   _creation;
    UserState   _state;
    mutable QReadWriteLock _lock;

signals:
    void deleted(QString uuid);
    void uuidChanged(QString uuid);
    void nameChanged(QString uuid, QString name);
    void eMailChanged(QString uuid, QString mail);
    void lastLoginChanged(QString uuid, QDateTime lastLogin);
    void balanceChanged(QString uuid, int balance);
    void stateChanged(QString uuid, UserState state);
    void surnameChanged(QString uuid, QString surname);
    void roleChanged(QString uuid, QString role);
    void courseChanged(QString uuid, QString course);
    void userPropertyChanged(QString property, QVariant value);
};

#endif // USER_H
