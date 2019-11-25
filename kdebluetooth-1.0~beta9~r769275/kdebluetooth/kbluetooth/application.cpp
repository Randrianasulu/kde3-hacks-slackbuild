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


#include "pindialog.h"
#include "authorize.h"
#include "trayicon.h"

#include "application.h"

KBluetoothApp::KBluetoothApp() :
    KUniqueApplication()
{
    m_config = new KConfig("kdebluetoothrc");
    // Enable autostart
    m_config->setGroup("General");
    m_config->writeEntry("AutoStart", true);
    m_config->sync();
    noAdapter = 0;	
	noDBus = 0;
    bFirstNewInstance = true;

    // blueZ DBUS API
    dbus = new DBusInit();
    conn = dbus->getDBus();

	if (!conn) {
		noDBus = 1;
		return;
	}
    manager = new Manager(conn);
    if (!manager->listAdapters().count() || manager->defaultAdapter() == "") { 
		noAdapter = 1;
	 	adapter = NULL;
    } else {

   	 	 adapter = new Adapter(manager->defaultAdapter(), conn);
    	 rfcomm = new RfcommPortListener(m_config,adapter);
	}
		

   

//    hcid = new DBusHcid(this);

    // Register Default PasskeyAgent
    passkeyagent = new PasskeyAgent(conn, "/org/kde/kbluetooth_1234");
    passkeyagent->registerDefaultPasskeyAgent();

    authagent = new AuthAgent(conn, "/org/kde/kbluetooth_auth_1234");
    authagent->registerDefaultAuthorizationAgent();




    connect(passkeyagent, SIGNAL(request(const QString&, const QString&, bool)), SLOT(slotPinDialog(const QString&, const QString&, bool))); 
    connect(authagent, SIGNAL(authorize(const QString&, const QString&, const QString&, const QString&)), SLOT(slotAuthDialog(const QString&, const QString&, const QString&, const QString&))); 

    connect(manager, SIGNAL(adapterAdded(const QString&)), SLOT(slotAdapterAdded(const QString&)));
    connect(manager, SIGNAL(adapterRemoved(const QString&)), SLOT(slotAdapterRemoved(const QString&)));
   	trayIcon = new TrayIcon(this);
   	setMainWidget(trayIcon);

    connect(trayIcon, SIGNAL(setKbtobexSrv(bool)), this, SLOT(slotKbtobexSrv(bool)));

//    slotAuthDialog("/org/bluez/hci0", "00:00:00:ff:ff:ff", "foo", "0x213");

}


void KBluetoothApp::slotAdapterAdded(const QString& /*address*/) {

	 if (!noAdapter)
	 	 return;
	 else {
	 	 noAdapter = 0;
   	 	 adapter = new Adapter(manager->defaultAdapter(), conn);
    	 rfcomm = new RfcommPortListener(m_config,adapter);
	 }

}

void KBluetoothApp::slotAdapterRemoved(const QString& /*address*/) {

	 	 noAdapter = 1;
	 	 if (rfcomm)
	     	 delete rfcomm;

}


void KBluetoothApp::slotKbtobexSrv(bool status) {
	if (status)
		rfcomm = new RfcommPortListener(m_config,adapter);
	else
		delete rfcomm;

}

KBluetoothApp::~KBluetoothApp()
{
    delete m_config;
    if (!noDBus) {
    	passkeyagent->unregisterDefaultPasskeyAgent();	
    	delete trayIcon;
	 	if (adapter != NULL)
	 	 	delete adapter;
	 	delete manager;
//	 	delete dbus;
    }
}

int KBluetoothApp::newInstance()
{
    // KBluetooth is started via autostart. If session management
    // is activated it will be called twice after login. This would
    // set the tray icon to visible every time, so we turn off
    // session management.
    disableSessionManagement();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    // If kbluetooth is started while already running we
    // make the tray icon permanently visible
    if (bFirstNewInstance == false) {
        if ((args->isSet("dontforceshow") == false) && trayIcon) {
            trayIcon->setAlwaysShowIcon(true);
        }
    }
    bFirstNewInstance = false;
    return 0;
}

void KBluetoothApp::slotPinDialog(const QString &path, const QString &address, bool isNumeric)
{
	kdDebug() << k_funcinfo << " (" << path << "," << address << "," << isNumeric << ")" << endl;

	PinDialog *dialog = new PinDialog(address, adapter->getRemoteName(address), passkeyagent); 

	kapp->updateUserTimestamp();
	dialog->show();
	dialog->raise();

}

void KBluetoothApp::slotAuthDialog(const QString &path, const QString &address, const QString &service, const QString &uuid)
{
	kdDebug() << k_funcinfo << " (" << path << "," << address << "," << service << "," << uuid << ")" << endl;

	AuthorizeDialog *dialog = new AuthorizeDialog(address, adapter->getRemoteName(address), service, uuid, authagent); 
	
	kapp->updateUserTimestamp();
	dialog->show();
	dialog->raise();

	
}

#include "application.moc"
