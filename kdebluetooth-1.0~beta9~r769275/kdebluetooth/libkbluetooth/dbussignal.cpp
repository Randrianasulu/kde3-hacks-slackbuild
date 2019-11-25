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

#include "dbussignal.h"

namespace KBluetooth {

DBusSignal::DBusSignal(const QString &service, const QString &path, const QString &interface, DBusConnection *dbusconn) 
	: QObject(), mService(service), mPath(path), mInterface(interface), conn(dbusconn)
{
	kdDebug() << k_funcinfo << ": " << service << " path: " << path << " interface: " << interface << " conn: " << dbusconn << endl;
}

DBusSignal::~DBusSignal() {
}


bool DBusSignal::serviceDown(DBusMessage *msg)
{

	if (!dbus_message_is_signal (msg, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
		return false;

	const char *service, *old_owner, *new_owner;
	dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &service,
			DBUS_TYPE_STRING, &old_owner,
			DBUS_TYPE_STRING, &new_owner,
			DBUS_TYPE_INVALID);

	if (!strcmp(BLUEZ_SERVICE, service) && *new_owner == '\0') {
		kdDebug() << k_funcinfo << " SERVICE IS DOWN" << endl;
		return true;
	}

	return false;
}

bool DBusSignal::serviceUp(DBusMessage *msg)
{

	if (!dbus_message_is_signal (msg, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
		return false;

	const char *service, *old_owner, *new_owner;
	dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &service,
			DBUS_TYPE_STRING, &old_owner,
			DBUS_TYPE_STRING, &new_owner,
			DBUS_TYPE_INVALID);

	if (!strcmp(BLUEZ_SERVICE, service) && *old_owner == '\0' && *new_owner != '\0') {
		kdDebug() << k_funcinfo << " SERVICE IS UP" << endl;
		return true;
	}

	return false;
}

DBusMessage* DBusSignal::newMessage(const QString &method)
{
	kdDebug() << k_funcinfo << ": " << mService << " path: " << mPath << " mInterfacE: " << mInterface << " method: " << method << endl;
	DBusMessage *_msg = dbus_message_new_method_call(mService.ascii(), mPath.ascii(), mInterface.ascii(), method.ascii());

	if (!_msg) {
		kdDebug() << "libkbluetooth: Can't allocate new method call" << endl;
		return NULL;
	}

	return _msg;
}

bool DBusSignal::appendArgs(int first_type, va_list var_args)
{
	bool retval;
	retval = dbus_message_append_args_valist(msg, first_type, var_args);
	return retval;
}

bool DBusSignal::appendArgs(int first_type, ...)
{

	bool retval;
	va_list var_args;

	va_start(var_args, first_type);
	retval = appendArgs(first_type, var_args);
	va_end(var_args);

	return retval;
}

bool DBusSignal::getArgs(DBusMessage *message, DBusError *error, int first_type, ...) {
	bool retval;
	va_list var_args;

	va_start(var_args, first_type);
	retval = dbus_message_get_args_valist(message, error, first_type, var_args);
	va_end(var_args);

	return retval;

}

bool DBusSignal::send()
{
	DBusPendingCall *pcall = NULL;
	return dbus_connection_send_with_reply(conn, msg, &pcall, -1);
}

DBusMessage *DBusSignal::sendWithReply(DBusError *error, int timeout)
{
	return dbus_connection_send_with_reply_and_block(conn, msg, timeout, error);
}

bool DBusSignal::sendString(const QString &method)
{
	return sendString(method, DBUS_TYPE_INVALID);
}

bool DBusSignal::sendString(const QString &method, int first_type, ...)
{
	const char *string;
	DBusMessage *reply = NULL;
	va_list var_args;


	DBusError error;
	dbus_error_init(&error);

	msg = newMessage(method);

	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << __LINE__ << "appendArgs failed" << endl; 
	va_end(var_args);

	bool ret = send();

	kdDebug() << k_funcinfo << " " << ret << endl;

	return ret;
}

QString DBusSignal::getString(const QString &method)
{
	return getString(method, DBUS_TYPE_INVALID);
}

QString DBusSignal::getString(const QString &method, int first_type, ...)
{	
	const char *string;
	QString ret;
	DBusMessage *reply = NULL;
	va_list var_args;


	DBusError error;
	dbus_error_init(&error);

	msg = newMessage(method);

	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << __LINE__ << "appendArgs failed" << endl; 
	va_end(var_args);

	reply = sendWithReply(&error);
		
	if (!reply) {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << error.message << endl; 
			dbus_error_free(&error);
		}

		goto error;
	}

	getArgs(reply, &error, DBUS_TYPE_STRING, &string, DBUS_TYPE_INVALID); 
	if (dbus_error_is_set(&error)) {
		mError = QString(error.message);
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << error.message << endl; 
		dbus_error_free(&error);
		return ret;
	}

	ret = QString::fromUtf8(string);

	dbus_message_unref(reply);

error:	
	dbus_message_unref(msg);
	return ret;
}

QValueList<QString> DBusSignal::getStringList(const QString &method)
{
	return getStringList(method, DBUS_TYPE_INVALID);
}

QValueList<QString> DBusSignal::getStringList(const QString &method, int first_type, ...)
{
	QValueList<QString> list;

	int num_adapters = 0;
	char **adapters = NULL;

	DBusMessage *reply = NULL;
	va_list var_args;

	DBusError error;
	dbus_error_init(&error);

	msg = newMessage(method);

	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << __LINE__ << "appendArgs failed" << endl; 

	va_end(var_args);

	appendArgs(DBUS_TYPE_INVALID);

	reply = sendWithReply(&error);

	if (reply) {
		getArgs(reply, &error, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &adapters, &num_adapters, DBUS_TYPE_INVALID); 
	} else {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(): " << error.message << endl; 
			dbus_error_free(&error);
		}

		goto error;
	}

	for (int i = 0; i < num_adapters; i++)
		list.append(adapters[i]);

	dbus_free_string_array(adapters);
	dbus_message_unref(reply);

error:	
	dbus_message_unref(msg);
	return list;
}

QValueList<Q_UINT8> DBusSignal::getByteList(const QString &method)
{
	return getByteList(method, DBUS_TYPE_INVALID);
}

QValueList<Q_UINT8> DBusSignal::getByteList(const QString &method, int first_type, ...)
{
	QValueList<Q_UINT8> list;

	int num_records = 0;
	Q_INT8 *records = NULL;
	va_list var_args;

	DBusError error;
	DBusMessage *reply = NULL;

	dbus_error_init(&error);

	msg = newMessage(method);

	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << __LINE__ << "appendArgs failed" << endl; 

	va_end(var_args);

	appendArgs(DBUS_TYPE_INVALID);

	reply = sendWithReply(&error);

	if (reply) {
		getArgs(reply, &error, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &records, &num_records, DBUS_TYPE_INVALID); 
	} else {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(): " << error.message << endl; 
			dbus_error_free(&error);
		}
		goto error;
	}

