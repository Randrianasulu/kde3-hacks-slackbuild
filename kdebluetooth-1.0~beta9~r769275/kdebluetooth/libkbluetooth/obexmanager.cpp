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

#include "obexmanager.h"
namespace KBluetooth
{

ObexManager* ObexManager::_manager = NULL;	

ObexManager::ObexManager(DBusConnection *dbusconn) : DBusSignal(OBEX_SERVICE, OBEX_PATH, INTERFACE_OBEXMANAGER, dbusconn), conn(dbusconn) {
	
	_manager = this;

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch("type='signal',interface='" INTERFACE_OBEXMANAGER "', path='" OBEX_PATH "',sender='" OBEX_SERVICE "'");
	filter->addMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
}	

ObexManager::~ObexManager()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch("type='signal',interface='" INTERFACE_OBEXMANAGER "', path='" OBEX_PATH "',sender='" OBEX_SERVICE "'");
	filter->removeMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
	delete filter;

}


void ObexManager::createBluetoothSession(const QString &address, const QString &pattern/*, bool auto_connect, char* session_object*/)
{
	QCString _addr = address.latin1();
    	const char* _address = _addr.data();

	QCString _pat = pattern.latin1();
    	const char* _pattern = _pat.data();

//	dbus_bool_t auto_conn = auto_connect; 

	sendString("CreateBluetoothSession", DBUS_TYPE_STRING, &_address, DBUS_TYPE_STRING, &_pattern, 
			/*DBUS_TYPE_BOOLEAN, &auto_conn, DBUS_TYPE_STRING, &session_object,*/ DBUS_TYPE_INVALID);

}

void ObexManager::createBluetoothServer(const QString &source_address, const QString &pattern, char* session_object)
{
	QCString _addr = source_address.latin1();
    	const char* _source_address = _addr.data();

	QCString _pat = pattern.latin1();
    	const char* _pattern = _pat.data();

	sendString("CreateBluetoothServer", DBUS_TYPE_STRING, &_source_address, DBUS_TYPE_STRING, &_pattern, 
			DBUS_TYPE_STRING, &session_object, DBUS_TYPE_INVALID);

}

DBusHandlerResult ObexManager::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg = QString::null;

	dbus_error_init(&error);

	if (!dbus_message_has_interface(msg, INTERFACE_OBEXMANAGER))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, OBEX_PATH))
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
	kdDebug() << "Obex-Manager Signal: " << member << " Argument: " << qarg << endl;

	if (!strcmp("SessionCreated", member)) {
		kdDebug() << __func__ << " session created!!!" << endl;
		_manager->emit sessionCreated(qarg);
	} else if (!strcmp("SessionRemoved", member)) {
		_manager->emit sessionRemoved(qarg);
	}  else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}
}

#include "obexmanager.moc"
