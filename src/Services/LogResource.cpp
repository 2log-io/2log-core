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


#include "LogResource.h"


LogResource::LogResource(QVariantMap parameters, QObject* parent) : ListResource(nullptr, parent)
{
    setStorage(new ListResourceTemporaryStorage(this));
    setAllowUserAccess(false);
    setDynamicContent(true);
    setFilter(parameters);
    connect(LogAccess::instance(), &LogAccess::newLogInserted, this, &LogResource::newLogInserted);
}

LogResource::~LogResource()
{
}

bool LogResource::setFilter(QVariantMap filter)
{
    this->clearList();
    _filter = filter;    
    QFutureWatcher<QVariantList>* watcher = new QFutureWatcher<QVariantList>(this);
    connect(watcher, &QFutureWatcher<QVariantList>::finished,this, &LogResource::futureResult);
    QFuture<QVariantList> future = QtConcurrent::run(LogAccess::getLogsWithUser, filter);
    watcher->setFuture(future);
    return true;
}


void LogResource::newLogInserted(Log log)
{
    int sort = _filter["sort"].toMap()["timestamp"].toInt();
    int limit = _filter.value("limit",-1).toInt();
    if(fitsToFilter(log))
    {       
        if(sort == -1)
        {
            if(limit > 0 && this->getCount() == limit)
            {
                this->removeItem(this->getCount() -1);
            }
            this->insertAt(log.toVariant(), 0);
        }
    }
}

void LogResource::futureResult()
{
    QFutureWatcher<QVariantList>* watcher = static_cast<QFutureWatcher<QVariantList>*>(sender());
    if(watcher)
    {
        resetData(watcher->result());
        delete watcher;
        watcher = 0;
    }
}

bool LogResource::fitsToFilter(Log& log)
{
    QVariantMap match = _filter["match"].toMap();

    return  (!match.contains("userID")     || (log.userID == match["userID"].toString()))
        &&  (!match.contains("resourceID") || (log.resourceID == match["resourceID"].toString()))
        &&  (!match.contains("logType")    || (log.event == match["logType"].toInt()));
    //TODO: check timerange!
}

