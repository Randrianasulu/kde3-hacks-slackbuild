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

#ifndef ADAPTER_H
#define ADAPTER_H

#include "dbussignal.h"
#include "dbusfilter.h"
#include "servicerecord.h"

#define INTERFACE_ADAPTER "org.bluez.Adapter"
#define MAX_DISCOVERABLE_TIMEOUT 1860	// Maximal timeout 31 minutes

namespace KBluetooth
{

class Adapter : public DBusSignal
{
	Q_OBJECT

	public:
		Adapter(const QString &adapter, DBusConnection *conn);
		~Adapter();

		typedef QValueList<QString> ConnectionList;
		typedef QValueList<QString> MinorClassList;
		typedef QValueList<QString> ServiceClassList;
		typedef QValueList<Q_UINT32> ServiceHandleList;
		typedef QValueList<ServiceRecord*> ServiceRecordList;
		typedef QValueList<QString> BondingList;
		typedef QValueList<Q_UINT8> ByteList;


		/** Returns the device address of the given adapter.
		@return The device address
		*/
		QString getAddress();

		QString getVersion();
		QString getRevision(); 
		QString getManufacturer();
		QString getCompany();
		QString getMode();
		int getModeInt();
		int getDiscoverableTimeout();
		bool setDiscoverableTimeout(int timeout);
		bool isConnectable();
		bool isDiscoverable();
		bool isConnected(const QString &address);
		ConnectionList listConnections();
		QString getMajorClass();
		MinorClassList listAvailableMinorClasses();
		QString getMinorClass();

		ServiceClassList getServiceClasses();

		/** Returns the name of the adapter.
		@return The device name
		*/
		QString getDeviceName();
//		bool setDeviceName(const QString &name);

		QString getRemoteVersion(const QString &address);
		QString getRemoteRevision(const QString &address);
		QString getRemoteManufacturer(const QString &address);
		QString getRemoteCompany(const QString &address);
		QString getRemoteMajorClass(const QString &address);
		QString getRemoteMinorClass(const QString &address);
		ServiceClassList getRemoteServiceClasses(const QString &address);
		Q_UINT32 getRemoteClass(const QString &address);
		ByteList getRemoteFeatures(const QString &address);
		QString getRemoteName(const QString &address);
		QString getRemoteAlias(const QString &address);
		QString lastSeen(const QString &address);
		QString lastUsed(const QString &address);
		bool isTrusted(const QString &address);
		bool hasBonding(const QString &address);
		BondingList listBondings();
		Q_INT8 getPinCodeLength(const QString &address);
		Q_INT8 getEncryptionKeySize(const QString &address);
		bool isPeriodicDiscovery();
		bool getPeriodicDiscoveryNameResolving();
		ServiceHandleList getRemoteServiceHandles(const QString &address, const QString &match); 
		ByteList getRemoteServiceRecordAsByteList(const QString &address, Q_UINT32 handle);
		QString getRemoteServiceRecordAsXML(const QString &address, Q_UINT32 handle);
		ServiceRecord getRemoteServiceRecord(const QString &address, Q_UINT32 handle);
		QStringList listRemoteDevices();
		QStringList listRecentRemoteDevices(const QString &date);


		QStringList listTrusts();
		QStringList getRemoteServiceIdentifiers(const QString &address);
		

		DBusConnection* getDBus() { return conn; };
		QString getPath() { return path; };

	public slots:
		bool setDeviceName(const QString &name);
		bool setMode(const QString &mode);
		bool setMode(int mode);
		bool setMinorClass(const QString &minorclass);
		bool setRemoteAlias(const QString &address);
		bool clearRemoteAlias(const QString &address);
		bool disconnectRemoteDevice(const QString &address);
		bool createBondingBlock(const QString &address);
		void createBonding(const QString &address);
		bool cancelBondingProcess(const QString &address);
		bool removeBonding(const QString &address);
		bool setTrusted(const QString &address);
		bool removeTrust(const QString &address);
		bool discoverDevices();
		bool discoverDevicesWithoutNameResolving();
		bool cancelDiscovery();
		bool startPeriodicDiscovery();
		bool stopPeriodicDiscovery();
		bool setPeriodicDiscoveryNameResolving(bool);
	
	signals:
		void modeChanged(const QString &mode);
		void discoverableTimeoutChanged(int timeout);
		void minorClassChanged(const QString &minor);
		void nameChanged(const QString &name);
		void discoveryStarted(void);
		void discoveryCompleted(void);
		void remoteDeviceFound(const QString &address, int devclass, short rssi);
		void remoteDeviceDisappeared(const QString &address);
		void remoteClassUpdated(const QString &address, int devclass);
		void remoteNameUpdated(const QString &address, const QString &name);
		void remoteNameFailed(const QString &address);
		void remoteAliasChanged(const QString &address, const QString &name);
		void remoteAliasCleared(const QString &address);
		void remoteDeviceConnected(const QString &address);
		void remoteDeviceDisconnected(const QString &address);
		void bondingCreated(const QString &address);
		void bondingRemoved(const QString &address);
		void periodicDiscoveryStopped();

	private:
		QString path;
		QString match;
		DBusConnection *conn;
		DBusFilter *filter;

		static Adapter *_adapter;
		static DBusHandlerResult filterFunctionAd(DBusConnection * conn, DBusMessage *msg, void * data);
};

}

#endif	// ADAPTER_H_

