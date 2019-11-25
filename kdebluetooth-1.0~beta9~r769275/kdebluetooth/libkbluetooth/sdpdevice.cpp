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

#include <stdlib.h>
#include <assert.h>
#include "sdpdevice.h"
#include <kdebug.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

using namespace std;

namespace KBluetooth
{
namespace SDP
{

Device::Device()
{}

void Device::setTarget(const KBluetooth::DeviceAddress& bdAddr)
{ 
    set<uuid_t> uidSet;
    uidSet.insert(uuid_t("0x1002")); // Public Browse Group
    setTarget(bdAddr, uidSet);
}

void Device::setTarget(const KBluetooth::DeviceAddress& bdAddr, 
    std::set<uuid_t> uuids)
{
    this->bdDevAddr = bdAddr;
    this->searchUUIDs = uuids;
    updateServices();
}

Device::~Device()
{}

ServiceVector& Device::services()
{
    return serviceList;
}

void Device::updateServices()
{
    serviceList.clear();
    sdp_session_t *session;
    bdaddr_t bdAddr;
    str2ba(QString(bdDevAddr).ascii(), &bdAddr);
    bdaddr_t bdaddr_any = {{0,0,0,0,0,0}};
    session = sdp_connect(&bdaddr_any, &bdAddr, 0);
    if (!session)
    {
        kdWarning() << "sdp_connect(" << QString(DeviceAddress(bdAddr))
            << ") failed"  << endl;
        return; // error
    }

    uint32_t range = 0x0000ffff;
    sdp_list_t* attrid = sdp_list_append(0, &range);
    
    sdp_list_t* search = NULL;
    const int maxuuids = 12; // Limit introduced by the Bluetooth standard
    ::uuid_t uuidMap[maxuuids];
    int curUuid = 0;
    set<KBluetooth::SDP::uuid_t>::iterator it;
    for (it = searchUUIDs.begin(); it != searchUUIDs.end(); ++it) {
        kdDebug() << "appended sdp uuid: " << QString(*it) << endl;
        uuidMap[curUuid] = *it;
        search = sdp_list_append(search, uuidMap+curUuid);
        if (++curUuid == maxuuids) break;
    }
    
    sdp_list_t* seq = NULL;
    sdp_list_t* next = NULL;

    if (sdp_service_search_attr_req(session, search,
                                    SDP_ATTR_REQ_RANGE, attrid, &seq))
    {
        kdWarning() << "Service Search failed" << endl;
        sdp_close(session);
        return;
    }
    sdp_list_free(attrid, 0);
    sdp_list_free(search, 0);

    for (; seq; seq = next)
    {
        sdp_record_t *rec = (sdp_record_t *) seq->data;

        sdp_list_t* attrlist = rec->attrlist;
        vector<Attribute> alist;
        Service service;
        for (; attrlist; attrlist = attrlist->next)
        {
            int attrID = ((sdp_data_t*)(attrlist->data))->attrId;
            Attribute attr =
                getAttribute((sdp_data_t*)(attrlist->data));
            service.addAttribute(attrID, attr);
        }
        kdDebug() << "Added service" << endl;
        serviceList.push_back(service);

        next = seq->next;
        free(seq);
        sdp_record_free(rec);
    }
    sdp_close(session);
}

Attribute Device::getAttribute(sdp_data_t* attrib)
{
    //int attrID = attrib->attrId;
    int dataLen = (attrib->dtd & 0x7);
    int dataType = (attrib->dtd >> 3);

    int attrByteSize = 0;
    bool valid = true;
    Attribute newAttr;
    switch (dataType)
    {
    case 0: // Nil type
        {
            newAttr.setNil();
            break;
        }
    case 1: // Unsigned integer
        {
            unsigned long long uintVal = 0;
            switch (dataLen)
            {
            case 0: // 1 Byte
                attrByteSize = 1;
                uintVal = attrib->val.uint8;
                break;
            case 1: // 2 Byte
                attrByteSize = 2;
                uintVal = attrib->val.uint16;
                break;
            case 2: // 4 Byte
                attrByteSize = 4;
                uintVal = attrib->val.uint32;
                break;
            case 3: // 8 Byte
                attrByteSize = 8;
                uintVal = attrib->val.uint64;
                break;
            case 4: // 16 Byte
                attrByteSize = 16;
                //uintVal = attrib->val.uint128;
                assert(false); // BUG/TODO: uint128 integers not supported
                break;
            default:
                valid = false;
            }
            newAttr.setUInt(attrByteSize, uintVal);
            break;
        }
    case 2: // Signed integers
        {
            long long intVal = 0;
            switch (dataLen)
            {
            case 0: // 1 Byte
                attrByteSize = 1;
                intVal = attrib->val.int8;
                break;
            case 1: // 2 Byte
                attrByteSize = 2;
                intVal = attrib->val.int16;
                break;
            case 2: // 4 Byte
                attrByteSize = 4;
                intVal = attrib->val.int32;
                break;
            case 3: // 8 Byte
                attrByteSize = 8;
                intVal = attrib->val.int64;
                break;
            case 4: // 16 Byte
                attrByteSize = 16;
                //intVal = attrib->val.int128;
                assert(false); // BUG/TODO: uint128 types not supported
                break;
            default:
                valid = false;
            }
            newAttr.setInt(attrByteSize, intVal);
            break;
        }
    case 3: // UUIDs
        {
            ::uuid_t uuidVal = attrib->val.uuid;
            SDP::uuid_t id;
            switch (dataLen)
            {
            case 1: // 2 Byte
                id.setUUID32(uuidVal.value.uuid16);
                break;
            case 2: // 4 Byte
                id.setUUID32(uuidVal.value.uuid32);
                break;
            case 4: // 16 Byte
                uint64_t* v128;
                v128 = reinterpret_cast<uint64_t*>(&(uuidVal.value.uuid128));
                id.setUUID128(v128[0], v128[1]);
                break;
            default:
                valid = false;
            }

            //kdDebug() << "ID::" << uuidVal.value.uuid128.data[0] << endl;
            //kdDebug() << "ID::::" << (long int)id->hi << (long int)id->lo << endl;
            newAttr.setUUID(attrByteSize, id);
            break;
        }
    case 4:
    case 8: // Text string / URL
        {
            // todo: Use the size information and don't depend on
            // the null-termination of the string
            QCString text(attrib->val.str);
            if (dataType == 4)
            {
                newAttr.setString(text);
            }
            else
            {
                newAttr.setURL(text);
            }
            break;
        }
    case 5: // boolean value
        {
            // todo: shall we ignore this attribute if size!=0?
            bool boolVal = (attrib->val.int8 != 0);
            newAttr.setBool(boolVal);
            break;
        }
    case 6:
    case 7: // Sequence / Alternative
        {
            vector<Attribute> subAttribs;
            sdp_data_t* subAttrib = attrib->val.dataseq;
            for (; subAttrib; subAttrib = subAttrib->next)
            {
                subAttribs.push_back(getAttribute(subAttrib));
            }
            if (dataType == 6)
            {
                newAttr.setSequence(subAttribs);
            }
            else
            {
                newAttr.setAlternative(subAttribs);
            }
            break;
        }
    } // end case
    if (valid)
    {
        return newAttr;
    }
    else
    {
        return Attribute();
    }
}

bool Device::haveServiceClassID(SDP::uuid_t uuid) {

    ServiceVector::iterator sIt;
    for(sIt = serviceList.begin(); sIt != serviceList.end(); ++sIt) {
        if(sIt->haveServiceClassID(uuid))
		    return true;
	}
    return false;
}

/** Get a vector of Rfcomm channels of the services having "uuid" in the class ID List*/
std::vector<int> Device::getRfcommList(SDP::uuid_t uuid) {

    std::vector<int> rfcommList;
    unsigned int channel;

	ServiceVector::iterator sIt;
	for(sIt = serviceList.begin(); sIt != serviceList.end(); ++sIt) {
        if(sIt->haveServiceClassID(uuid))
		    if(sIt->getRfcommChannel(channel))
			    rfcommList.push_back(channel);
	}
	return rfcommList;
}


}
}
