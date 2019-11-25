/*
 *
 *  Passkey Agent implementation of bluez DBus API
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


#ifndef PASSKEYAGENT_H
#define PASSKEYAGENT_H

#include "security.h"
#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_PASSKEYAGENT "org.bluez.PasskeyAgent"

namespace KBluetooth
{

class PasskeyAgent : public DBusSignal 
{

	Q_OBJECT
	public:
		PasskeyAgent(DBusConnection *dbusconn, const QString &agent_path);
		~PasskeyAgent();

		bool registerPasskeyAgent(const QString&);
		bool unregisterPasskeyAgent(const QString&);
		bool registerDefaultPasskeyAgent();
		bool unregisterDefaultPasskeyAgent();

		DBusHandlerResult pinRequest(DBusMessage *msg);
		DBusHandlerResult pinConfirm(DBusMessage *msg);
		QString getError();

	signals:
		void request(const QString &path, const QString &address, bool numeric);
		void confirm(const QString &path, const QString &address, const QString &value);
		void display(const QString &path, const QString &address, const QString &value);
		void keypress(const QString &path, const QString &address);
		void complete(const QString &path, const QString &address);
		void cancel(const QString &path, const QString &address);
		void release();
		void cleanup();

	public slots:
		void sendPasskey(const QString &passkey);	
		void sendConfirm(bool valid);

	private:	
		QString match;
		DBusFilter *filter;

		bool reRegisterDefaultAgent;

		static PasskeyAgent *_passkeyagent;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);

		QString path;

		static DBusConnection *conn;
		static DBusMessage *_msg;
};
}

#endif // PASSKEYAGENT_H 
