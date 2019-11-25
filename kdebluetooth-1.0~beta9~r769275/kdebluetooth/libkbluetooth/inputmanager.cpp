/*
 *
 *  Input Device InputManager implementation of bluez DBus API
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

#include "inputmanager.h"

namespace KBluetooth
{

InputManager* InputManager::_inputmanager = NULL;	

InputManager::InputManager(DBusConnection *dbusconn, const QString &bus) 
	: DBusSignal(bus, BLUEZ_PATH_INPUTMANAGER, INTERFACE_INPUTMANAGER, dbusconn), conn(dbusconn) {

	_inputmanager = this;

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch("type='signal',interface='" INTERFACE_INPUTMANAGER "', path='" BLUEZ_PATH_INPUTMANAGER "',sender="+bus );
	filter->addMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
}	

InputManager::~InputManager()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;

	_inputmanager = 0;
}

InputManager::InputDeviceList InputManager::listDevices()
{
	return getStringList("ListDevices");
}

void InputManager::createDevice(const QString &address)
{
	const char *_address = address.latin1();
	sendString("CreateDevice", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString InputManager::createDeviceBlock(const QString &address)
{
	const char *_address = address.latin1();
	return getString("CreateDevice", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool InputManager::removeDevice(const QString &address)
{
	const char *_address = address.latin1();
	return setString("RemoveDevice", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

DBusHandlerResult InputManager::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{
	kdDebug() << k_funcinfo << endl;
	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg = QString::null;

	dbus_error_init(&error);

	if (DBusSignal::serviceDown(msg)) {
		emit _inputmanager->cleanup();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_message_has_interface(msg, INTERFACE_INPUTMANAGER))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, BLUEZ_PATH_INPUTMANAGER))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;


	dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);

	if (dbus_error_is_set(&error)) {
		kdDebug() << __func__ << " (" << member << "): " << error.message << endl;
		dbus_error_free(&error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	
	qarg = QString::fromUtf8(arg);
	kdDebug() << "InputManager Signal: " << member << " Argument: " << qarg << endl;

	if (!strcmp("DeviceCreated", member)) {
		emit _inputmanager->deviceCreated(qarg);
	} else if (!strcmp("DeviceRemoved", member)) {
		emit _inputmanager->deviceRemoved(qarg);
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "inputmanager.moc"
