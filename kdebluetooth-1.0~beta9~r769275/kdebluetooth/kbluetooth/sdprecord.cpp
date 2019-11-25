//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <stdlib.h>

#include "sdprecord.h"

#include <qdom.h>
#include <kdebug.h>
#include <bluetooth/sdp_lib.h>
#include <kmessagebox.h>
#include <klocale.h>

namespace Bluetooth
{

int64_t toi64(QString s)
{
    if (s.isNull())
        return 0;
    return ::strtoll(s.ascii(), NULL, 0);
}

uint64_t tou64(QString s)
{
    if (s.isNull())
        return 0;
    return ::strtoll(s.ascii(), NULL, 0);
}

SdpRecord::SdpRecord()
{
    m_sdpSession = NULL;
}

void SdpRecord::fromXmlDocument(const QDomDocument& xmlData,
                                const QMap<QString, QString>& valrefMap)
{
    m_valrefMap = valrefMap;
    fromXmlDocument(xmlData);
}

void SdpRecord::fromXmlDocument(const QDomDocument& xmlData)
{
    clear();
    parseDocument(xmlData.documentElement());
}

void SdpRecord::parseDocument(const QDomElement &e)
{
    kdDebug() << "<-" << e.tagName().ascii() << "->" << endl;
    QDomNode n;
    //clear();
    for (n = e.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement child = n.toElement();
        if (!child.isNull())
        {
            if (child.tagName() == "attributes")
            {
                parseAttributes(child);
            }
            else
            {
                kdDebug() << "attributes-tag expected" << endl;
            }
        }
    }
    kdDebug() << "</" << e.tagName().ascii() << ">"<< endl;
}

void SdpRecord::clear()
{
    m_sdpRecord.pattern = NULL;
    m_sdpRecord.attrlist = NULL;

    m_sdpSession = NULL;
    memset((void *)&m_sdpRecord, 0, sizeof(sdp_record_t));
    m_sdpRecord.handle = 0xffffffff;
}

void SdpRecord::parseAttributes(const QDomElement &e)
{
    kdDebug() << "@attributes" << endl;
    QDomNode n;

    for (n = e.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement child = n.toElement();
        if (!child.isNull())
        {
            uint16_t attrID = 0;
            sdp_data_t* attr = parseAttribute(child, &attrID);
            if (attr)
            {
                kdDebug() << "added attribute!" << endl;
                sdp_attr_add(&m_sdpRecord, attrID, attr);
            }
            else
            {
                kdDebug() << "Failed to parse " << child.tagName().ascii() << endl;
            }
        }
    }
}

sdp_data_t* SdpRecord::parseAttribute(const QDomElement& e, uint16_t* id)
{
    sdp_data_t* ret = NULL;
    QString sAttrID = e.attribute("id");
    if ((!sAttrID.isNull()) && (e.tagName() == "attribute"))
    {
        *id = (uint16_t)tou64(sAttrID);
        if (e.firstChild().isElement())
        {
            kdDebug() << "attribute id=" << *id << endl;
            ret = parseData(e.firstChild().toElement());
        }
        else
        {
            kdDebug() << "Data element expected" << endl;
        }
    }
    else
    {
        kdDebug() << "attribute+id exptected. " << e.tagName().ascii()
        << " id=" << sAttrID.ascii() << endl;
    }
    return ret;
}

sdp_data_t* SdpRecord::parseData(const QDomElement& e)
{
    sdp_data_t* attr = NULL;

    QString tag = e.tagName();
    QString sizeStr = e.attribute("size", QString());
    QString val = e.attribute("val");
    QString valref = e.attribute("valref", QString());
    if (!valref.isNull())
    {
        if (m_valrefMap.find(valref) != m_valrefMap.end())
        {
            val = m_valrefMap[valref];
            kdDebug() << QString("SdpRecord: set %1 to %2.")
                .arg(valref).arg(val) << endl;
        }
        else
        {
            kdDebug() << "Undefinied valref (" << valref.ascii() << ")" << endl;
            return NULL;
        }
    }
    int valSize;
    if (sizeStr.isNull())
    {
        int charlen = val.length();
        if (val.lower().startsWith("0x"))
            charlen -= 2;
        if (charlen <= 2)
            valSize = 8;
        else if (charlen <= 4)
            valSize = 16;
        else if (charlen <= 8)
            valSize = 32;
        else if (charlen <= 16)
            valSize = 64;
        else
            valSize = 128;
    }
    else
    {
        valSize = sizeStr.toInt();
    }
    QString text = e.text();
    kdDebug() << "attribute=" << e.tagName().ascii() << " size=" << valSize << endl;

    if (tag == "uint")
    {
        if (valSize == 8)
        {
            uint8_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UINT8, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 16)
        {
            uint16_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UINT16, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 32)
        {
            uint32_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UINT32, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 64)
        {
            uint64_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UINT64, &v);
            kdDebug() << "val=" << (int)v << endl;
        }
        else
        {
            kdDebug() << "Unsupported uint size: " << valSize << endl;
        }
    }
    else if (tag == "int")
    {
        if (valSize == 8)
        {
            int8_t v = tou64(val);
            attr = sdp_data_alloc(SDP_INT8, &v);
            kdDebug() << "val=" <<  v << endl;
        }
        else if (valSize == 16)
        {
            int16_t v = tou64(val);
            attr = sdp_data_alloc(SDP_INT16, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 32)
        {
            int32_t v = tou64(val);
            attr = sdp_data_alloc(SDP_INT32, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 64)
        {
            int64_t v = tou64(val);
            attr = sdp_data_alloc(SDP_INT64, &v);
            kdDebug() << "val=" << (int)v << endl;
        }
        else
        {
            kdDebug() << "Unsupported int size: " << valSize << endl;
        }
    }
    else if (tag == "uuid")
    {
        if (valSize == 16)
        {
            uint16_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UUID16, &v);
            kdDebug() << "val=" << v << endl;
        }
        else if (valSize == 32)
        {
            uint32_t v = tou64(val);
            attr = sdp_data_alloc(SDP_UUID32, &v);
            kdDebug() << "val=" << v << endl;
        }
        else
        {
            kdDebug() << "Unsupported uuid size: " << valSize << endl;
        }
    }
    else if ((tag == "seq") || (tag == "alt"))
    {
        sdp_data_t* seq = NULL;
        int numElements = 0;
        for (QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            QDomElement child = n.toElement();
            if (!child.isNull())
            {
                sdp_data_t* childAttr = parseData(child);
                if (childAttr)
                {
                    seq = sdp_seq_append(seq, childAttr);
                    ++numElements;
                }
            }
        }
        if (numElements >= 256*256 || valSize == 32)
        {
            if (tag == "seq")
                attr = sdp_data_alloc(SDP_SEQ32, seq);
            else
                sdp_data_alloc(SDP_ALT32, seq);
        }
        else if (numElements > 256 || valSize == 16)
        {
            if (tag == "seq")
                attr = sdp_data_alloc(SDP_SEQ16, seq);
            else
                sdp_data_alloc(SDP_ALT16, seq);
        }
        else
        {
            if (tag == "seq")
                attr = sdp_data_alloc(SDP_SEQ8, seq);
            else
                sdp_data_alloc(SDP_ALT8, seq);
        }
    }
    else if ((tag == "str") || (tag == "url"))
    {
        kdDebug() << "string: " << text.ascii() << endl;
        QCString* s = new QCString(text.utf8());
        if (s->size() >= 256*256 || valSize == 32)
        {
            if (tag == "str")
            {
                attr = sdp_data_alloc(SDP_TEXT_STR32, (const char*)(*s));
            }
            else
            {
                attr = sdp_data_alloc(SDP_URL_STR32, (const char*)(*s));
            }
        }
        else if (s->size() >= 256 || valSize == 16)
        {
            if (tag == "str")
            {
                attr = sdp_data_alloc(SDP_TEXT_STR16, (const char*)(*s));
            }
            else
            {
                attr = sdp_data_alloc(SDP_URL_STR16, (const char*)(*s));
            }
        }
        else
        {
            if (tag == "str")
            {
                attr = sdp_data_alloc(SDP_TEXT_STR8, (const char*)(*s));
            }
            else
            {
                attr = sdp_data_alloc(SDP_URL_STR8, (const char*)(*s));
            }
        }
    }
    return attr;
}


bool SdpRecord::sdpRegister()
{
    kdDebug() << "registering..." << endl;
    sdpUnregister();
    bdaddr_t bdaddr_any = {{0,0,0,0,0,0}};
    bdaddr_t bdaddr_local = {{0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF}};
    m_sdpSession = sdp_connect(&bdaddr_any, &bdaddr_local, 0);
    if (m_sdpSession > 0) {
        int e;
        if ((e=sdp_record_register(m_sdpSession, &m_sdpRecord, 0/*SDP_RECORD_PERSIST*/)) < 0)
        {
            KMessageBox::detailedError(NULL, i18n("Failed to register an SDP record."),
                i18n("Please make sure that sdpd is running; \n\
without it, other devices will not be able to find out \n\
which services your computer offers. \n\
The error code returned by sdp_record_register() was %1").arg(e),
                    i18n("KDE Bluetooth Framework"));
            kdError() << "Error: sdpRegister() failed (" << e << ")" << endl;
            return false;
        }
        kdDebug() << "done." << endl;
        return true;
    }
    else {
        KMessageBox::detailedError(NULL, i18n("Failed to connect to the SDP server."),
            i18n("Please make sure that sdpd is running; \n\
without it, other devices will not be able to find out \n\
which services your computer offers."),
            i18n("KDE Bluetooth Framework"));
        kdError() << "Error: sdp_connect() failed" << endl;
        return false;
    }
}

void SdpRecord::sdpUnregister()
{
    if (m_sdpSession)
    {
        kdDebug() << "unregistering." << endl;
        sdp_close(m_sdpSession);
    }
    m_sdpSession = NULL;
}

SdpRecord::~SdpRecord()
{
    sdpUnregister();
}

}
