/*
 *
 *  Authentication Agent implementation of bluez DBus API
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

#include "service.h"
#include "authagent.h"

namespace KBluetooth
{

DBusConnection* AuthAgent::conn = NULL;	
AuthAgent* AuthAgent::_authagent = NULL;	
DBusMessage* AuthAgent::_msg = NULL;	

AuthAgent::AuthAgent(DBusConnection *dbusconn, const QString &agent_path)
	: DBusSignal(BLUEZ_SERVICE, BLUEZ_PATH, INTERFACE_SECURITY, dbusconn) {

	conn = dbusconn;
	path = agent_path;

	reRegisterDefaultAgent = false;

	_authagent = this;

	match = QString("type='signal', interface='" INTERFACE_AUTHAGENT "'");

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch(match);
}	

AuthAgent::~AuthAgent()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;

	_authagent = 0;
}

bool AuthAgent::registerDefaultAuthorizationAgent()
{
	reRegisterDefaultAgent = true;
	const char *_path = path.latin1();
	return setString("RegisterDefaultAuthorizationAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_INVALID);
}

bool AuthAgent::unregisterDefaultAuthorizationAgent()
{
	reRegisterDefaultAgent = false;
	const char *_path = path.latin1();
	return setString("UnregisterDefaultAuthorizationAgent", DBUS_TYPE_STRING, &_path, DBUS_TYPE_INVALID);
}

void AuthAgent::sendAuth(bool grant)
{
	DBusMessage *reply = NULL;

	if (grant)
		reply = dbus_message_new_method_return(_msg);
	else
		reply = dbus_message_new_error(_msg, "org.bluez.Error.Rejected", "Rejected");

	if (!reply) {
		kdDebug() << k_funcinfo << "(): Reply failed." << endl;
		dbus_message_unref(_msg);
		return;
	}

	dbus_connection_send(conn, reply, NULL);
	dbus_message_unref(reply);
	dbus_message_unref(_msg);
}

void AuthAgent::setAlwaysTrust(const QString &address, const QString &servicepath)
{
	Service service(conn, servicepath);
	service.setTrusted(address);
}

DBusHandlerResult AuthAgent::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *path, *address, *service, *uuid;
	QString qarg = QString::null;

	if (DBusSignal::serviceDown(msg)) {
		emit _authagent->cleanup();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (DBusSignal::serviceUp(msg) && _authagent->reRegisterDefaultAgent) {
		_authagent->registerDefaultAuthorizationAgent();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_message_has_interface(msg, INTERFACE_AUTHAGENT))
	       return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, _authagent->path.latin1()))
	       return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;


	kdDebug() << "AuthAgent " << __func__ << "(): " << member << endl;

	dbus_error_init(&error);

	if (!strcmp("Authorize", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &path, DBUS_TYPE_STRING, &address, DBUS_TYPE_STRING, &service, DBUS_TYPE_STRING, &uuid, DBUS_TYPE_INVALID);

		_msg = msg;
     		dbus_message_ref(_msg);

		emit _authagent->authorize(path, address, service, uuid);
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (!strcmp("Cancel", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &path, DBUS_TYPE_STRING, &address, DBUS_TYPE_STRING, &service, DBUS_TYPE_STRING, &uuid, DBUS_TYPE_INVALID);

		_msg = msg;
     		dbus_message_ref(_msg);

		emit _authagent->authorize(path, address, service, uuid);
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (!strcmp("Release", member)) {
		// TODO
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

}

#include "authagent.moc"
