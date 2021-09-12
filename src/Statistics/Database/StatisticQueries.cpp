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

#include "StatisticQueries.h"
#include "Database/MongoDB.h"
#include <QVariant>
#include <QDebug>
#include <QJsonDocument>
#include "src/Database/Log.h"


int StatisticQueries::getActiveUserCount(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    QVariantMap group1;
    group1["_id"] = "$userID";

    QVariantMap val1;
    val1["$sum"] = 1;
    group1["value"] = val1;

     QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), QVariantMap(), group1, QVariantMap(),QVariantMap(), "value");
     if(!result.isEmpty())
     {
         return result.at(0).toMap()["value"].toInt();
     }

     return -1;
}

int StatisticQueries::getOpenCredit()
{

    QVariantMap match;
    QVariantMap gte;
    gte["$gte"] = 0;
    match["balance"] = gte;

    //#############

    QVariantMap project;
    project["value"] ="$balance";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;
    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("users", -1, match, QVariantMap(), project, group);
    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toInt();
    }

    return -1;
}



int StatisticQueries::getDebts()
{
    QVariantMap match;
    QVariantMap lte;
    lte["$lte"] = 0;
    match["balance"] = lte;

    //#############

    QVariantMap project;
    project["value"] ="$balance";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;
    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("users", -1, match, QVariantMap(), project, group);

    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toInt();
    }

    return -1;
}

QVariantList StatisticQueries::getAverageMachineUsage(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    QVariantMap range;
    range["$gt"] = QDateTime::fromMSecsSinceEpoch(0);
    match["startTime"] = range;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#############

    QVariantMap project;
    QVariantList subParams;
    subParams << "$endTime" << "$startTime";
    QVariantMap sub;
    sub["$subtract"] = subParams;

    project["runtime"] = sub;
    project["ts"] = "$timestamp";
    project["resourceID"] = "$resourceID";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    QVariantMap id;
    id["resourceID"] = "$resourceID";
    group["_id"] = id;


    QVariantMap value;
    value["$avg"] = "$runtime";
    group["value"] = value;

    //#############

    QVariantMap sort;
    sort["value"] = -1;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, sort, project, group);

    qDebug().noquote()<<QJsonDocument::fromVariant(result).toJson();

    if(!result.isEmpty())
    {
        return result;
    }

    return QVariantList();
}

int StatisticQueries::getMachineRuntime(QDateTime from, QDateTime to, QString machineID)
{
    QVariantMap match;

    if(from.isValid() && to.isValid())
    {
        QVariantMap time;
        time["$gte"] = from;
        time["$lte"] = to;
        match["timestamp"] = time;
    }

    match["logType"] = Log::JOB;
    match["resourceID"] = machineID;

    //#############

    QVariantMap project;
    QVariantList subParams;
    subParams << "$endTime" << "$startTime";
    QVariantMap sub;
    sub["$subtract"] = subParams;

    project["runtime"] = sub;
    project["ts"] = "$timestamp";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;

    QVariantMap value;
    value["$sum"] = "$runtime";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), project, group);
    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toInt();
    }

    return -1;
}

QVariantList StatisticQueries::getMachineRuntime(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;

    QVariantMap range;
    range["$gt"] = QDateTime::fromMSecsSinceEpoch(0);
    match["startTime"] = range;
    match["timestamp"] = time;
    match["logType"] = Log::JOB;
//    match["resourceID"] = machineID;

    //#############

    QVariantMap project;
    QVariantList subParams;
    subParams << "$endTime" << "$startTime";
    QVariantMap sub;
    sub["$subtract"] = subParams;

    project["runtime"] = sub;
    project["ts"] = "$timestamp";
    project["resourceID"] = "$resourceID";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    QVariantMap id;
    id["resourceID"] = "$resourceID";
    group["_id"] = id;


    QVariantMap value;
    value["$sum"] = "$runtime";
    group["value"] = value;

    //#############

    QVariantMap sort;
    sort["value"] = -1;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, sort, project, group);
    if(!result.isEmpty())
    {
        return result;
    }

    return QVariantList();
}

QVariantList StatisticQueries::getMachineRevenue(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#############

    QVariantMap project;

    project["name"] = "value";
    project["value"] ="$price";
    project["resourceID"] = "$resourceID";

    project["_id"] = 0;

    //#############

    QVariantMap group;
    QVariantMap id;
    id["resourceID"] = "$resourceID";
    group["_id"] = id;

    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;

    //#############

    QVariantMap sort;
    sort["value"] = -1;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, sort, project, group);


    if(!result.isEmpty())
    {
        return result;
    }

    return QVariantList();
}

