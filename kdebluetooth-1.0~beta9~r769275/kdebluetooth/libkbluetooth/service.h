/*
 *
 *  Security implementation of bluez DBus API
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


#ifndef SERVICE_H
#define SERVICE_H

#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_SERVICE "org.bluez.Service"

namespace KBluetooth
{

class Service : public DBusSignal
{

	Q_OBJECT
	public:
		Service(DBusConnection *dbusconn, const QString &servicepath);
		~Service();

		typedef QValueList<QString> UserList;

		QString getIdentifier();
		QString getName();
		QString getDescription();
		QString getBusName();
		bool start();
		bool stop();
		bool isRunning();
		bool isExternal();
		UserList listUsers();
		bool removeUser(const QString &address);
		bool setTrusted(const QString &address);
		bool isTrusted(const QString &address);
		bool removeTrust(const QString &address);
		QStringList listTrusts();

	signals:	
		void stopped();
		void started();
		void trustAdded(const QString &address);
		void trustRemoved(const QString &address);

	private:	
		QString path;
		QString match;
		DBusConnection *conn;
		DBusFilter *filter;

		static Service *_service;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);
};
}

#endif // SERVICE_H 
