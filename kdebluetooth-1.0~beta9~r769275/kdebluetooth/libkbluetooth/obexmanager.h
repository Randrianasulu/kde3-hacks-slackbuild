/*
 *
 *  Manager implementation of Obex-Data-Server D-Bus API
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
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


#ifndef OBEXMANAGER_H
#define OBEXMANAGER_H

#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_OBEXMANAGER "org.openobex.Manager"
namespace KBluetooth
{

class ObexManager : public DBusSignal
{

	Q_OBJECT
	
	public:
		ObexManager(DBusConnection *dbusconn);
		~ObexManager();

		void createBluetoothSession(const QString &address, const QString &pattern/*, bool auto_connect, char* session_object*/);
		void createBluetoothServer(const QString &source_address, const QString &pattern, char* server_object);

	signals:
		
		void sessionCreated(QString path);
		void sessionRemoved(QString path);

	private:
		DBusConnection *conn;
		DBusFilter *filter;

		static ObexManager *_manager;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);




};
}
#endif
