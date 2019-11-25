/*
 *
 *  DBus Filter
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

#include "dbusfilter.h"

namespace KBluetooth {

DBusFilter::DBusFilter(DBusConnection *dbusconn) : conn(dbusconn)
{
}

DBusFilter::~DBusFilter() {
}

bool DBusFilter::addFilter(DBusHandleMessageFunction filter) {

	DBusHandleMessageFunction filterFunction = filter;

        DBusError error;
        dbus_error_init(&error);

        if (!dbus_connection_add_filter(conn, filterFunction, NULL, NULL)) {
                return false;
        }

	return true;
}

void DBusFilter::removeFilter(DBusHandleMessageFunction filter) {

	DBusHandleMessageFunction filterFunction = filter;

        DBusError error;
        dbus_error_init(&error);

        dbus_connection_remove_filter(conn, filterFunction, NULL);
}

bool DBusFilter::addMatch(const QString &match_rule) {	

	DBusError error;
	dbus_error_init(&error);

        dbus_bus_add_match(conn, match_rule.latin1(), &error);
                       
        if (dbus_error_is_set(&error)) { 
                kdDebug() << "Adding match failed: " << error.name << ": " << error.message << endl;
                dbus_error_free(&error);
                return false;
        }

	return true;
}

bool DBusFilter::removeMatch(const QString &match_rule) {	

	DBusError error;
	dbus_error_init(&error);

        dbus_bus_remove_match(conn, match_rule.latin1(), &error);
                       
        if (dbus_error_is_set(&error)) { 
                kdDebug() << "Removing match failed: " << error.name << ": " << error.message << endl;
                dbus_error_free(&error);
                return false;
        }

	return true;
}

}

