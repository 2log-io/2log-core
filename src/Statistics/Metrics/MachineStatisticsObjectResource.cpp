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

#include "MachineStatisticsObjectResource.h"
#include <QtConcurrent>
#include <QFuture>

MachineStatisticsObjectResource::MachineStatisticsObjectResource(QString resourceID, QVariantMap parameters, QObject *parent) : ObjectResource(nullptr, parent)
{
    _resourceID = resourceID;
    setDynamicContent(true);
    reset(QDateTime::fromString(parameters["from"].toString()),
            QDateTime::fromString(parameters["to"].toString()));

}

QVariantMap MachineStatisticsObjectResource::getObjectData() const
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

IResource::ModificationResult MachineStatisticsObjectResource::setProperty(QString name, const QVariant &value, QString token)
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

bool MachineStatisticsObjectResource::setFilter(QVariantMap query)
{
    reset(query["from"].toDateTime(), query["to"].toDateTime());
    return true;
}

void MachineStatisticsObjectResource::reset(QDateTime from, QDateTime to)
{
    _from = from;
    _to = to;

    QFutureWatcher<QVariantMap>* watcher = new QFutureWatcher<QVariantMap>(this);
    connect(watcher, &QFutureWatcher<QVariantMap>::finished,this, &MachineStatisticsObjectResource::futureResult);
    QFuture<QVariantMap> future = QtConcurrent::run(resetWorker, _resourceID, from, to);
    watcher->setFuture(future);
}

QVariantMap MachineStatisticsObjectResource::resetWorker(QString resourceID, QDateTime from, QDateTime to)
{
     QVariantMap metrics;
     double revenue =  StatisticQueries::getMachineRevenue(resourceID, from,to);
     metrics["revenue"] = revenue;
     double runtime =  StatisticQueries::getMachineRuntime( from,to, resourceID);
     metrics["runtime"] = runtime;
     return metrics;
}

void MachineStatisticsObjectResource::futureResult()
{
    QFutureWatcher<QVariantMap>* watcher = static_cast<QFutureWatcher<QVariantMap>*>(sender());
    if(watcher)
    {
        QVariantMap data = watcher->result();
        Q_EMIT propertyChanged("revenue", data["revenue"], nullptr);
        Q_EMIT propertyChanged("runtime", data["runtime"], nullptr);
        _metrics  = data;

        delete watcher;
        watcher = 0;
    }
}



