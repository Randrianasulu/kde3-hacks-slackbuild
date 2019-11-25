/*
 *
 *  kbluetooth - Another KDE Bluetooth tray icon application.
 *
 *  Copyright (C) 2003  Fred Schaettgen <kbluetoothd@schaettgen.de>
 *  Copyright (C) 2006  Daniel Gollub <dgollub@suse.de>
 *
 *
 *  This file is part of kbluetooth.
 *
 *  kbluetooth is free software; you can redistribute it and/or modify
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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>

#include <qptrlist.h>
#include <qcstring.h>
#include <qsocketnotifier.h>

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/passkeyagent.h>
#include <libkbluetooth/authagent.h>

#include "rfcommportlistener.h"



class KConfig;
class TrayIcon;

using namespace KBluetooth;

class KBluetoothApp: public KUniqueApplication
{
Q_OBJECT
public:
	KBluetoothApp();
	~KBluetoothApp();
	virtual int newInstance();

	TrayIcon *trayIcon;

	DBusInit *dbus;
	DBusConnection *conn;
	PasskeyAgent *passkeyagent;
	AuthAgent *authagent;
	Manager *manager;
	Adapter *adapter;
	bool noAdapter;
	bool noDBus;

public slots:
	void slotPinDialog(const QString&, const QString&, bool);
	void slotAuthDialog(const QString&, const QString&, const QString&, const QString&);
	void slotKbtobexSrv(bool status);
	void slotAdapterAdded(const QString&);
	void slotAdapterRemoved(const QString&);
private:
	KConfig* m_config;
	bool bFirstNewInstance;
	RfcommPortListener* rfcomm;
};


#endif // APPLICATION_H_
