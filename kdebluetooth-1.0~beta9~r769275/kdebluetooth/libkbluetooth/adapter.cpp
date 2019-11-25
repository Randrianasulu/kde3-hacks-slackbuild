/*
 *
 *  Adapter implementation of bluez DBus API
 *
 *  Copyright (C) 2003  Fred Schaettgen <kdebluetooth@schaettgen.de>
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

#include "adapter.h"

namespace KBluetooth
{

Adapter *Adapter::_adapter = NULL;	

Adapter::Adapter(const QString &path, DBusConnection *conn)
	: DBusSignal(BLUEZ_SERVICE, path, INTERFACE_ADAPTER, conn), path(path), conn(conn)
{
	_adapter = this;

	match = QString("type='signal',interface='" INTERFACE_ADAPTER "', path='%1',sender='" BLUEZ_SERVICE "'").arg(path);

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunctionAd);
	filter->addMatch(match);
}

Adapter::~Adapter()
{
	kdDebug() << k_funcinfo << endl;

	filter->removeFilter(filterFunctionAd);
	filter->removeMatch(match);
	delete filter;

	_adapter = 0;
}

QString Adapter::getAddress()
{
	return getString("GetAddress");
}

QString Adapter::getVersion()
{
	return getString("GetVersion");
}

QString Adapter::getRevision()
{
	return getString("GetRevision");
}

QString Adapter::getManufacturer()
{
	return getString("GetManufacturer");
}

QString Adapter::getCompany()
{
	return getString("GetCompany");
}

int Adapter::getModeInt()
{
	QString mode = getMode();

	if (mode == "off")
		return 0;
	else if (mode == "connectable")
		return 1;
	else if (mode == "discoverable")
		return 2;

	return -1;
}

QString Adapter::getMode()
{
	return getString("GetMode");
}

bool Adapter::setMode(int mode)
{

	if (mode == 0)
		return setMode("off");
	else if (mode == 1)
		return setMode("connectable");
	else if (mode == 2)
		return setMode("discoverable");

	return false;
}

bool Adapter::setMode(const QString &mode)
{
	QCString _mode = mode.latin1();
	const char* _mo = _mode.data();
	return setString("SetMode", DBUS_TYPE_STRING, &_mo, DBUS_TYPE_INVALID);
}

int Adapter::getDiscoverableTimeout()
{
	return getInteger("GetDiscoverableTimeout", DBUS_TYPE_UINT32);
}

bool Adapter::setDiscoverableTimeout(int timeout)
{

	return setString("SetDiscoverableTimeout", DBUS_TYPE_UINT32, &timeout, DBUS_TYPE_INVALID);
}

bool Adapter::isConnectable()
{
	return getBoolean("IsConnectable");
}

bool Adapter::isDiscoverable()
{
	return getBoolean("IsDiscoverable");
}

bool Adapter::isConnected(const QString &address)
{
	QCString _address = address.latin1();
	const char* _addr = _address.data();
	return setString("IsConnected", DBUS_TYPE_STRING, &_addr, DBUS_TYPE_INVALID);
}

Adapter::ConnectionList Adapter::listConnections()
{
	return getStringList("ListConnections");
}


QString Adapter::getMajorClass()
{
	return getString("GetMajorClass");
}

Adapter::MinorClassList Adapter::listAvailableMinorClasses()
{
	return getStringList("ListAvailableMinorClasses");
}

QString Adapter::getMinorClass()
{
	return getString("GetMinorClass");
}

bool Adapter::setMinorClass(const QString &minorclass)
{
	QCString _minorclass = minorclass.latin1();
	const char* _minorcl = _minorclass.data();
	return setString("SetMinorClass", DBUS_TYPE_STRING, &_minorcl, DBUS_TYPE_INVALID);
}

Adapter::ServiceClassList Adapter::getServiceClasses()
{
	return getStringList("GetServiceClasses");
}

QString Adapter::getDeviceName()
{
	return getString("GetName");
}

bool Adapter::setDeviceName(const QString &name)
{
	QCString _name = name.latin1();
	const char* _nam = _name.data();
	return setString("SetName", DBUS_TYPE_STRING, &_nam, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteVersion(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteVersion", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteRevision(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteRevision", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteManufacturer(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteManufacturer", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteCompany(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteCompany", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteMajorClass(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteMajorClass", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteMinorClass(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getString("GetRemoteMinorClass", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

Adapter::ServiceClassList Adapter::getRemoteServiceClasses(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getStringList("GetRemoteServiceClasses", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

Q_UINT32 Adapter::getRemoteClass(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getInteger("GetRemoteClass", DBUS_TYPE_UINT32, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

Adapter::ByteList Adapter::getRemoteFeatures(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();
	return getByteList("GetRemoteFeatures", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteName(const QString &address)
{
	QCString _addr = address.latin1();
	const char* _address = _addr.data();

	return getString("GetRemoteName", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteAlias(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getString("GetRemoteAlias", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::setRemoteAlias(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("SetRemoteAlias", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::clearRemoteAlias(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("ClearRemoteAlias", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::lastSeen(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getString("LastSeen", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

QString Adapter::lastUsed(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getString("LastUsed", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::disconnectRemoteDevice(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("DisconnectRemoteDevice", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

//
// Bonding functions
//

void Adapter::createBonding(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	sendString("CreateBonding", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::createBondingBlock(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getBoolean("CreateBonding", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::cancelBondingProcess(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getBoolean("CancelBondingProcess", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::removeBonding(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getBoolean("RemoveBonding", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::hasBonding(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("HasBonding", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::setTrusted(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("SetTrusted", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::removeTrust(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("RemoveTrust", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

bool Adapter::isTrusted(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getBoolean("IsTrusted", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

Adapter::BondingList Adapter::listBondings()
{
	return getStringList("ListBondings");
}

//
// Pin and Encryption functions
//

Q_INT8 Adapter::getPinCodeLength(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getInteger("GetPinCodeLength", DBUS_TYPE_BYTE, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

Q_INT8 Adapter::getEncryptionKeySize(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
	return getInteger("GetEncryptionKeySize", DBUS_TYPE_BYTE, DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);
}

//
// Discovery
//

bool Adapter::discoverDevices()
{
	return getBoolean("DiscoverDevices");
}

bool Adapter::discoverDevicesWithoutNameResolving()
{
	return getBoolean("DiscoverDevicesWithoutNameResolving");
}

bool Adapter::cancelDiscovery()
{
	return getBoolean("CancelDiscovery");
}

bool Adapter::startPeriodicDiscovery()
{
	return setString("StartPeriodicDiscovery");
}

bool Adapter::stopPeriodicDiscovery()
{
	return setString("StopPeriodicDiscovery");
}

bool Adapter::isPeriodicDiscovery()
{
	return getBoolean("IsPeriodicDiscovery");
}

bool Adapter::setPeriodicDiscoveryNameResolving(bool _resolve_names)
{
        dbus_bool_t resolve_names = _resolve_names;
	return setString("SetPeriodicDiscoveryNameResolving", DBUS_TYPE_BOOLEAN, &resolve_names, DBUS_TYPE_INVALID);
}

bool Adapter::getPeriodicDiscoveryNameResolving()
{
	return getBoolean("GetPeriodicDiscoveryNameResolving");
}

Adapter::ServiceHandleList Adapter::getRemoteServiceHandles(const QString &address, const QString &match)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();
    QCString _mat = match.latin1();
    const char* _match = _mat.data();

	return getUInt32List("GetRemoteServiceHandles", DBUS_TYPE_STRING, &_address, DBUS_TYPE_STRING, &_match, DBUS_TYPE_INVALID);
}

Adapter::ByteList Adapter::getRemoteServiceRecordAsByteList(const QString &address, Q_UINT32 handle)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getByteList("GetRemoteServiceRecord", DBUS_TYPE_STRING, &_address, DBUS_TYPE_UINT32, &handle, DBUS_TYPE_INVALID);
}

QString Adapter::getRemoteServiceRecordAsXML(const QString &address, Q_UINT32 handle)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return getString("GetRemoteServiceRecordAsXML", DBUS_TYPE_STRING, &_address, DBUS_TYPE_UINT32, &handle, DBUS_TYPE_INVALID);
}

ServiceRecord Adapter::getRemoteServiceRecord(const QString &address, Q_UINT32 handle)
{
	return ServiceRecord( getRemoteServiceRecordAsXML(address, handle) );
}

QStringList Adapter::listRemoteDevices()
{
	return getStringList("ListRemoteDevices");
}

QStringList Adapter::listRecentRemoteDevices(const QString &date)
{
    QCString _dat = date.latin1();
    const char* _date = _dat.data();
	return  getStringList("ListRecentRemoteDevices", DBUS_TYPE_STRING, &_date, DBUS_TYPE_INVALID);
}



QStringList Adapter::listTrusts()
{
	return  getStringList("ListTrusts");
}


QStringList Adapter::getRemoteServiceIdentifiers(const QString &address)
{
    QCString _addr = address.latin1();
    const char* _address = _addr.data();

	return  getStringList("GetRemoteServiceIdentifiers", DBUS_TYPE_STRING, &_address, DBUS_TYPE_INVALID);

}





//
// dbus filter function
//

DBusHandlerResult Adapter::filterFunctionAd(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg = QString::null;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_interface(msg, "org.bluez.Adapter"))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	/* make sure adapter got already initalized ... this avoids a lot of race condition which leads to segfaults */
	if (!_adapter)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, _adapter->path.utf8()))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	kdDebug() << "Adapter " << __func__ << "(): " << member << endl;


	if (!strcmp("ModeChanged", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->modeChanged(qarg);
	} else if (!strcmp("DiscoverableTimeoutChanged", member)) {
		Q_UINT32 timeout;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT32, &timeout, DBUS_TYPE_INVALID);
		emit _adapter->discoverableTimeoutChanged(timeout);
	} else if (!strcmp("MinorClassChanged", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->minorClassChanged(qarg);
	} else if (!strcmp("NameChanged", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->nameChanged(qarg);
	} else if (!strcmp("DiscoveryStarted", member)) {
		emit _adapter->discoveryStarted();
	} else if (!strcmp("DiscoveryCompleted", member)) {
		emit _adapter->discoveryCompleted();
	} else if (!strcmp("PeriodicDiscoveryStopped", member)) {
		emit _adapter->periodicDiscoveryStopped();
	} else if (!strcmp("RemoteDeviceFound", member)) {
		Q_UINT32 devclass;
		short rssi;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_UINT32, &devclass, DBUS_TYPE_INT16, &rssi, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteDeviceFound(qarg, devclass, rssi);
	} else if (!strcmp("RemoteDeviceDisappeared", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteDeviceDisappeared(qarg);
	} else if (!strcmp("RemoteClassUpdated", member)) {
		Q_UINT32 devclass;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_UINT32, &devclass, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteClassUpdated(qarg, devclass);
	} else if (!strcmp("RemoteNameUpdated", member)) {
		const char *name;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		QString qname = QString::fromUtf8(name);
		emit _adapter->remoteNameUpdated(qarg, qname);
	} else if (!strcmp("RemoteNameFailed", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteNameFailed(qarg);
	} else if (!strcmp("RemoteAliasChanged", member)) {
		const char *name;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		QString qname = QString::fromUtf8(name);
		emit _adapter->remoteAliasChanged(qarg, qname);
	} else if (!strcmp("RemoteAliasCleared", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteAliasCleared(qarg);
	} else if (!strcmp("RemoteDeviceConnected", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteDeviceConnected(qarg);
	} else if (!strcmp("RemoteDeviceDisconnected", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->remoteDeviceDisconnected(qarg);
	} else if (!strcmp("BondingCreated", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->bondingCreated(qarg);
	} else if (!strcmp("BondingRemoved", member)) {
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		emit _adapter->bondingRemoved(qarg);
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "adapter.moc"
