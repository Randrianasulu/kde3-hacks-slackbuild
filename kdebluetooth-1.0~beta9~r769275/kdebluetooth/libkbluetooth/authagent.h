/*
 *
 *  Authorization Agent implementation of bluez DBus API
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


#ifndef AUTHAGENT_H
#define AUTHAGENT_H

#include "security.h"
#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_AUTHAGENT "org.bluez.AuthorizationAgent"

namespace KBluetooth
{

class AuthAgent : public  DBusSignal
{

	Q_OBJECT
	public:
		AuthAgent(DBusConnection *dbusconn, const QString &agent_path);
		~AuthAgent();


		bool registerDefaultAuthorizationAgent();
		bool unregisterDefaultAuthorizationAgent();

		void sendAuth(bool grant);

		QString getError();

	signals:
		void authorize(const QString &adapter_path, const QString &address, const QString &service, const QString &uuid);
		void cancel(const QString &adapter_path, const QString &address, const QString &service, const QString &uuid);
		void release();
		void cleanup();

	public slots:
		void setAlwaysTrust(const QString &address, const QString &service);

	private:	

		static AuthAgent *_authagent;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);

		bool reRegisterDefaultAgent;

		QString path;
		QString match;
		DBusFilter *filter;

		static DBusConnection *conn;
		static DBusMessage *_msg;
};
}

#endif // AUTHAGENT_H 
