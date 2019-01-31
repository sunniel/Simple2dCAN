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

#include "TrafficGenerator.h"

Define_Module(TrafficGenerator);

TrafficGenerator::TrafficGenerator() {
    traffic = new cMessage(msg::TRAFFIC_CAN);
}

TrafficGenerator::~TrafficGenerator() {
    // destroy self timer messages
    cancelAndDelete(traffic);
}

void TrafficGenerator::initialize() {
    traffic_cycle = par("traffic_cycle");
    scheduleAt(simTime() + traffic_cycle, traffic);
}

void TrafficGenerator::final() {
    ;
}

void TrafficGenerator::handleMessage(cMessage *msg) {
    if (msg->isName(msg::TRAFFIC_CAN)) {
        CANInfo* sender = NULL;
        CANInfo* target = NULL;
        do {
            sender = GlobalNodeListAccess().get()->randCAN();
            target = GlobalNodeListAccess().get()->randCAN();
        } while (target == NULL || sender == NULL
                || target->getId() == sender->getId()
                || !GlobalNodeListAccess().get()->isReady(sender->getId())
                || !GlobalNodeListAccess().get()->isReady(target->getId()));
        CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
        message->setType(CANMsgType::LOOK_UP);
        message->setSender(sender->getId());
        unsigned long length = GlobalParametersAccess().get()->getCANAreaSize();
        unsigned long x = uniform(0, length + 1);
        unsigned long y = uniform(0, length + 1);
//        string xy = to_string(x) + " " + to_string(y);
        json content;
        content["x"] = x;
        content["y"] = y;
//        message->setContent(xy.c_str());
        message->setContent(content.dump().c_str());
        message->setHop(0);

        IPvXAddress destAddr = GlobalNodeListAccess().get()->getCANAddr(
                sender->getId());
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(destAddr);
        message->setControlInfo(udpControlInfo);
        SimpleNodeEntry* destEntry = sender->getEntry();
        cSimpleModule::sendDirect(message, 0, 0, destEntry->getTcpIPv4Gate());
        scheduleAt(simTime() + traffic_cycle, traffic);

        cout << simTime() << " ["
                << GlobalNodeListAccess().get()->getCANProfile(sender->getId())
                << "] look up: " << content.dump() << endl;
    }
}
