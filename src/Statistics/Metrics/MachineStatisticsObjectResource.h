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


#ifndef MACHINESTATISTICSOBJECTRESOURCE_H
#define MACHINESTATISTICSOBJECTRESOURCE_H

#include <QObject>
#include "Server/Resources/ObjectResource/ObjectResource.h"
#include "../Database/StatisticQueries.h"

class MachineStatisticsObjectResource : public ObjectResource
{
    Q_OBJECT

public:
    explicit MachineStatisticsObjectResource(QString resourceID, QVariantMap parameters, QObject *parent = nullptr);
    QVariantMap getObjectData() const override;
    ModificationResult setProperty(QString name, const QVariant &value, QString token) override;
    bool setFilter(QVariantMap query) override;

 private:
    void reset(QDateTime from, QDateTime to);
    QVariantMap     _metrics;
    QDateTime       _from;
    QDateTime       _to;
    QString         _resourceID;
    StatisticQueries* _queries = nullptr;
    static QVariantMap resetWorker(QString resourceID, QDateTime from, QDateTime to);

signals:

private slots:
    void futureResult();
};

#endif // MACHINESTATISTICSOBJECTRESOURCE_H
