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

#include "UnderlayConfigurator.h"
#include "../ctrl/CANCtrl.h"

Define_Module(UnderlayConfigurator);

UnderlayConfigurator::UnderlayConfigurator() {
    can_counter = 0;
}

UnderlayConfigurator::~UnderlayConfigurator() {
    disposeFailures();
}

int UnderlayConfigurator::numInitStages() const {
    return 1;
}

void UnderlayConfigurator::initialize(int stage) {
    nextFreeAddress = 0x1000001;
    globalNodeList = GlobalNodeListAccess().get();
    parameterList = GlobalParametersAccess().get();

    WATCH_MAP(death_schedule);

    // initialize Chord overlay
    cMessage* clientInit = new cMessage(msg::INIT_CAN);
    scheduleAt(0, clientInit);
}

void UnderlayConfigurator::handleMessage(cMessage* msg) {
    if (msg->isName(msg::INIT_CAN)) {
        handleCANInit(msg);
    }
}

void UnderlayConfigurator::handleCANInit(cMessage* msg) {
    const char* canType = par("can_type");
    const char* canName = par("can_name");

    // create the master node, i.e.,  the first node
    // create a new node
    cModuleType *moduleType = cModuleType::get(canType);
    cModule* parent = getParentModule();
    // create (possibly compound) module and build its submodules (if any)
    cModule* first = moduleType->create(canName, parent, can_counter + 1,
            can_counter);
    can_counter++;
    // set up parameters, if any
    first->finalizeParameters();
    first->buildInside();
    // create activation messages
    first->scheduleStart(0);
    first->callInitialize(0);

    // create address for the can control protocol
    CANCtrl* ctrl = check_and_cast<CANCtrl*>(first->getSubmodule("ctrl"));
    IPvXAddress firstAddr = IPAddress(nextFreeAddress++);
    ctrl->setIPAddress(firstAddr);
    ctrl->info.setId(
            util::getSHA1(firstAddr.get4().str() + "4000",
                    parameterList->getAddrSpaceSize())); // Hash(IP || port)
    first->callInitialize(1);

    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(first);
    CANInfo* info = new CANInfo(ctrl->getId(), first->getFullName());
    info->setEntry(entry);
    info->setId(ctrl->info.getId());
    ctrl->info.setId(info->getId());
    //add host to bootstrap oracle
    globalNodeList->addPeer(firstAddr, info);

    // init the master node
    ctrl->initArea();
    ctrl->startMaint();
    globalNodeList->updateCANProfile(ctrl->info);
    globalNodeList->addZone(ctrl->info);
    globalNodeList->canReady(ctrl->info.getId());
    addNode(true);

    delete msg;
}

void UnderlayConfigurator::removeNode(IPvXAddress& nodeAddr) {
    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    if (info != nullptr) {
        SimpleNodeEntry* destEntry = info->getEntry();
        cModule* node = destEntry->getUdpIPv4Gate()->getOwnerModule();
        node->callFinish();
        node->deleteModule();
        globalNodeList->killPeer(nodeAddr);
        parameterList->remoteHost(nodeAddr);
    }
}

void UnderlayConfigurator::addNode(bool init) {
    const char* canType = par("can_type");
    const char* canName = par("can_name");

    // search a bootstrap node to join
    CANInfo* bootstrap = NULL;
    do {
        bootstrap = GlobalNodeListAccess().get()->randCAN();
    } while (bootstrap == NULL
            || !GlobalNodeListAccess().get()->isReady(bootstrap->getId()));

    // create a new node
    cModuleType *moduleType = cModuleType::get(canType);
    cModule* parent = getParentModule();
    // create (possibly compound) module and build its submodules (if any)
    cModule* can = moduleType->create(canName, parent, can_counter + 1,
            can_counter);
    can_counter++;
    // set up parameters, if any
    can->finalizeParameters();
    can->buildInside();
    // create activation messages
    can->scheduleStart(0);
    can->callInitialize(0);

    // create address for the can control protocol
    CANCtrl* ctrl = check_and_cast<CANCtrl*>(can->getSubmodule("ctrl"));
    IPvXAddress addr = IPAddress(nextFreeAddress++);
    ctrl->setIPAddress(addr);
    ctrl->info.setId(
            util::getSHA1(addr.get4().str() + "4000",
                    parameterList->getAddrSpaceSize())); // Hash(IP || port)
    can->callInitialize(1);

    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(can);
    CANInfo* info = new CANInfo(ctrl->getId(), can->getFullName());
    info->setEntry(entry);
    info->setId(ctrl->info.getId());
    ctrl->info.setId(info->getId());
    //add host to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    // add the node to the overlay through bootstrap
    if (init) {
        ctrl->join(bootstrap->getId(), "init");
    } else {
        ctrl->join(bootstrap->getId());
    }
}

void UnderlayConfigurator::initNodeAdded() {
    if (globalNodeList->canSize() < parameterList->getCANInitSize()) {
        addNode(true);
    }
}

void UnderlayConfigurator::disposeFailures() {
//    try {
//        for (map<string, Failure*>::iterator it = failures.begin();
//                it != failures.end(); ++it) {
//            Failure* failure = it->second;
//            if (failure != NULL) {
//                cancelAndDelete(failure);
//            }
//        }
//        failures.clear();
//    } catch (exception& e) {
//        EV << e.what() << endl;
//    }
}
