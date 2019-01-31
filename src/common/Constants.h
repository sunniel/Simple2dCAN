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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_
// message names
namespace msg {
// CAN message types
extern const char* CAN_LOOK_UP;
extern const char* CAN_FINAL;
extern const char* CAN_JOIN;
extern const char* CAN_JOIN_REPLY;
extern const char* CAN_JOIN_REPLY_FAIL;
extern const char* CAN_ADD_NEIGHBOR;
extern const char* CAN_RM_NEIGHBOR;
extern const char* CAN_UPDATE_AREA;
extern const char* CAN_NEIGHBOR_UPDATE;
extern const char* CAN_NEIGHBOR_TAKEOVER;
extern const char* CAN_NEIGHBOR_EXCHANGE;
extern const char* CAN_ERS;
extern const char* CAN_ERS_REPLY;
extern const char* CAN_REPLICATE;
extern const char* CAN_FIX_REPLICA;
extern const char* CAN_FIX_LOAD;

// global schedules
extern const char* INIT_CAN;
extern const char* TRAFFIC_CAN;
extern const char* CHURN_SCAN_CYCLE;
extern const char* CHURN_CYCLE;
extern const char* DATA_DIST_CYCLE;

// Chord schedules
extern const char* CAN_MAINT;
}

namespace data{
// chord data type
extern const char* CAN_DATA_EMPTY;
}
#endif /* CONSTANTS_H_ */
