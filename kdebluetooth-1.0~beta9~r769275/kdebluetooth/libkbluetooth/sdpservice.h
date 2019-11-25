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

#ifndef SDPSERVICE_H
#define SDPSERVICE_H

#include <vector>
#include <map>
#include "sdpattribute.h"

namespace KBluetooth
{
namespace SDP
{

/**
@author Fred Schaettgen
*/
class Service
{
    friend class Device;
private:
    struct AttributeEntry
    {
        int id;
        Attribute attr;
    };
    std::vector<AttributeEntry> attributeList;
    std::vector<int> languageBases;
    std::map<int,int> languageBaseToMibEnum;
protected:
    Service();
public:
    void addAttribute(int id, const Attribute& attr);
    const Attribute& getAttribute(int index);
    int getAttributeID(int index);
    std::vector<SDP::Attribute> getSubAttributes(SDP::Attribute attr);

    bool getAttributeByID(int id, Attribute& attrib);
    bool getI18nAttributeByID(int id, Attribute& attrib, int &languageBase);
    QString decodeI18nString(int languageBase, const QCString &cstring);

    // See Bluetooth Core Spec Sec. 5.1
    bool getServiceRecordHandle(uint32_t *handle); // 0x00
    //bool getServiceClassIDList(vector<uuid_t> *classIDList); // 0x01
    //bool getServiceRecordState(uint32_t *state); // 0x02
    //bool getServiceID(uuid_t *id); // 0x03;
    //bool getProtocolDescriptorList(SDP::Attribute *attribute); // 0x04
    //bool getBrowseGroupList(vector<uuid_t> *browseGroupList); // 0x05
    bool getServiceName(QString &name); // langBase + 0x00
    bool getServiceDescription(QString &desc); // langBase + 0x01
    std::vector<SDP::uuid_t> getAllUUIDs();



    /** Retrieves the Rfcomm channel.
    This function is provided just for conveniance.
    @param n The Rfcomm channel.
    @ret true if an rfcomm channel was found, false otherwise. */
    bool getRfcommChannel(unsigned int &n);
    SDP::UUIDVec getClassIdList();

	bool haveServiceClassID(SDP::uuid_t uuid);

    virtual ~Service();
};

}
}

#endif
