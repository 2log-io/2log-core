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


#ifndef ITEMCONTAINER_H
#define ITEMCONTAINER_H

#include <QObject>
#include <QVariant>
#include "Server/Authentication/AuthentificationService.h"
#include "Server/Authentication/IUser.h"

/*!
    \class NestedArrayContainer
    \brief This class helps to create and access nested arrays within MongoDB Documents

    \ingroup 2log

    Derive this class to create and access list data that belongs to a specific MongoDB Document.
    This class is for example used to insert / access and edit
    - user permissions
    - user cards
    - user groups
*/

class NestedArrayContainer : public QObject
{
    Q_OBJECT
    friend class UserAccess;

public:
    virtual         ~NestedArrayContainer();
    QVariantList    getData();
    QVariantMap     getItem(int index);

    virtual QVariantMap setProperty(int index, QString name, QVariant data) = 0;
    virtual QVariantMap insert(QVariant) = 0;

    virtual bool        canRead(iIdentityPtr user) const = 0;
    virtual bool        canWrite(iIdentityPtr user) const = 0;

    virtual int         count() const;
    virtual bool        removeItem(int index);
    bool                load();

protected:
    explicit        NestedArrayContainer(QString collection, QString parentID, QString itemType, QObject *parent = nullptr);
    QVariantMap     _setProperty(int index, QString name, QVariant data);
    QVariantMap     _appendItem(QVariantMap data);
    QVariantMap     _createTemplate(QVariantMap data) const;
    QVariantList    _data;


private:
    virtual void    _insert(QVariantList data);
    QString         _parentID;
    QString         _field;
    QString         _collection;


signals:
     void destroyed(QString collection, QString parentID, QString itemType);
     void itemAppended(QVariant data);
     void itemRemoved(int index);
     void propertyChanged(int index, QString property, QVariant data);
     void arrayDataChanged();
};


class IContainerFactory : public QObject
{
    Q_OBJECT

public:
    explicit IContainerFactory(QString collection, QString fieldType, QObject* parent = nullptr) : QObject(parent), _collection(collection), _fieldType(fieldType){}
    QString getCollection(){return _collection;}
    QString getFieldType(){return _fieldType;}
    virtual QSharedPointer<NestedArrayContainer> createInsance(QString parentID) = 0;

private:
    QString _collection;
    QString _fieldType;
};

#endif // ITEMCONTAINER_H
