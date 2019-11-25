/*
 *
 *  Manager implementation of bluez DBus API
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

#include "manager.h"

namespace KBluetooth
{

Manager* Manager::_manager = NULL;	

Manager::Manager(DBusConnection *dbusconn) : DBusSignal(BLUEZ_SERVICE, BLUEZ_PATH, INTERFACE_MANAGER, dbusconn), conn(dbusconn) {

	_manager = this;

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch("type='signal',interface='" INTERFACE_MANAGER "', path='" BLUEZ_PATH "',sender='" BLUEZ_SERVICE "'");
	filter->addMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
}	

Manager::~Manager()
{
	filter->removeFilter(filterFunction);
	filter->removeMatch("type='signal',interface='" INTERFACE_MANAGER "', path='" BLUEZ_PATH "',sender='" BLUEZ_SERVICE "'");
	filter->removeMatch("type='signal',interface='" DBUS_INTERFACE_DBUS "', sender='" DBUS_SERVICE_DBUS "'");
	delete filter;

	_manager = 0;
}

unsigned int Manager::interfaceVersion()
{
	return getInteger("InterfaceVersion", DBUS_TYPE_UINT32);
}

QString Manager::defaultAdapter()
{
	QString s =  getString("DefaultAdapter");
    return s.isNull() ? "" : s;
}

QString Manager::findAdapter(const QString &pattern)
{
	QCString _pattern = pattern.latin1();
	const char* _patt = _pattern.data();
	return getString("FindAdapter", DBUS_TYPE_STRING, &_patt, DBUS_TYPE_INVALID);

}

Manager::AdapterList Manager::listAdapters()
{
	kdDebug() << k_funcinfo << getStringList("ListAdapters").count() << endl;
	return getStringList("ListAdapters");
}

QString Manager::findService(const QString &pattern)
{
	const char *_pattern = pattern.latin1();
	return getString("FindService", DBUS_TYPE_STRING, &_pattern, DBUS_TYPE_INVALID);
}

Manager::ServiceList Manager::listServices()
{
	return getStringList("ListServices");
}

QString Manager::activateService(const QString &pattern)
{
	const char *_pattern = pattern.latin1();
	return getString("ActivateService", DBUS_TYPE_STRING, &_pattern, DBUS_TYPE_INVALID);
}

DBusHandlerResult Manager::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg = QString::null;

	dbus_error_init(&error);

	if (DBusSignal::serviceDown(msg)) {
		kdDebug() << "Going to clean up! " << endl;
		emit _manager->cleanup();
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_message_has_interface(msg, INTERFACE_MANAGER))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, BLUEZ_PATH))
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
	kdDebug() << "Manager Signal: " << member << " Argument: " << qarg << endl;

	if (!strcmp("AdapterAdded", member)) {
	// XXX Workaround to avoid to early emit of AdapterAdded signal.
	// bluez-utils emits this signal before the Adapter is ready...
	// If the adapter is not ready we cannot retrieve information about it...
		usleep(500000);
		emit _manager->adapterAdded(qarg);
	} else if (!strcmp("AdapterRemoved", member)) {
		emit _manager->adapterRemoved(qarg);
	} else if (!strcmp("DefaultAdapterChanged", member)) {
		emit _manager->defaultAdapterChanged(qarg);
	} else if (!strcmp("ServiceAdded", member)) {
		emit _manager->serviceAdded(qarg);
	} else if (!strcmp("ServiceRemoved", member)) {
		emit _manager->serviceRemoved(qarg);
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "manager.moc"
