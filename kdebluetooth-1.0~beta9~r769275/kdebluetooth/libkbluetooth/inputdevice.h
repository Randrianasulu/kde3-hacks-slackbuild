/*
 *
 *  Input Device Service implementation of BlueZ D-Bus API
 *
 *  Copyright (C) 2007  Daniel Gollub <dgollub@suse.de>
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


#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_INPUTDEVICE "org.bluez.input.Device"

namespace KBluetooth
{

class InputDevice : public DBusSignal
{

	Q_OBJECT
	public:
		InputDevice(DBusConnection *dbusconn, const QString &busid, const QString &devicepath);
		~InputDevice();

		QString getName();
		QString getAddress();
		int getProductId();
		int getVendorId();
		bool isConnected();

	public slots:
		void connect();
		bool connectBlock();
		void disconnect();

	signals:	
		void connected();
		void disconnected();

	private:	
		QString path;
		QString match;
		DBusConnection *conn;
		DBusFilter *filter;

		static InputDevice *_inputdevice;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);
};
}

#endif // INPUTDEVICE_H 
