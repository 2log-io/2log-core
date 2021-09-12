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


#include "LogAccess.h"
#include "MongoDB.h"
#include <QJsonDocument>
#include <QDebug>

Q_GLOBAL_STATIC(LogAccess, logAccess);

void LogAccess::insertLog(Log &log)
{
	log.logID = MongoDB::instance()->insertDocument("logs", log.toVariant());
    Q_EMIT newLogInserted(log);
}

QList<Log> LogAccess::getLogs(QVariantMap filter, bool joinWithUsers)
{
    QVariantMap match = filter["match"].toMap();
    QVariantMap sort = filter["sort"].toMap();

    if(filter.contains("from"))
    {
        QDateTime from = filter.value("from").toDateTime();
        QDateTime to = filter.contains("to") ? filter["to"].toDateTime() : QDateTime::currentDateTime();

        //check logs that started before "from" and ended after "from"
        QVariantMap rule1Map;
        QVariantMap rule1A;
        QVariantMap rule1AA;
        rule1AA["$lt"] = from;
        rule1A["startTime"] = rule1AA;
        QVariantMap rule1B;
        QVariantMap rule1BB;
        rule1BB["$gte"] = from;
        rule1B["endTime"] = rule1BB;
        QVariantList rule1List;
        rule1List << rule1A << rule1B;
        rule1Map["$and"] = rule1List;

        // check logs that started in range
        QVariantMap rule2A;
        rule2A["$gte"] = from;
        rule2A["$lt"] = to;
        QVariantMap rule2Map;
        rule2Map["startTime"] = rule2A;

        // check also timestamps between from and to
        QVariantMap rule3A;
        rule3A["$gte"] = from;
        rule3A["$lt"] = to;
        QVariantMap rule3map;
        rule3map["timestamp"] = rule3A;
        QVariantList orList;
        orList << rule1Map << rule2Map << rule3map;
        QVariantMap orMap;
        match.insert("$or", orList);
    }

    int limit = filter.value("limit",-1).toInt();

    if(joinWithUsers)
    {
        QVariantMap project;
        QVariantMap typeConversion;
        typeConversion["$toObjectId"] = "$userID";
        project["userIdObj"] = typeConversion;
        return toLogs(MongoDB::instance()->JoinWithPipeline("logs",limit, match, sort, QVariantMap(), QVariantMap(), "users", "userIdObj", "_id", "user", project));
    }
    else
        return toLogs(MongoDB::instance()->selectPipeline("logs",limit, match, sort));
}

QVariantList LogAccess::getLogsWithUser(QVariantMap filter)
{
    QList<Log> items = getLogs(filter, true);
    QVariantList logs;
    for(int i = 0; i < items.count(); i++)
    {
        Log& logItem = items[i];
        QVariantMap log = logItem.toVariant();
        log["email"] = logItem.userData["mail"];
        logs.append(log);
    }

    return logs;
}


LogAccess *LogAccess::instance()
{
    return logAccess;
}

QList<Log> LogAccess::toLogs(QVariantList logs)
{
    QList<Log> logVec;
    foreach(QVariant log, logs)
    {
        QVariantMap logMap = log.toMap();
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(logMap["timestamp"].toMap()["$date"].toLongLong());
        QDateTime start = QDateTime::fromMSecsSinceEpoch(logMap["startTime"].toMap()["$date"].toLongLong());
        QDateTime end = QDateTime::fromMSecsSinceEpoch(logMap["endTime"].toMap()["$date"].toLongLong());

        Log item = Log(log.toMap());
        item.timestamp = timestamp;
        item.start = start;
        item.end = end;

        logVec.append(item);
    }
    return logVec;
}


LogAccess::LogAccess(QObject *parent) : QObject(parent)
{
}