	for (int i = 0; i < num_records; i++)
		list.append(records[i]);

	dbus_message_unref(reply);

error:	
	dbus_message_unref(msg);
	return list;
}

QValueList<Q_UINT32> DBusSignal::getUInt32List(const QString &method)
{
	return getUInt32List(method, DBUS_TYPE_INVALID);
}

QValueList<Q_UINT32> DBusSignal::getUInt32List(const QString &method, int first_type, ...)
{
	QValueList<Q_UINT32> list;

	int num_records = 0;
	Q_INT32 *records = NULL;
	va_list var_args;

	DBusError error;
	DBusMessage *reply = NULL;

	dbus_error_init(&error);

	msg = newMessage(method);

	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << __LINE__ << "appendArgs failed" << endl; 

	va_end(var_args);

	appendArgs(DBUS_TYPE_INVALID);

	reply = sendWithReply(&error);

	if (reply) {
		getArgs(reply, &error, DBUS_TYPE_ARRAY, DBUS_TYPE_UINT32, &records, &num_records, DBUS_TYPE_INVALID); 
	} else {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(): " << error.message << endl; 
			dbus_error_free(&error);
		}
		goto error;
	}

	kdDebug() << k_funcinfo << " Resullt: " << num_records << endl;

	for (int i = 0; i < num_records; i++)
		list.append(records[i]);

	dbus_message_unref(reply);

error:
	dbus_message_unref(msg);
	return list;
}

bool DBusSignal::setString(const QString &method)
{
	return setString(method, DBUS_TYPE_INVALID);
}

bool DBusSignal::setString(const QString &method, int first_type, ...)
{
	DBusMessage *reply = NULL;
	va_list var_args;

	DBusError error;
	dbus_error_init(&error);

	msg = newMessage(method);
	va_start(var_args, first_type);
	if (!appendArgs(first_type, var_args))
		kdDebug() << "libkbluetooth: " << __func__ << "(): " << __LINE__ << "appendArgs failed" << endl; 
	va_end(var_args);

	reply = sendWithReply(&error);
	if (!reply) {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << error.message << endl; 
			dbus_error_free(&error);
		}
		goto error;

	}

	dbus_message_unref(reply);
	dbus_message_unref(msg);

	return true; 

error:
	dbus_message_unref(msg);
	return false;
}

int DBusSignal::getInteger(const QString &method, int type)
{
	return getInteger(method, type, DBUS_TYPE_INVALID);
}

int DBusSignal::getInteger(const QString &method, int type, int first_type, ...)
{

	int ret = -1;
	DBusMessage *reply = NULL;
	DBusError error;
	dbus_error_init(&error);
	va_list var_args;

	msg = newMessage(method);

	va_start(var_args, first_type);
	appendArgs(first_type, var_args);
	va_end(var_args);

	reply = sendWithReply(&error);
	if (!reply) {
		if (dbus_error_is_set(&error)) {
			mError = QString(error.message);
			kdDebug() << "libkbluetooth: " << __func__ << "(" << method << "): " << error.message << endl; 
			dbus_error_free(&error);
		}
		goto error;
	}

	getArgs(reply, &error, type, &ret, DBUS_TYPE_INVALID);

	dbus_message_unref(reply);

error:
	dbus_message_unref(msg);
	return ret;
}

bool DBusSignal::getBoolean(const QString &method)
{
	return getBoolean(method, DBUS_TYPE_INVALID);
}

bool DBusSignal::getBoolean(const QString &method, int first_type, ...)
{

	dbus_bool_t ret = false;
	DBusMessage *reply = NULL;
	DBusError error;
	dbus_error_init(&error);
	va_list var_args;
	va_start(var_args, first_type);

	msg = newMessage(method);

	appendArgs(first_type, var_args);

	va_end(var_args);

	reply = sendWithReply(&error);
	if (!reply) {
		if (dbus_error_is_set(&error)) {
			kdDebug() << __func__ << " (" << method << "): " << error.message << endl;
			dbus_error_free(&error);

		}
		goto error;
	}

	getArgs(reply, &error, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);

	dbus_message_unref(reply);

error:
	dbus_message_unref(msg);
	return ret;
}

}

