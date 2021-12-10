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

#ifndef LOG_H
#define LOG_H
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QJsonDocument>
#include <QDebug>

struct Log
{
    enum EventType
    {
        BILL = 0,        //   Session
        SWITCH_ON = 1,      //     Etwas wurde angeschaltet
        SWITCH_OFF = 2,     //     Etwas wurde ausgeschaltet
        START = 3,          //     Eine Maschine wurde gestartet
        STOP = 4,           //     Eine Maschine wurde gestoppt
        EVENT = 5,          //     Allgemeine Events
        LOGIN = 6,          //     User hat sich bei angemeldet
        LOGOUT = 7,         //     User wurde abgemeldet / hat sich abgemeldet
        WARNING = 8,        //     Warnung
        ERROR = 9,          //     Fehler
        OPEN = 10,           //    Etwas wurde geöffnet (Schrank, Klappe)
        CLOSED = 11,         //    Etwas wurde geschlossen (Schrank, Klappe)
        TRANSFER = 12,       //    Konto wurde aufgeladen / etwas wurde abgebucht
        JOB = 13,            //     Log über einen Job mit Dauer
    };

    QString         resourceID;
    QString         userID;
    QString         cardID;
    EventType       event;
    int             units;
    int             price;
    QDateTime       timestamp;
    QDateTime       start;
    QDateTime       end;
    QString         description;
    QString         externalReference;
    QString         externalType;
    QString         logID;
    QString         userName;
    QString         executive;
    QVariantMap     userData;
    QVariant        internalAttachment;

    QVariantMap toVariant() const
    {
        QVariantMap log;
        log["resourceID"] = resourceID;

        if(!userID.isEmpty())
            log["userID"] = userID;

        log["cardID"] = cardID;
        log["timestamp"] = timestamp;
        log["logType"] = event;
        log["units"] = units;
        log["description"] = description;
        log["userName"] = userName;
        log["startTime"] = start;
        log["endTime"] = end;
        log["price"] = price;
        log["executive"] = executive;
        log["extType"] = externalType;
        log["extRef"] = externalReference;
        if(!logID.isEmpty())
            log["logID"] = logID;
        return log;
    }

    /*
        timestamp = QDateTime::fromMSecsSinceEpoch(docMap["timestamp"].toMap()["$date"].toLongLong());
        start = QDateTime::fromMSecsSinceEpoch(docMap["startTime"].toMap()["$date"].toLongLong());
        end = QDateTime::fromMSecsSinceEpoch(docMap["endTime"].toMap()["$date"].toLongLong());
    */

    Log(QVariantMap docMap)
    {
        logID = docMap["_id"].toMap()["$oid"].toString();
        resourceID = docMap["resourceID"].toString();
        userID = docMap["userID"].toString();
        cardID = docMap["cardID"].toString();
        timestamp = docMap["timestamp"].toDateTime();
        start = docMap["startTime"].toDateTime();
        end = docMap["endTime"].toDateTime();
        event = static_cast<EventType>(docMap["logType"].toInt());
        units = docMap["units"].toInt();
        description = docMap["description"].toString();
        userName = docMap["userName"].toString();
        price = docMap["price"].toInt();
        executive = docMap["executive"].toString();
        externalType = docMap["extType"].toString();
        externalReference = docMap["extRef"].toString();
        if(docMap.contains("user"))
        {
            QVariantList userList = docMap["user"].toList();
            if(!userList.isEmpty())
                userData = userList[0].toMap();
        }
    }

    Log()
    {
        timestamp = QDateTime::currentDateTime();
    }
};

#endif // LOG_H
