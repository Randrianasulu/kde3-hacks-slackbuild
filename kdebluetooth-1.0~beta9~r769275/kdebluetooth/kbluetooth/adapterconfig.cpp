/*
 *
 *  Adapter configuration dialog for kbluetooth
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

AdapterConfig::AdapterConfig(DBusConnection *conn, QString path)
{
	adapterDialog = new AdapterDialog(); 
	adapter = new Adapter(path, conn);
	setupDialog();	


}

AdapterConfig::AdapterConfig(KBluetoothApp* app) {

	adapterDialog = new AdapterDialog();
	adapter = app->adapter;
	setupDialog();

}

void AdapterConfig::setupDialog() {

	adapterDialog->deviceModes->insertItem( i18n("off") );
	adapterDialog->deviceModes->insertItem( i18n("connectable") );
	adapterDialog->deviceModes->insertItem( i18n("discoverable") );


	// ADAPTER -> DIALOG
	connect(adapter, SIGNAL(nameChanged(const QString&)), this, SLOT(slotChangeName(const QString&)));
	connect(adapter, SIGNAL(discoverableTimeoutChanged(int)), this, SLOT(slotChangeTimeout(int)));


	// DIALOG -> ADAPTER
	connect(adapterDialog->adapterName, SIGNAL(textChanged(const QString &)), adapter, SLOT(setDeviceName(const QString &)));

	// DIALOG -> (DIALOG ->) ADAPTER
	connect(adapterDialog->deviceModes, SIGNAL(activated(int)), this, SLOT(slotSetMode(int)));
	connect(adapterDialog->deviceClass, SIGNAL(activated(const QString &)), this, SLOT(slotSetClass(const QString &)));
	connect(adapterDialog->sliderTimeout, SIGNAL(valueChanged(int)), this, SLOT(slotChangeAdapterTimeout(int)));


	addAdapterDialogInfo(); 
	
	name = adapter->getDeviceName();
	adapterDialog->adapterName->setText(name);
	adapterDialog->deviceModes->setCurrentItem( adapter->getModeInt() );

	slotSetMode(adapter->getModeInt());
	slotSetClass(adapter->getMinorClass());
	slotChangeTimeout(adapter->getDiscoverableTimeout());

	adapterDialog->show();



}

AdapterConfig::~AdapterConfig()
{
	kdDebug() << __func__ << endl;
	delete adapter;
	delete adapterDialog;

}

void AdapterConfig::addAdapterDialogInfo()
{

	QString revision = adapter->getRevision();
	QString address = adapter->getAddress();
	QString company = adapter->getCompany();
	QString manufacturer = adapter->getManufacturer();
	QString version = adapter->getVersion();


//	QVBoxLayout *infoLayout = new QVBoxLayout(adapterDialog->groupBoxInfo->layout());
	//GridLayout *infoGrid = new QGridLayout(adapterDialog->groupBoxInfo->layout());


	if ( !address.isNull() )
		adapterDialog->macLabel->setText( i18n( "MAC Address: <b>%1</b>" ).arg(address) );

	if ( !version.isNull() )
		adapterDialog->verLabel->setText( i18n( "Version: <b>%1</b>" ).arg(version) );

}

void AdapterConfig::slotChangeTimeout(int timeout)
{
	
	if (adapterDialog->sliderTimeout->value() != timeout) {
		// Disconnect signal of valueChanged! D-Bus signal get emmited immeditaly .. and would trigger a endless loop of signals!
		disconnect(adapterDialog->sliderTimeout, SIGNAL(valueChanged(int)), this, SLOT(slotChangeAdapterTimeout(int)));
		if (timeout == 0) 
			adapterDialog->sliderTimeout->setValue(1860);
		else
			adapterDialog->sliderTimeout->setValue(timeout);
		connect(adapterDialog->sliderTimeout, SIGNAL(valueChanged(int)), this, SLOT(slotChangeAdapterTimeout(int)));
	}

	if (timeout >= 1860 || timeout == 0) {
		adapterDialog->timeout->setText( i18n("none"));
	} else {

		adapterDialog->timeout->setNum(timeout / 60);
		adapterDialog->timeout->setText(adapterDialog->timeout->text().append( i18n(" minutes")));
	}
	

}

void AdapterConfig::slotChangeName(const QString& name)
{
	if (adapterDialog->adapterName->text() != name) {
		//otherwise signal textchanged() from the dialog and slot nameChanged from the adapter will end in a endless loop.
		disconnect(adapterDialog->adapterName, SIGNAL(textChanged(const QString &)), adapter, SLOT(setDeviceName(const QString &)));
		adapterDialog->adapterName->setText(name);
	
		connect(adapterDialog->adapterName, SIGNAL(textChanged(const QString &)), adapter, SLOT(setDeviceName(const QString &)));
	}


}


void AdapterConfig::slotChangeAdapterTimeout(int timeout) 
{
	if (timeout >= 1860)
		adapter->setDiscoverableTimeout(0);
	else
		adapter->setDiscoverableTimeout(timeout);

	kdDebug() << "TimeoutSetTo:__ " << timeout << endl;
	slotChangeTimeout(timeout);
}

void AdapterConfig::slotSetMode(int modenr) {

	switch (modenr) {
		case 0:
		case 1:
			adapterDialog->sliderTimeout->setEnabled(false);
			adapterDialog->timeout->setEnabled(false);
			adapterDialog->textLabel9->setEnabled(false);
			break;
		case 2:
			adapterDialog->sliderTimeout->setEnabled(true);
			adapterDialog->timeout->setEnabled(true);
			adapterDialog->textLabel9->setEnabled(true);
			break;	
	}

	adapter->setMode(modenr);

}	

void AdapterConfig::slotSetClass(const QString & deviceClass) {

	if (deviceClass == "desktop" || deviceClass == "Desktop") {
		adapter->setMinorClass("desktop");
		adapterDialog->deviceClass->setCurrentItem( 1 );
	} else if (deviceClass == "laptop" || deviceClass == "Laptop") {
		adapter->setMinorClass("laptop");
		adapterDialog->deviceClass->setCurrentItem( 2 );
	} else {
		adapter->setMinorClass("uncategorized");
		adapterDialog->deviceClass->setCurrentItem( 0 );
	}
}


#include "adapterconfig.moc"