double StatisticQueries::getMachineRevenue(QString resourceID, QDateTime from, QDateTime to)
{
    QVariantMap match;

    if(from.isValid() && to.isValid())
    {
        QVariantMap time;
        time["$gte"] = from;
        time["$lte"] = to;
        match["timestamp"] = time;
    }

    match["logType"] = Log::BILL;
    match["resourceID"] = resourceID;

    //#############

    QVariantMap project;

    project["name"] = "value";
    project["value"] ="$price";

    project["_id"] = 0;

    //#############

    QVariantMap group;
    QVariantMap id;
    group["_id"] = 0;
    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;


    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), project, group);
    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toDouble();
    }

    return -1;
}

double StatisticQueries::getTotalRevenue(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#############

    QVariantMap project;

    project["name"] = "value";
    project["value"] ="$price";
    project["ts"] = "$timestamp";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;

    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), project, group);
    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toDouble();
    }

    return -1;
}

QVariantList StatisticQueries::getRevenuePerDay(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#################

    QVariantMap project;

    QVariantMap year;
    year["$year"] = "$timestamp";

    QVariantMap day;
    day["$dayOfYear"] = "$timestamp";


    project["day"] = day;
    project["name"] = "value";
    project["value"] ="$price";
    project["ts"] = "$timestamp";
    project["_id"] = 0;

    //#################

    QVariantMap group;

    QVariantMap id;
    id["day"] = "$day";
    id["year"] = "$year";
    group["_id"] = id;

    QVariantMap value;
    value["$sum"] = "$value";
    group["value"] = value;

    QVariantMap ts;
    ts["$min"] = "$ts";
    group["ts"] = ts;

    QVariantMap sort;
    sort["ts"] = 1;

     //#################

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, sort, project, group);

    if(!result.isEmpty())
    {
        return result;
    }

    return QVariantList();
}

QVariantList StatisticQueries::getUsersPerDay(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#################

    QVariantMap group;
    QVariantMap day;
    day["$dayOfYear"] = "$timestamp";
    QVariantMap year;
    year["$year"] = "$timestamp";

    QVariantMap id;
    id["day"] = day;
    id["year"] = year;
    group["_id"] = id;

    //--------

    QVariantMap value;
    value["$addToSet"] = "$userID";
    group["value"] = value;

    QVariantMap ts;
    ts["$min"] = "$timestamp";
    group["ts"] = ts;


    //#################

    QVariantMap sort;
    sort["ts"] = 1;

    //#################

    QVariantMap size;
    size["$size"] = "$value";
    QVariantMap project;
    project["ts"] = "$ts";
    project["value"] = size;

    //#################


    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, sort, QVariantMap(), group,QVariantMap(), project);


    if(!result.isEmpty())
    {
        return result;
    }

    return QVariantList();
}


int StatisticQueries::getTotalUsage(QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::BILL;

    //#############

    QVariantMap project;
    QVariantList subParams;

    subParams << "$endTime" << "$startTime";
    QVariantMap sub;
    sub["$subtract"] = subParams;
    project["duration"] = sub;
    project["ts"] = "$timestamp";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;

    QVariantMap value;
    value["$sum"] = "$duration";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), project, group);

    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toInt();
    }

    return -1;
}




int StatisticQueries::getMachineUsage(QString resourceID, QDateTime from, QDateTime to)
{
    QVariantMap match;
    QVariantMap time;
    time["$gte"] = from;
    time["$lte"] = to;
    match["timestamp"] = time;
    match["logType"] = Log::JOB;
    match["resourceID"] = resourceID;

    //#############

    QVariantMap project;
    QVariantList subParams;

    subParams << "$endTime" << "$startTime";
    QVariantMap sub;
    sub["$subtract"] = subParams;
    project["duration"] = sub;
    project["ts"] = "$timestamp";
    project["_id"] = 0;

    //#############

    QVariantMap group;
    group["_id"] = 0;

    QVariantMap value;
    value["$sum"] = "$duration";
    group["value"] = value;

    QVariantList result = MongoDB::instance()->selectPipeline("logs", -1, match, QVariantMap(), project, group);

    if(!result.isEmpty())
    {
        return result.at(0).toMap()["value"].toInt();
    }

    return -1;
}


