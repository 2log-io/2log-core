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


#ifndef LOGRESOURCE_H
#define LOGRESOURCE_H

#include <QObject>
#include "Server/Resources/ListResource/ListResource.h"
#include "Storage/ListResourceTemporaryStorage.h"
#include "../Database/LogAccess.h"

#include <QFuture>
#include <QtConcurrent>

class LogResource : public ListResource
{

public:
    LogResource(QVariantMap parameters, QObject* parent = nullptr);
    ~LogResource() override;
    bool setFilter(QVariantMap filter) override;

public slots:
    void newLogInserted(Log log);

private slots:
    void futureResult();

private:
    ListResourceTemporaryStorage           _storage;
    QSet<QString>                          _logSet;
    QVariantMap                            _filter;
    QFutureWatcher<QVariantList>*          _watcher;
    bool                                    fitsToFilter(Log &log);
};

#endif // LOGRESOURCE_H
