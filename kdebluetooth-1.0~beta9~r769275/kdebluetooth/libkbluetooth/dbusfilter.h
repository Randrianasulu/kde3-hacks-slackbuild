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

#ifndef DBUSFILTER_H
#define DBUSFILTER_H

#include <kdebug.h>
#include <qstring.h>
#include <dbus/dbus.h>

namespace KBluetooth {
class DBusFilter {

	public:
		DBusFilter(DBusConnection *conn);
		~DBusFilter();

		bool addFilter(DBusHandleMessageFunction filter);
		void removeFilter(DBusHandleMessageFunction filter);

		bool addMatch(const QString &match_rule);
		bool removeMatch(const QString &match_rule);

	private:
		DBusConnection *conn;

};
}

#endif // DBUSFILTER_H

