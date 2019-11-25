/*
 *
 *  Input Device Service implementation of BlueZ D-Bus API
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

#include "inputdevice.h"

namespace KBluetooth
{

InputDevice* InputDevice::_inputdevice = NULL;	

InputDevice::InputDevice(DBusConnection *dbusconn, const QString &busid, const QString &devicepath) 
	: DBusSignal(busid, devicepath, INTERFACE_INPUTDEVICE, dbusconn), path(devicepath), conn(dbusconn) {
	_inputdevice = this;

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch("type='signal',interface='" INTERFACE_INPUTDEVICE "', path='" + path + "',sender='" BLUEZ_SERVICE "'");
	filter->addMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
}	

InputDevice::~InputDevice()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;

	_inputdevice = 0;
}

QString InputDevice::getAddress()
{
	return getString("GetAddress");
}

QString InputDevice::getName()
{
	return getString("GetName");
}

int InputDevice::getProductId()
{
	return getInteger("getProductId", DBUS_TYPE_UINT16);
}

int InputDevice::getVendorId()
{
	return getInteger("getVendorId", DBUS_TYPE_UINT16);
}

bool InputDevice::isConnected()
{
	return getBoolean("IsConnected");
}

void InputDevice::connect()
{
	sendString("Connect");
}

bool InputDevice::connectBlock() 
{
	return getBoolean("Connect");

}

void InputDevice::disconnect()
{
	sendString("Disconnect");
}

DBusHandlerResult InputDevice::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);

	dbus_error_init(&error);

	if (!dbus_message_has_interface(msg, INTERFACE_INPUTDEVICE))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, _inputdevice->path.ascii()))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;


	if (dbus_error_is_set(&error)) {
		kdDebug() << __func__ << " (" << member << "): " << error.message << endl;
		dbus_error_free(&error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	
	kdDebug() << "InputDevice Signal: " << member << endl;

	if (!strcmp("Connected", member)) {
		emit _inputdevice->connected();
	} else if (!strcmp("Disconnected", member)) {
		emit _inputdevice->disconnected();
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "inputdevice.moc"
