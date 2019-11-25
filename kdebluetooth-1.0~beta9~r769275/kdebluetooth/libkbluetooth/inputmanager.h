/*
 *
 *  InputManager implementation of BlueZ D-Bus API
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


#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <unistd.h> // usleep workaround

#include "dbussignal.h"
#include "dbusfilter.h"

#define BLUEZ_PATH_INPUTMANAGER "/org/bluez/input"
#define INTERFACE_INPUTMANAGER "org.bluez.input.Manager"

namespace KBluetooth
{

class InputManager : public DBusSignal
{

	Q_OBJECT
	public:
		InputManager(DBusConnection *dbusconn, const QString &bus);
		~InputManager();

		typedef QValueList<QString> InputDeviceList;

		InputDeviceList listDevices();
		void createDevice(const QString &address);
		QString createDeviceBlock(const QString &address);
		bool removeDevice(const QString &address);

	signals:	
		void deviceCreated(const QString &path);
		void deviceRemoved(const QString &path);

		void cleanup();

	private:	
		QString path;
		QString match;
		DBusConnection *conn;
		DBusFilter *filter;

		static InputManager *_inputmanager;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);
};
}

#endif // INPUTMANAGER_H 
