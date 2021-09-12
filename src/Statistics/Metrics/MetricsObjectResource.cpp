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


#include "MetricsObjectResource.h"
#include <QVariantMap>
#include <QtConcurrent>
#include <QFuture>

MetricsObjectResource::MetricsObjectResource(QVariantMap parameters, QObject* parent) : ObjectResource(nullptr, parent)
{
    setDynamicContent(true);
    reset(QDateTime::fromString(parameters["from"].toString()),
            QDateTime::fromString(parameters["to"].toString()));
}

QVariantMap MetricsObjectResource::getObjectData() const
{
    QVariantMap data;
    QMapIterator<QString, QVariant>it(_metrics);
    while(it.hasNext())
    {
        it.next();
        QVariantMap prop;
        prop["data"] = it.value();
        data.insert(it.key(), prop );
    }
    QVariantMap fromMap;
    fromMap["data"] = _from;
    data["from"] = fromMap;

    QVariantMap toMap;
    toMap["data"] = _to;
    data["to"] = toMap;

    return data;
}

IResource::ModificationResult MetricsObjectResource::setProperty(QString name, const QVariant &value, QString token)
{
    Q_UNUSED(token);
    ModificationResult result;
    QDateTime from = _from;
    QDateTime to = _to;

    QVariantMap data;
    data["data"] = value;
    data["lastupdate"] = QDateTime::currentMSecsSinceEpoch();
    result.data = data;

    if(name == "from")
    {
        from = value.toDateTime();
        reset(from, to);
    }
    else if(name =="to")
    {
        to = value.toDateTime();
        reset(from, to);
    }
    else
    {
        result.error = INVALID_PARAMETERS;
    }

    return result;
}

bool MetricsObjectResource::setFilter(QVariantMap query)
{
    reset(query["from"].toDateTime(), query["to"].toDateTime());
    return true;
}

QVariantMap MetricsObjectResource::resetWorker(QDateTime from, QDateTime to)
{
    QVariantMap metrics;
    int count = StatisticQueries::getActiveUserCount(from,to);
    metrics["userCount"] = count;

    double totalRevenue =  StatisticQueries::getTotalRevenue(from,to);
    metrics["totalRevenue"] = totalRevenue;

    QVariantList revenueByDays = StatisticQueries::getRevenuePerDay(from,to);
    metrics["revenueByDays"] = revenueByDays;

    int openCredit = StatisticQueries::getOpenCredit();
    metrics["openCredit"] = openCredit;

     QVariantList runtime = StatisticQueries::getMachineRuntime(from, to);
    metrics["machineRuntime"] = runtime;

    QVariantList machineRevenue = StatisticQueries::getMachineRevenue(from, to);
    metrics["machineRevenue"] = machineRevenue;

    int totalUsage = StatisticQueries::getTotalUsage(from, to);
    metrics["totalUsage"] = totalUsage;

    QVariantList usersPerDay = StatisticQueries::getUsersPerDay(from, to);
    metrics["usersPerDay"] = usersPerDay;

    int debts = StatisticQueries::getDebts();
    metrics["debts"] = debts;

    QVariantList averageMachineUsage = StatisticQueries::getAverageMachineUsage(from,to);
    metrics["averageMachineUsage"] = averageMachineUsage;
    return metrics;
}


void MetricsObjectResource::reset(QDateTime from, QDateTime to)
{
    _from = from;
    _to = to;

    QFutureWatcher<QVariantMap>* watcher = new QFutureWatcher<QVariantMap>(this);
    connect(watcher, &QFutureWatcher<QVariantMap>::finished,this, &MetricsObjectResource::futureResult);
    QFuture<QVariantMap> future = QtConcurrent::run(resetWorker, from, to);
    watcher->setFuture(future);
}


void MetricsObjectResource::futureResult()
{
    QFutureWatcher<QVariantMap>* watcher = static_cast<QFutureWatcher<QVariantMap>*>(sender());
    if(watcher)
    {
        QVariantMap data = watcher->result();
        Q_EMIT propertyChanged("userCount", data["userCount"], nullptr);
        Q_EMIT propertyChanged("totalRevenue", data["totalRevenue"], nullptr);
        Q_EMIT propertyChanged("revenueByDays", data["revenueByDays"], nullptr);
        Q_EMIT propertyChanged("openCredit", data["openCredit"], nullptr);
        Q_EMIT propertyChanged("machineRuntime", data["machineRuntime"], nullptr);
        Q_EMIT propertyChanged("machineRevenue", data["machineRevenue"], nullptr);
        Q_EMIT propertyChanged("usersPerDay", data["usersPerDay"], nullptr);
        Q_EMIT propertyChanged("debts", data["debts"], nullptr);
        Q_EMIT propertyChanged("totalUsage", data["totalUsage"], nullptr);
        Q_EMIT propertyChanged("averageMachineUsage", data["averageMachineUsage"], nullptr);
        _metrics  = data;

        delete watcher;
        watcher = 0;
    }
}

