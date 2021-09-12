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


#include "NestedDataContainerManager.h"


Q_GLOBAL_STATIC(NestedDataContainerManager, nestedDataContainerManager);
NestedDataContainerManager::NestedDataContainerManager(QObject *parent) : QObject(parent)
{
}

void NestedDataContainerManager::registerArrayFactory(IContainerFactory *factory)
{
    _factories[factory->getCollection()][factory->getFieldType()] = factory;
}

nestedArrayPtr NestedDataContainerManager::getNestedArrayData(QString collection, QString parentID, QString field)
{
    nestedArrayPtr nestedData =  _nestedData.value(collection).value(field).value(parentID);
    if(!nestedData.isNull())
        return nestedData;

    IContainerFactory* factory = _factories.value(collection).value(field);
    if(factory)
    {
        nestedData = factory->createInsance(parentID);
        if(nestedData->load())
        {
            connect(nestedData.data(), &NestedArrayContainer::destroyed, this, &NestedDataContainerManager::nestedDataContainerDeleted);
            _nestedData[collection][field][parentID] = nestedData;
            return nestedData;
        }
    }

    return nestedArrayPtr();
}

NestedDataContainerManager *NestedDataContainerManager::instance()
{
    return nestedDataContainerManager;
}

void NestedDataContainerManager::nestedDataContainerDeleted(QString collection, QString parentID, QString field)
{
    _nestedData[collection][field].remove(parentID);
}
