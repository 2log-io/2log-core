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


#ifndef STATISTICQUERIES_H
#define STATISTICQUERIES_H

#include <QDateTime>
#include <QVariantList>

class StatisticQueries
{
public:
    static int             getActiveUserCount(QDateTime from, QDateTime to);
    static int             getOpenCredit();
    static int             getDebts();
    static QVariantList    getAverageMachineUsage(QDateTime from, QDateTime to);
    static QVariantList    getMachineRuntime(QDateTime from, QDateTime to);
    static QVariantList    getMachineRevenue(QDateTime from, QDateTime to);
    static double           getMachineRevenue(QString resourceID, QDateTime from, QDateTime to);
    static int             getMachineRuntime(QDateTime from, QDateTime to, QString machineID);
    static double          getTotalRevenue(QDateTime from, QDateTime to);
    static QVariantList    getRevenuePerDay(QDateTime from, QDateTime to);
    static QVariantList    getUsersPerDay(QDateTime from, QDateTime to);
    static int             getTotalUsage(QDateTime from, QDateTime to);
    int                    getMachineUsage(QString resourceID, QDateTime from, QDateTime to);
};

#endif // STATISTICQUERIES_H
