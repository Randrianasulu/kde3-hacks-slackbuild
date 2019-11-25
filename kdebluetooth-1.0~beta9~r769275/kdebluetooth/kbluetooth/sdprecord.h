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

#ifndef _SDP_RECORD_H_
#define _SDP_RECORD_H_

#include <list>
#include <map>
#include <qstring.h>
#include <qmap.h>
#include <qcstring.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

class QDomDocument;
class QDomElement;

namespace Bluetooth
{

class SdpRecord
{
public:
    SdpRecord();
    virtual ~SdpRecord();
    void fromXmlDocument(const QDomDocument& xmlData);
    void fromXmlDocument(const QDomDocument& xmlData,
                         const QMap<QString, QString>& valrefMap);
    bool sdpRegister();
    void sdpUnregister();

    class Error
    {
    public:
        QString msg;
    };

private:
    void parseDocument(const QDomElement &e);
    void parseAttributes(const QDomElement &e);
    sdp_data_t* parseAttribute(const QDomElement &e, uint16_t* id);
    sdp_data_t* parseData(const QDomElement &e);
    void clear();

    //std::list<QCString> m_stringList;
    //std::list<sdp_data_t> m_attrList;
    //std::list<sdp_list_t> m_listList;
    sdp_record_t m_sdpRecord;
    sdp_session_t* m_sdpSession;
    QMap<QString, QString>  m_valrefMap;
};

}


#endif
