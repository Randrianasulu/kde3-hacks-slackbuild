/*
 *
 *  KDE Blutooth Monitor 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluemon.
 *
 *  kbluemon is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluemon is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluemon; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef REMOTEDEVICE_H
#define REMOTEDEVICE_H
#include <qstring.h>

class RemoteDevice
{
public:
	RemoteDevice(const QString& ad,int rssi, QString& dev_class) : addr(ad), rssi(rssi), dev_class(dev_class) {}

	QString addr;
	QString name;
	int rssi;
	QString dev_class;
	QString major_class;
	QString version;
	QString revision;
	QString company;
	QString manufacturer;
	QString devInfo;
	typedef QValueVector<QString> Dev_Services;
	Dev_Services services;
	Dev_Services::iterator serv_it;


	
	void setName (QString& name) {name = name;}
	QString getAddr() {return addr;}
};

#endif
