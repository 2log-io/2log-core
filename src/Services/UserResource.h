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


#ifndef USERRESOURCE_H
#define USERRESOURCE_H

#include <QObject>
#include "Server/Resources/ListResource/ListResource.h"
#include "../Database/UserAccess.h"
#include "../Database/FablabUser.h"

class UserResource : public ListResource
{

Q_OBJECT

public:
    UserResource(QVariantMap parameters, QObject* parent = nullptr);
    ~UserResource() override {}
    bool setFilter(QVariantMap filter) override;
    virtual QVariantList       getListData() const override;
    virtual bool               isPermittedToRead(QString token) const override;
    virtual int                getCount() const override;
    QVariant                   getItem(int idx, QString uuid) const override;

private:
    QVariantMap createTemplate(QVariantMap data) const;

public slots:
    void newUserInserted(fablabUserPtr user);
    void userDeleted(fablabUserPtr user);


private:
    QList<fablabUserPtr> _users;
    QMap<void*, int> _indexMap;
    QVariantMap _filter;
    void reIndex();
    void connectUser(fablabUserPtr user);
    void disconnectUser(fablabUserPtr user);
    int indexOf(QObject *ptr);

private slots:
    void nameChanged(QString uuid, QString name);
    void surnameChanged(QString uuid, QString surname);
    void roleChanged(QString uuid, QString role);
    void courseChanged(QString uuid, QString course);
    void eMailChanged(QString uuid, QString mail);
    void lastLoginChanged(QString uuid, QDateTime lastLogin);
    void balanceChanged(QString uuid, int balance);
    void stateChanged(QString uuid, FablabUser::UserState state);
};

#endif // USERRESOURCE_H
