/*
 *
 *  DBus Interface for libkbluetooth 
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

#include "dbusinit.h"

namespace KBluetooth {

DBusInit::DBusInit() : QObject()
{	

	DBusError error;
	dbus_error_init(&error);
	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

	if (!conn) {
		kdDebug() << "dbus_bus_get() failed...." << endl;
		if (dbus_error_is_set(&error)) {
			kdDebug() << error.message << endl;
			dbus_error_free(&error);
			return;
		}
	}

	_dbusConnection = new DBusQt::Connection(this);
	if (!_dbusConnection) {
		kdDebug() << __func__ << " DBusQt::Connection() failed." << endl;
		return;
	}

	_dbusConnection->dbus_connection_setup_with_qt_main(conn);


}

DBusInit::~DBusInit()
{

	_dbusConnection->close();
	delete _dbusConnection;

//	dbus_connection_unref(conn);
////	dbus_connection_close(conn);
}

DBusConnection *DBusInit::getDBus()
{
	return conn;
}

}

#include "dbusinit.moc"
