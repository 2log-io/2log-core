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


#ifndef LOGACCESS_H
#define LOGACCESS_H

#include <QObject>
#include <QDateTime>
#include <QJsonDocument>
#include <QVariantMap>
#include <QVector>
#include "Log.h"

class LogAccess : public QObject
{
    Q_OBJECT

public:
     explicit LogAccess(QObject *parent = nullptr);
	 void insertLog(Log &log);
     static QList<Log> getLogs(QVariantMap filter, bool joinWithUsers = false);
     static QVariantList getLogsWithUser(QVariantMap filter);
     static LogAccess* instance();

private:
     static QList<Log> toLogs(QVariantList logs);

signals:
    void newLogInserted(Log log);

};

#endif // LOGACCESS_H
