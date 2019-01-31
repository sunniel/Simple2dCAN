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

#include <nlohmann/json.hpp>
#include "ContentDistributor.h"
#include "GlobalParametersAccess.h"
#include "GlobalNodeListAccess.h"
#include "../common/Util.h"
#include "../common/Constants.h"
#include "../messages/CANMessage_m.h"
#include "../messages/UDPControlInfo_m.h"

using json = nlohmann::json;

Define_Module(ContentDistributor);

ContentDistributor::ContentDistributor() {
    scan = NULL;
}

ContentDistributor::~ContentDistributor() {
    if (scan != NULL) {
        cancelAndDelete(scan);
    }
}

void ContentDistributor::initialize() {
    scan_cycle = par("scan_cycle");
    scan = new cMessage(msg::DATA_DIST_CYCLE);
    scheduleAt(simTime() + scan_cycle, scan);
    WATCH(coordinate);
    WATCH(value);
}

void ContentDistributor::handleMessage(cMessage *msg) {
    if (msg->isName(msg::DATA_DIST_CYCLE)) {
        EV << "coordinate: " << coordinate << " value: " << value << endl;
        if (!coordinate.empty()) {
            EV << "coordinate: " << endl;
            vector<string> parts;
            util::splitString(coordinate, ",", parts);
            int x = util::strToInt(parts[0]);
            int y = util::strToInt(parts[1]);

            CANInfo* sender = NULL;
            do {
                sender = GlobalNodeListAccess().get()->randCAN();
            } while (sender == NULL
                    || !GlobalNodeListAccess().get()->isReady(
                            sender->getId()));
            CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
            message->setSender(sender->getId());
            message->setHop(0);
            if (!value.empty()) {
                message->setType(CANMsgType::STORE);
                json content;
                content["x"] = x;
                content["y"] = y;
                content["value"] = value;
                message->setContent(content.dump().c_str());
                value.clear();
            } else {
                message->setType(CANMsgType::GET);
                json content;
                content["x"] = x;
                                content["y"] = y;
                message->setContent(content.dump().c_str());
            }
            IPvXAddress destAddr = GlobalNodeListAccess().get()->getCANAddr(
                    sender->getId());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(destAddr);
            message->setControlInfo(udpControlInfo);
            SimpleNodeEntry* destEntry = sender->getEntry();
            cSimpleModule::sendDirect(message, 0, 0,
                    destEntry->getTcpIPv4Gate());

            coordinate.clear();
        }
        scheduleAt(simTime() + scan_cycle, scan);
    }
}
