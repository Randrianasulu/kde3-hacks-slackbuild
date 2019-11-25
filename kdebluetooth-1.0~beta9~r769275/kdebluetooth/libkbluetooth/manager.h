/*
 *
 *  Manager implementation of BlueZ D-Bus API
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


#ifndef MANAGER_H
#define MANAGER_H

#include <unistd.h> // usleep workaround

#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_MANAGER "org.bluez.Manager"

namespace KBluetooth
{

class Manager : public DBusSignal
{

	Q_OBJECT
	public:
		Manager(DBusConnection *dbusconn);
		~Manager();

		typedef QValueList<QString> ServiceList;
		typedef QValueList<QString> AdapterList;


		unsigned int interfaceVersion();
		QString defaultAdapter();
		QString findAdapter(const QString &pattern);
		AdapterList listAdapters();
		QString findService(const QString &pattern);
		ServiceList listServices();
		QString activateService(const QString &pattern);

	signals:	
		void adapterAdded(const QString &path);
		void adapterRemoved(const QString &path);
		void defaultAdapterChanged(const QString &path);
		void serviceAdded(const QString &path);
		void serviceRemoved(const QString &path);
		void cleanup();

	private:	
		QString path;
		DBusConnection *conn;
		DBusFilter *filter;

		static Manager *_manager;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);
};
}

#endif // MANAGER_H 
