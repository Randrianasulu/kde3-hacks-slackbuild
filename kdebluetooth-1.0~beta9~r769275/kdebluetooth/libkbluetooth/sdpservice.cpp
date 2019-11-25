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

#include "sdpservice.h"
#include <qstringlist.h>
#include <qtextcodec.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

using namespace std;

namespace KBluetooth
{
namespace SDP
{

Service::Service()
{
}

Service::~Service()
{}

void Service::addAttribute(int id, const Attribute& attr)
{
    AttributeEntry entry;
    entry.id = id;
    entry.attr = attr;
    attributeList.push_back(entry);
    
    // if a LanguageBaseIDList attribute is added, we
    // use it to set the preferrred language bases according 
    // to the current locale
    if (id == 0x6 /*LanguageBaseAttributeIDList*/) {
        QStringList preferredLanguages = KGlobal::locale()->languagesTwoAlpha();
        AttrVec vec = attr.getSequence();
        languageBases.clear();
        languageBaseToMibEnum.clear();
        for (uint langNum = 0; langNum < preferredLanguages.size(); ++langNum) {
            bool foundLanguage = false;
            for (uint n=0; n<vec.size()/3; ++n) {
                uint16_t intISO = uint16_t(vec[n*3+0].getUInt().lo);
                uint16_t intEnc = uint16_t(vec[n*3+1].getUInt().lo);
                uint16_t intOff = uint16_t(vec[n*3+2].getUInt().lo);
                QString iso;
                iso += char(intISO >> 8);
                iso += char(intISO & 0xFF);
                kdDebug() << "Found language base attribute with ISO code '"<< iso << "'" << endl;
                if (iso == preferredLanguages[langNum]) {
                    kdDebug() << "Appending preferred language base " << intOff << endl;
                    languageBases.push_back(intOff);
                    languageBaseToMibEnum[intOff] = intEnc;
                }
            }
        }
    }
}

const Attribute& Service::getAttribute(int index)
{
    return attributeList[index].attr;
}

int Service::getAttributeID(int index)
{
    return attributeList[index].id;
}

bool Service::getAttributeByID(int id, Attribute& attrib)
{
    for (unsigned int n=0; n<attributeList.size(); ++n)
    {
        if (attributeList[n].id == id)
        {
            attrib = attributeList[n].attr;
            return true;
        }
    }
    return false;
}

bool Service::getI18nAttributeByID(int id, Attribute& attrib, int &languageBase)
{
    for (vector<int>::iterator it = languageBases.begin(); 
        it != languageBases.end(); ++it)
    {
        languageBase = *it;
        if (getAttributeByID(id + languageBase, attrib)) {
            kdDebug() << "Using preferred language base " << *it << endl;
            return true;
        }
    }
    kdDebug() << "Using default language base" << endl;
    languageBase = 0x100;
    return getAttributeByID(id + languageBase, attrib);
}

QString Service::decodeI18nString(int languageBase, const QCString &cstring)
{
    QTextCodec *codec = NULL;
    if (languageBaseToMibEnum.find(languageBase) != languageBaseToMibEnum.end()) {
        codec = QTextCodec::codecForMib(languageBaseToMibEnum[languageBase]);
    }
    if (codec == NULL) {
        codec = QTextCodec::codecForName("utf-8");
    }
    kdDebug() << "Decode string with " << codec->name() << " codec" << endl;
    return codec->toUnicode(cstring);
}

bool Service::getServiceRecordHandle(uint32_t *handle)
{
    Attribute attrib;
    bool ret = getAttributeByID(0x00, attrib);
    if (ret == true)
    {
        if (attrib.getType() == Attribute::UINT)
        {
            *handle = (uint32_t)attrib.getUInt().lo;
            return true;
        }
    }
    return false;
}

bool Service::getServiceName(QString &name)
{
    Attribute attrib;
    int languageBase;
    bool ret = getI18nAttributeByID(0x0, attrib, languageBase);
    if (ret == true)
    {
        if (attrib.getType() == Attribute::STRING)
        {
            QCString cname = attrib.getString();
            name = decodeI18nString(languageBase, cname);
            return true;
        }
    }
    return false;
}

bool Service::getServiceDescription(QString &desc)
{
    Attribute attrib;
    int languageBase;
    bool ret = getI18nAttributeByID(0x1, attrib, languageBase);
    if (ret == true)
    {
        if (attrib.getType() == Attribute::STRING)
        {
            QCString cdesc = attrib.getString();
            desc = decodeI18nString(languageBase, cdesc);
            return true;
        }
    }
    return false;
}

vector<SDP::uuid_t> Service::getAllUUIDs()
{
    vector<SDP::uuid_t> uuidList;
    vector<AttributeEntry>::iterator it;
    for (it = attributeList.begin();
            it != attributeList.end(); ++it)
    {
        vector<SDP::uuid_t> subList = it->attr.getAllUUIDs();
        copy(subList.begin(), subList.end(), back_inserter(uuidList));
    }
    return uuidList;
}

bool Service::getRfcommChannel(unsigned int &n)
{
    // Get the rfcomm channel
    Attribute protoDescAttr;
    // Get the the protocol descriptor list attribute (0x04)
    if (getAttributeByID(0x04 , protoDescAttr) == false) {
        return false;
    }

    std::vector<SDP::Attribute> protoDescList = protoDescAttr.getSequence();
    std::vector<SDP::Attribute>::iterator it;

	//Search in all subLists of the Protocol Description List.
	for(it = protoDescList.begin(); it != protoDescList.end(); ++it) {
        std::vector<SDP::Attribute> attrList = it->getSequence();
        std::vector<SDP::Attribute>::iterator it;

		//The List must have at least 2 Attributes
		//Example:
		//  UUID16 : 0x0003 - RFCOMM
        //  Channel/Port (Integer) : 0x6

		if(attrList.size() >= 2) {
		    it = attrList.begin();

			// The first Attribute of the list must be an UUID
			if(it->getType() != Attribute::UUID)
                continue;
            // The UUID must have the value of "0x0003" that's the RFCOMM UUID
			SDP::uuid_t rfcommUUID;
			rfcommUUID.fromString("0x0003");
			if(it->getUUID() != rfcommUUID) //RFCOMM UUID
			    continue;
			++it;
			//If the UUID is ok we get the rfcomm channel number
            if(it->getType() != Attribute::UINT)
			    continue;
			n = it->getUInt().lo;
			return true;
		}
	}
	// If we're here, we haven't found a correct Rfcomm channel, so we return false
    return false;
}

bool Service::haveServiceClassID(SDP::uuid_t uuid) {

    Attribute ClassIDAttr;
    // Get the the ClassID descriptor list attribute (0x01)
    if (getAttributeByID(0x01 , ClassIDAttr) == false)
        return false;

	AttrVec ClassIDList = ClassIDAttr.getSequence();
    AttrVec::iterator it;
    for(it = ClassIDList.begin(); it != ClassIDList.end(); ++it) {
        if(it->getType() != Attribute::UUID)
            continue;
        if(it->getUUID() == uuid)
            return true;
    }
        return false;
}

/** Get a vector of UUID of the services class Id List */
SDP::UUIDVec Service::getClassIdList() {

    SDP::UUIDVec uuidList;

	Attribute ClassIDAttr;
    // Get the the ClassID descriptor list attribute (0x01)
    if (getAttributeByID(0x01 , ClassIDAttr) == false)
        return uuidList;

	AttrVec ClassIDList = ClassIDAttr.getSequence();
    AttrVec::iterator it;
    for(it = ClassIDList.begin(); it != ClassIDList.end(); ++it) {
        if(it->getType() != Attribute::UUID)
            continue;
        uuidList.push_back(it->getUUID());
    }
    return uuidList;
}


}
}

