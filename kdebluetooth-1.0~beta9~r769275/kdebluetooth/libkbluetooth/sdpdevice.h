//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SDPDEVICE_H
#define SDPDEVICE_H

#include <vector>
#include <set>
#include "sdpservice.h"
#include "deviceaddress.h"

struct _sdp_list;
typedef struct _sdp_list sdp_list_t;
struct sdp_data_struct;
typedef struct sdp_data_struct sdp_data_t;

namespace KBluetooth
{
namespace SDP
{

typedef std::vector<KBluetooth::SDP::Service> ServiceVector;

class Device
{
private:
    KBluetooth::DeviceAddress bdDevAddr;
    ServiceVector serviceList;
    void updateServices();
    SDP::Attribute getAttribute(sdp_data_t* attrlist);
    std::set<uuid_t> searchUUIDs;
public:
    Device();
    void setTarget(const KBluetooth::DeviceAddress& bdAddr); 
    void setTarget(const KBluetooth::DeviceAddress& bdAddr, std::set<uuid_t> searchUUIDs);
    ~Device();
    ServiceVector& services();

    /** Returns TRUE if the device has at least a service with inside the required class Id.*/
	bool haveServiceClassID(SDP::uuid_t uuid);
    /** Get a vector of Rfcomm channels of the services having "uuid" in the class ID List*/
	std::vector<int> getRfcommList(SDP::uuid_t);
};

}
}

#endif
