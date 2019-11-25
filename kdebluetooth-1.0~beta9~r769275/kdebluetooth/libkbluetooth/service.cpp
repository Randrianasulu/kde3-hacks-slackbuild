/*
 *
 *  Service implementation of bluez DBus API
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

namespace KBluetooth
{

Service* Service::_service = NULL;	

Service::Service(DBusConnection *dbusconn, const QString &servicepath) 
	: DBusSignal(BLUEZ_SERVICE, servicepath, INTERFACE_SERVICE, dbusconn), path(servicepath) {

	conn = dbusconn;
	_service = this;

	match = QString("type='signal',interface='" INTERFACE_SERVICE "', path='%1',sender='" BLUEZ_SERVICE "'").arg(servicepath);

	// TODO set up filter and signals...
	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch(match);
}	

Service::~Service()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;

	_service = 0;
}

QString Service::getIdentifier()
{
	return getString("GetIdentifier");
}

QString Service::getName()
{
	return getString("GetName");
}

QString Service::getDescription()
{
	return getString("GetDescription");
}

QString Service::getBusName()
{
	return getString("GetBusName");
}

bool Service::start()
{
	return setString("Start");
}

bool Service::stop()
{
	return setString("Stop");
}

bool Service::isRunning()
{
	return getBoolean("IsRunning");
}	

bool Service::isExternal()
{
	return getBoolean("IsExternal");
}

Service::UserList Service::listUsers()
{
	return getStringList("ListUsers");
}

bool Service::removeUser(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return setString("RemoveUser", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Service::setTrusted(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return setString("SetTrusted", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Service::isTrusted(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getBoolean("IsTrusted", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Service::removeTrust(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return setString("RemoveTrust", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QStringList Service::listTrusts()
{
	return  getStringList("ListTrusts");
}

DBusHandlerResult Service::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	dbus_error_init(&error);

	dbus_message_get_args(msg, &error, DBUS_TYPE_INVALID);

	if (dbus_error_is_set(&error)) {
		kdDebug() << __func__ << " (" << member << "): " << error.message << endl;
		dbus_error_free(&error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	kdDebug() << "Service Signal: " << member << endl;

	if (!strcmp("Started", member)) {
		emit _service->started();
	} else if (!strcmp("Stopped", member)) {
		emit _service->stopped();
	} else if (!strcmp("TrustAdded", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _service->trustAdded(qarg);
	} else if (!strcmp("TrustRemoved", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _service->trustRemoved(qarg);
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "service.moc"
