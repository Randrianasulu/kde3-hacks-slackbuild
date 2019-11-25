/*
 *
 *  DBus function wrapper for signals 
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

#ifndef DBUSSIGNAL_H
#define DBUSSIGNAL_H

#include "dbusinit.h"

namespace KBluetooth {
class DBusSignal : public QObject {

	public:
		DBusSignal(const QString &service, const QString &path, const QString &interface, DBusConnection *dbusconn);
		~DBusSignal();

		static bool serviceDown(DBusMessage *msg);
		static bool serviceUp(DBusMessage *msg);

		bool sendString(const QString &method, int first_type, ...);
		bool sendString(const QString &method);

		QString getString(const QString &method, int first_type, ...);
		QString getString(const QString &method);

		QValueList<QString> getStringList(const QString &method, int first_type, ...);
		QValueList<QString> getStringList(const QString &method);

		QValueList<Q_UINT8> getByteList(const QString &method, int first_type, ...);
		QValueList<Q_UINT8> getByteList(const QString &method);

		QValueList<Q_UINT32> getUInt32List(const QString &method, int first_type, ...);
		QValueList<Q_UINT32> getUInt32List(const QString &method);

		bool setString(const QString &method);
		bool setString(const QString &method, int first_type, ...); 

		int getInteger(const QString &method, int type, int first_type, ...);
		int getInteger(const QString &method, int type);

		bool getBoolean(const QString &method, int first_type, ...);
		bool getBoolean(const QString &method);

		QString getError() { return mError; };

	private:
		DBusMessage *newMessage(const QString &method);

		bool appendArgs(int first_type, va_list vargs);
		bool appendArgs(int first_type, ...);
		bool getArgs(DBusMessage *message, DBusError *error, int first_type, ...);

		bool send();
		DBusMessage *sendWithReply(DBusError *error, int timeout = -1);

		DBusMessage *msg;

		QString mError;

		QString mService;
		QString mPath;
		QString mInterface;
		DBusConnection *conn;
};

}

#endif // DBUSSIGNAL_H
