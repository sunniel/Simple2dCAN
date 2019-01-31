//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "UniformChurnGenerator.h"

Define_Module(UniformChurnGenerator);

UniformChurnGenerator::UniformChurnGenerator() {
    churn = NULL;
}

UniformChurnGenerator::~UniformChurnGenerator() {
    if (churn != NULL) {
        cancelAndDelete(churn);
    }
}

void UniformChurnGenerator::initialize() {
    departRate = par("depart_rate");
    arrivalRate = par("arrival_rate");
    churnCycle = par("churn_cycle");
    max = par("overlay_size_max");
    min = par("overlay_size_min");
    churn = new cMessage(msg::CHURN_CYCLE);
    scheduleAt(simTime() + churnCycle, churn);
}

void UniformChurnGenerator::handleMessage(cMessage *msg) {
    if (msg->isName(msg::CHURN_CYCLE)) {
        double depart = uniform(0, 1);
        int size = GlobalNodeListAccess().get()->canSize();
        if (depart < departRate && size > min) {
            CANInfo* toRemove = NULL;
            int zoneNumInCharge = 0;
            do {
                toRemove = GlobalNodeListAccess().get()->randCAN();
                zoneNumInCharge = GlobalNodeListAccess().get()->zonesInCharge(
                        toRemove->getId());
                /*
                 * Restrict the selection of node failure, since currently the
                 * zone re-assignement algorithm is not implemented. This restriction
                 * can be removed the implementation of the zone re-assignment algorithm.
                 */
            } while (toRemove == NULL || zoneNumInCharge < 1
                    || zoneNumInCharge > 2);
            IPvXAddress addrToRemove = GlobalNodeListAccess().get()->getCANAddr(
                    toRemove->getId());
            UnderlayConfiguratorAccess().get()->removeNode(addrToRemove);

            cout << simTime() << " remove node: " << toRemove->getId() << endl;

            GlobalStatisticsAccess().get()->DEPARTURE++;
        }
        double arrival = uniform(0, 1);
        if (arrival < arrivalRate && size < max) {
            UnderlayConfiguratorAccess().get()->addNode();
            GlobalStatisticsAccess().get()->ARRIVAL++;
        }
        scheduleAt(simTime() + churnCycle, churn);
    }
}
