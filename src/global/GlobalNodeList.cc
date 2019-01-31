//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file GlobalNodeList.cc
 * @author Markus Mauch, Robert Palmer, Ingmar Baumgart
 */

#include <iostream>
#include "GlobalNodeList.h"
#include "../common/MiscWatch.h"

Define_Module(GlobalNodeList);

std::ostream& operator<<(std::ostream& os, const BootstrapEntry& entry) {
    os << " " << *(entry.info);

    return os;
}

void GlobalNodeList::initialize() {
    WATCH_UNORDERED_MAP(peerStorage.getPeerHashMap());
    WATCH_MAP(cans);
    WATCH_MAP(canProfiles);
    WATCH_SET(zoneInCharges);
}

void GlobalNodeList::handleMessage(cMessage* msg) {

}

void GlobalNodeList::addPeer(const IPvXAddress& ip, PeerInfo* info) {
    BootstrapEntry temp;
    temp.info = info;

    peerStorage.insert(std::make_pair(ip, temp));

    cans.insert( { dynamic_cast<CANInfo*>(info)->getId(), ip });
    states.insert( { dynamic_cast<CANInfo*>(info)->getId(), false });
}

void GlobalNodeList::updateCANProfile(CANProfile profile) {
    unsigned long id = profile.getId();
    canProfiles[id] = profile;
}

CANProfile GlobalNodeList::getCANProfile(unsigned long canId) {
    if (canProfiles.count(canId) > 0) {
        return canProfiles[canId];
    } else {
        return CANProfile();
    }
}

void GlobalNodeList::addZone(CANProfile zone) {
    zoneInCharges.insert(zone);
}

void GlobalNodeList::delZone(CANProfile zone) {
    zoneInCharges.erase(zone);
}

int GlobalNodeList::zonesInCharge(unsigned long canId){
    int zones = 0;
    for(auto elem : zoneInCharges){
        if(elem.getId() == canId){
            zones++;
        }
    }
    return zones;
}

bool GlobalNodeList::validZone(CANProfile zone) {
    return zoneInCharges.count(zone) > 0;
}

void GlobalNodeList::killPeer(const IPvXAddress& ip) {
    PeerHashMap::iterator it = peerStorage.find(ip);
    if (it != peerStorage.end()) {
        unsigned long idToKill =
                dynamic_cast<CANInfo*>(it->second.info)->getId();
        peerStorage.erase(it);

        // remove CAN profile
        cans.erase(idToKill);
        canProfiles.erase(idToKill);
        states.erase(idToKill);

        // remove the zone(s) managed by the node
        for (set<CANProfile>::iterator it = zoneInCharges.begin();
                it != zoneInCharges.end();) {
            CANProfile element = (*it);
            if (element.getId() == idToKill) {
                it = zoneInCharges.erase(it);
            }else{
                it++;
            }
        }
    }
}

PeerInfo* GlobalNodeList::getPeerInfo(const TransportAddress& peer) {
    return getPeerInfo(peer.getIp());
}

PeerInfo* GlobalNodeList::getPeerInfo(const IPvXAddress& ip) {
    PeerHashMap::iterator it = peerStorage.find(ip);

    if (it == peerStorage.end())
        return NULL;
    else
        return it->second.info;
}

std::vector<IPvXAddress>* GlobalNodeList::getAllIps() {
    std::vector<IPvXAddress>* ips = new std::vector<IPvXAddress>;

    const PeerHashMap::iterator it = peerStorage.begin();

    while (it != peerStorage.end()) {
        ips->push_back(it->first);
    }

    return ips;
}

bool GlobalNodeList::isUp(unsigned long canId) {
    if (cans.count(canId) > 0) {
        return true;
    }
    return false;
}

bool GlobalNodeList::isReady(unsigned long canId) {
    if (cans.count(canId) > 0 && states[canId]) {
        return true;
    }
    return false;
}

void GlobalNodeList::canReady(unsigned long canId) {
    states[canId] = true;
}

IPvXAddress GlobalNodeList::getCANAddr(unsigned long canId) {
    IPvXAddress addr;
    if (cans.count(canId) > 0) {
        addr = cans[canId];
    }
    return addr;
}

int GlobalNodeList::canSize() {
    return cans.size();
}

CANInfo* GlobalNodeList::randCAN() {
    // Retrieve all keys
    vector<unsigned long> keys;
    for (auto elem : cans)
        keys.push_back(elem.first);
    int random = (int) uniform(0, keys.size());
    unsigned long key = keys[random];
    IPvXAddress addr = cans[key];
    CANInfo* info = dynamic_cast<CANInfo*>(getPeerInfo(addr));
    return info;
}
