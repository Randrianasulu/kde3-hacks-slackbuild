/*
 *
 *  XML Service Record parser of BlueZ D-Bus API
 *
 *  Copyright (C) 2006  Daniel Gollub <dgollub@suse.de>
 *
 *
 *  This file is part of libkbluetooth.
 *
 *  libkbluetooth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libkbluetooth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libkbluetooth; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "servicerecord.h"
#include <kdebug.h>

namespace KBluetooth
{

ServiceRecord::ServiceRecord(const QString &xml) : mXml(xml)
{
	QDomDocument doc;

  	doc.setContent( xml );

	QDomElement docElement = doc.documentElement();

	for(QDomNode n = docElement.firstChild(); !n.isNull(); n = n.nextSibling() ) {
   		QDomElement e = n.toElement();

		kdDebug() << e.text() << endl;
		
		if ( e.attribute("id") == "0x0001" ) {
			mUuid = parseAttributeUuid(e);
		} else if ( e.attribute("id") == "0x0009" ) {
			mServId = parseAttributeUuid(e);
		} else if ( e.attribute("id") == "0x0004" ) {
			mRfcommChannel = parseAttributeRfcomm(e);
		} else if ( e.attribute("id") == "0x0100" ) {
			mName = parseAttributeName(e);
		} else if ( e.attribute("id") == "0x0101" ) {
			if (mTyp == "")
				mTyp = parseAttributeName(e);
		}

	}
}

ServiceRecord::~ServiceRecord()
{
}

QString ServiceRecord::parseAttributeName(const QDomElement &e)
{	
	kdDebug() << k_funcinfo << mXml << endl;
	QDomNodeList list = e.elementsByTagName("text");
	QString serviceName = list.item(0).toElement().attribute("value");
	if (list.item(0).toElement().attribute("encoding") == "hex") {
		QString localValue = list.item(0).toElement().attribute("value");
		QString a = localValue.mid(0, 2);
		bool ok;

		serviceName.insert(0, (char) a.toInt(&ok, 16));
		for (uint i=2; i < localValue.length(); i = i + 2) {
			a = localValue.mid(i, 2);
			serviceName.insert(i/2, (char) a.toInt(&ok, 16));
		}
		serviceName.setLength(localValue.length()/2 - 1);
	}

	kdDebug() << "serviceName: " << serviceName << endl;
	return serviceName;
}

QStringList ServiceRecord::parseAttributeUuid(const QDomElement &e)
{
	QStringList uuid;

	QDomNodeList list = e.elementsByTagName("sequence").item(0).toElement().elementsByTagName("uuid");
	for (uint i=0;i <list.count(); ++i) {
		QString id = list.item(i).toElement().attribute("value");
		uuid.append( id );
	}

	return uuid;
}

int ServiceRecord::parseAttributeRfcomm(const QDomElement &e)
{
	int rfcommChannelValue = -1;

	QDomNodeList list = e.elementsByTagName("sequence");
	for (uint i=0;i <list.count(); ++i) {
		QDomNode m;
		for( m = list.item(i).firstChild(); !m.isNull(); m = m.nextSibling() ) {
			QDomNodeList list = e.elementsByTagName("uuid");
			bool isRfcomm = false;
			for (uint i=0;i <list.count(); ++i) {
				if (list.item(i).toElement().tagName() == "uuid"
						&& list.item(i).toElement().attribute("value") == "0x0003")
					isRfcomm = true;
			}

			if (!isRfcomm)
				break;

			// get RFCOMM Channel
			list = e.elementsByTagName("uint8");
			for (uint i=0;i <list.count(); ++i) {
				rfcommChannelValue = list.item(i).toElement().attribute("value").toInt(0, 16);
			}
		}
	}

	return rfcommChannelValue;
}


}
