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

#include "passkeyagent.h"

namespace KBluetooth
{

DBusConnection* PasskeyAgent::conn = NULL;	
PasskeyAgent* PasskeyAgent::_passkeyagent = NULL;	
DBusMessage* PasskeyAgent::_msg = NULL;	

PasskeyAgent::PasskeyAgent(DBusConnection *dbusconn, const QString &agent_path) 
	: DBusSignal(BLUEZ_SERVICE, BLUEZ_PATH, INTERFACE_SECURITY, dbusconn) {

	conn = dbusconn;
       	path = agent_path;

	reRegisterDefaultAgent = false;

	_passkeyagent = this;

	match = QString("type='signal', interface='" INTERFACE_PASSKEYAGENT "'");

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch(match);

}	

PasskeyAgent::~PasskeyAgent()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;

	_passkeyagent = 0;
}

bool PasskeyAgent::registerPasskeyAgent(const QString &address)
{
	const char *_path = path.latin1();
	const char *_address = address.latin1();

	return setString("RegisterPasskeyAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}


bool PasskeyAgent::unregisterPasskeyAgent(const QString &address)
{
	const char *_path = path.latin1();
	const char *_address = address.latin1();

	return setString("UnregisterPasskeyAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool PasskeyAgent::registerDefaultPasskeyAgent()
{
	reRegisterDefaultAgent = true;
	const char *_path = path.latin1();
	return setString("RegisterDefaultPasskeyAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_INVALID);
}


bool PasskeyAgent::unregisterDefaultPasskeyAgent()
{
	reRegisterDefaultAgent = false;
	const char *_path = path.latin1();
	return setString("UnregisterDefaultPasskeyAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_INVALID);
}


DBusHandlerResult PasskeyAgent::pinRequest(DBusMessage *msg)
{

	dbus_bool_t isnumeric;
	const char *_path, *_address;

	_msg = msg;
     	dbus_message_ref(_msg);

	if (!dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_BOOLEAN, &isnumeric, DBUS_TYPE_INVALID)) {
		kdDebug() << __func__ << " PasskeyAgent: Invalid arguemntes for Passkey Request method." << endl;
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	emit request(_path, _address, isnumeric);

	return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult PasskeyAgent::pinConfirm(DBusMessage *msg)
{
	const char *_path, *_address, *_value;

	_msg = msg;
	dbus_message_ref(_msg);

	if (!dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_STRING, &_value, DBUS_TYPE_INVALID)) {
		kdDebug() << __func__ << " PasskeyAgent: Invalid arguemntes for Passkey Confirm method." << endl;
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	emit confirm(_path, _address, _value);

	return DBUS_HANDLER_RESULT_HANDLED;
}

void PasskeyAgent::sendPasskey(const QString &passkey)
{
	kdDebug() << k_funcinfo << endl;

	const char *_passkey = passkey.ascii();

	DBusMessage *rep = dbus_message_new_method_return(_msg);
        if (!rep) {
                kdDebug() << "DBusPasskeyAgent: " << __func__ << "(): Reply failed." << endl;
		return;
        }

	dbus_message_append_args(rep, DBUS_TYPE_STRING, &_passkey, DBUS_TYPE_INVALID);


	DBusError error;
	dbus_error_init(&error);

	dbus_connection_send(conn, rep, NULL);

	dbus_message_unref(rep);
	dbus_message_unref(_msg);
}

void PasskeyAgent::sendConfirm(bool valid)
{
	DBusMessage *reply = NULL;

	if (valid)
		reply = dbus_message_new_method_return(_msg);
	else
		reply = dbus_message_new_error(_msg, "org.bluez.Error.Rejected", "Rejected");

	if (!reply) {
		kdDebug() << __func__ << " DBusPasskeyAgent: " << __func__ << "(): Reply failed." << endl;
		dbus_message_unref(_msg);
		return;
	}

	dbus_connection_send(conn, reply, NULL);
	dbus_message_unref(reply);
	dbus_message_unref(_msg);
}

DBusHandlerResult PasskeyAgent::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *_path, *_address, *_value;
	QString qarg = QString::null;

	if (DBusSignal::serviceDown(msg)) {
		emit _passkeyagent->cleanup();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (DBusSignal::serviceUp(msg) && _passkeyagent->reRegisterDefaultAgent) {
		_passkeyagent->registerDefaultPasskeyAgent();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_message_has_interface(msg, INTERFACE_PASSKEYAGENT))
	       return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, _passkeyagent->path.latin1()))
	       return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;


	kdDebug() << "PasskeyAgent " << __func__ << "(): " << member << endl;

	dbus_error_init(&error);

	if (!strcmp("Request", member)) {
		return _passkeyagent->pinRequest(msg);
	}

	if (!strcmp("Confirm", member)) {
		return _passkeyagent->pinConfirm(msg);
	}

	if (!strcmp("Display", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_STRING, &_value, DBUS_TYPE_INVALID);

		emit _passkeyagent->display(QString(_path), QString(_address), QString(_value));
		return DBUS_HANDLER_RESULT_HANDLED;

	}

	if (!strcmp("Keypress", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
		emit _passkeyagent->keypress(QString(_path), QString(_address));
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (!strcmp("Complete", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
		emit _passkeyagent->complete(QString(_path), QString(_address));
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	if (!strcmp("Cancel", member)) {	
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &_path, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
		emit _passkeyagent->cancel(QString(_path), QString(_address));
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (!strcmp("Release", member)) {
		emit _passkeyagent->release();
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

}

#include "passkeyagent.moc"
