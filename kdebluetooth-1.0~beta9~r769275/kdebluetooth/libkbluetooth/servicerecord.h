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


#ifndef SERVICERECORD_H
#define SERVICERECORD_H

#include <kdebug.h>
#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>

namespace KBluetooth
{

class ServiceRecord
{
	public:
		ServiceRecord() {};
		ServiceRecord(const QString &xml);
		~ServiceRecord();

		int rfcommChannel() { return mRfcommChannel; };
		QString name() { return mName; };
		QStringList uuid() { return mUuid; };
		QStringList servId() { return mServId; };
		QString typ() { return mTyp; };

	private:	
		QString parseAttributeName(const QDomElement &e);
		int parseAttributeRfcomm(const QDomElement &e);
		QStringList parseAttributeUuid(const QDomElement &e);

		QString mXml;
		QString mName;
		QString mTyp;
		QStringList mUuid;
		QStringList mServId;
		int mRfcommChannel;
};
}

#endif // SERVICERECORD_H 
