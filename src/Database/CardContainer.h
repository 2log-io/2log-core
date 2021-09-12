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

#ifndef CARDCONTAINER_H
#define CARDCONTAINER_H

#include <QObject>
#include "NestedArrayContainer.h"
#include <QSharedPointer>
#include <QDateTime>

class CardContainer : public NestedArrayContainer
{
    Q_OBJECT

 public:
    struct Card
    {
        QString     _cardID;
        int         _cardType = 0;
        bool        _expires = true;
        bool        _enabled = true;
        QDateTime   _expirationTime;
        QDateTime   _lastUsage;
		int			_origin = 0;

        Card(QVariant data)
        {
            QVariantMap map = data.toMap();
            _expirationTime = map["expirationDate"].toDateTime();
            _lastUsage = map["lastUsage"].toDateTime();
            _expires = map["expires"].toBool();
            _enabled  = map["active"].toBool();
            _cardID = map["cardID"].toString();
            _cardType = map["type"].toInt();
			_origin = map["origin"].toInt();
        }

        QVariantMap toVariant()
        {
            QVariantMap data;
            data["expirationDate"] = _expirationTime;
            data["lastUsage"] = _lastUsage;
            data["expires"] = _expires;
            data["active"] =  _enabled;
            data["cardID"] =  _cardID;
            data["type"] =  _cardType;
			data["origin"] = _origin;
            return data;
        }
    };

    QVariantMap setProperty(int index, QString property, QVariant value) override;

    explicit CardContainer(QString parentID, QObject *parent = nullptr);
   ~CardContainer() override;
    QVariantMap insert(QVariant data) override;
    bool canRead(iIdentityPtr user) const override;
    bool canWrite(iIdentityPtr user) const override;


private:
    void _insert(QVariantList data) override;

signals:

public slots:
};


class CardContainerFactory : public IContainerFactory
{
    Q_OBJECT

public:
    explicit CardContainerFactory(QObject* parent = nullptr) : IContainerFactory("users","cards", parent){}
    virtual QSharedPointer<NestedArrayContainer> createInsance(QString parentID){return QSharedPointer<NestedArrayContainer>(new CardContainer(parentID));}


};

#endif // CARDCONTAINER_H
