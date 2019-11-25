/*
 *
 *  Device configuration dialog for kbluetooth
 *
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


#include <kdebug.h>

#include "adapterconfig.h"
#include "deviceconfig.h"

DeviceConfig::DeviceConfig(KBluetoothApp *_app)
   : KDialogBase(NULL, "deviceconfig", true, "Device Config", (Ok)), app(_app), manager(_app->manager)
{
	nodevice = NULL;

	tabWidget = new QTabWidget(this);
	Manager::AdapterList list = manager->listAdapters();
	Manager::AdapterList::iterator it;

	for (it = list.begin(); it != list.end(); ++it)
		addAdapter((*it));
	if (list.count() == 0) {
		
		nodevice = new QLabel( i18n("No Bluetooth device found!"), this );
		tabWidget->addTab( nodevice, i18n("no device"));
		tabWidget->setMinimumSize(250,300);
	}

	tabWidget->show();
	setMainWidget(tabWidget);

//	connect(this, SIGNAL(okClicked()), this, SLOT(closeDeviceConfig()));
	connect(this, SIGNAL(okClicked()), tabWidget, SLOT(close()));
	connect(manager, SIGNAL(adapterAdded(const QString&)), SLOT(addAdapter(const QString&)));
	connect(manager, SIGNAL(adapterRemoved(const QString&)), SLOT(removeAdapter(const QString&)));
	connect(manager, SIGNAL(cleanup()), SLOT(cleanup()));
	


}

DeviceConfig::~DeviceConfig()
{
	delete tabWidget;
}


void DeviceConfig::closeDeviceConfig()
{
	delete tabWidget;

}


void DeviceConfig::addAdapter(const QString &path)
{	
	
	kdDebug() << __func__ << "(" << path << ")" << endl;

	AdapterConfig *aconfig;
	if (path == app->manager->defaultAdapter()) {
		
		aconfig = new AdapterConfig(app);
	} else {
		aconfig = new AdapterConfig(app->conn, path);
	}
	
	tabWidget->addTab( aconfig->dialog(), aconfig->getName() );

	if (tabWidget->isTabEnabled(aconfig->dialog())) {
	tabWidget->showPage( aconfig->dialog());
	}
//	kdDebug() << __func__ << "(foo)" << endl;
	adapterList.insert( path, aconfig );

	
	connect(aconfig->dialog()->adapterName, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangeTab(const QString &)));

	if (nodevice) {
	//	tabWidget->removePage(nodevice);
		delete nodevice;
	}
}

void DeviceConfig::removeAdapter(const QString &path)
{

	kdDebug() << __func__ << "(" << path << ")" << endl;

	AdapterConfig *aconfig = adapterList[path];
	if (!aconfig)
		return;
	
	tabWidget->removePage( aconfig->dialog() );
	
	adapterList.remove( path );


	if (adapterList.count() == 0) {
//		tabWidget->removePage( aconfig->dialog() );
//		delete nodevice;
		
		nodevice = new QLabel( i18n("No Bluetooth device found!"), tabWidget);
		tabWidget->addTab( nodevice, i18n("no device"));
		if (tabWidget->isTabEnabled(nodevice)) {
			tabWidget->showPage( nodevice );
		}
//		tabWidget->show();
	}

	delete aconfig;
}

void DeviceConfig::slotChangeTab(const QString &name) {
	
	tabWidget->changeTab(tabWidget->currentPage(), name);
}

void DeviceConfig::cleanup()
{
	kdDebug() << k_funcinfo << endl;
	QDictIterator<AdapterConfig> it(adapterList);

	for (; it.current(); ++it)
		removeAdapter( it.currentKey() );

}

#include "deviceconfig.moc"
