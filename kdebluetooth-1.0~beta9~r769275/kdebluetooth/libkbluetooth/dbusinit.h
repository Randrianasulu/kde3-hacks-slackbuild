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

#ifndef DBUSINIT_H_
#define DBUSINIT_H_

#include <kdebug.h>
#include <qstring.h>
#include <dbus/dbus.h>
#include <dbus/connection.h>

#define BLUEZ_SERVICE "org.bluez"
#define BLUEZ_PATH "/org/bluez"

#define OBEX_SERVICE "org.openobex"
#define OBEX_PATH "/org/openobex"


namespace KBluetooth {
class DBusInit : public QObject {

	Q_OBJECT

	public:
		DBusInit();
		~DBusInit();

		DBusConnection *getDBus();
	private:	

		DBusConnection *conn;
		DBusQt::Connection *_dbusConnection;
};
}

#endif // DBUSINIT_H_
