/*
 *
 *  Dialogs for kbluetooth Input Devices 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluetooth.
 *
 *  kbluetooth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluetooth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluetooth; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "inputwizard.h"
#include <kdebug.h>
#define HID "00001124-0000-1000-8000-00805F9B34FB"
#define PNP "00001200-0000-1000-8000-00805F9B34FB"
#define HEADSET "00001108-0000-1000-8000-00805F9B34FB"

InputWizard::InputWizard() : InputDialog()
{	
	dbus = new DBusInit();

//	dbus1 = new QDBusConnection("dbus");

	conn = dbus->getDBus();
	manager = new Manager(conn);

	kdDebug() << k_funcinfo << endl;
	kdDebug() << "ADD_BUTTON:  "<<  addButton->isOn() << endl;
	
	adapter = new Adapter(manager->defaultAdapter(), conn);
		kdDebug() << manager->defaultAdapter() << endl;
//	inputdlg = new InputDialog();
	inputdlg_ext = new InputDialog_Ext(this);

	setExtension(inputdlg_ext->newDevFrame);
	setOrientation(Qt::Vertical);

	mainlogo =  KGlobal::iconLoader()->loadIcon("kdebluetooth", KIcon::Small, 16);
	setIcon(mainlogo);

	size1 = 0;
	size2 = 0;

	busid = manager->activateService("input");
	inman = new InputManager(conn,busid);
	
	pix = KGlobal::iconLoader()->loadIcon(
        "kbluetooth", KIcon::Small, 48);
	pixmapLabel->setPixmap(pix);

	logoBlend = pix;
    	KPixmapEffect::blend(logoBlend,-1,red,KPixmapEffect::DiagonalGradient);

	setupNewDevList = new QStringList();
	inputList = inman->listDevices();

	deviceBox->header()->hide();

	for (it = inputList.begin(); it != inputList.end(); ++it) {
		kdDebug() << "device path: " << *it << endl;

		newdev = new InputDevice(conn, busid, *it);
		kdDebug() << newdev->getAddress() << endl;
		kdDebug() << adapter->getPath() << endl;
		QListViewItem *toAddDev = new QListViewItem(deviceBox);
		toAddDev->setText(1,adapter->getRemoteName(newdev->getAddress()));
		QString iconName =  DeviceClassMimeConverter::classToIconName(adapter->getRemoteClass(newdev->getAddress()));
	        KPixmap pix2 = KGlobal::iconLoader()->loadIcon(iconName, KIcon::Small, 16);
        	toAddDev->setPixmap(2,pix2);

	        if (newdev->isConnected()) {	
			KPixmap pix3 = KGlobal::iconLoader()->loadIcon(
        		"bookmark", KIcon::Small, 16);
        		toAddDev->setPixmap(0,pix3);
		}

		deviceBox->insertItem(toAddDev);



		kdDebug() << adapter->getRemoteName(newdev->getAddress()) << endl;
	}
	
	int colSize = deviceBox->columnWidth(1);
	deviceBox->setColumnWidth(1,colSize+20);


	/* disable button by default - by default nothing is selected -> nothing to do */
	connectButton->setEnabled(false);
	deleteButton->setEnabled(false);

	inputdlg_ext->newdevList->header()->hide();
	inputdlg_ext->newdevList->setColumnAlignment(2,Qt::AlignVCenter);

	connect(addButton, SIGNAL(toggled(bool)), this, SLOT(showExtension(bool)));
	connect(addButton, SIGNAL(clicked()), SLOT(configNew()));


	connect(inputdlg_ext->setupButton, SIGNAL(clicked()), this, SLOT(setupNewDev()));
	connect(inputdlg_ext->searchButton, SIGNAL(clicked()), this, SLOT(slotSearch()));

	connect(adapter, SIGNAL(discoveryCompleted()), this, SLOT(slotEnableSearch()));

	connect(deviceBox, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotChangeButton(QListViewItem*)));
	connect(inputdlg_ext->newdevList, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotChangeSetupButton(QListViewItem*)));
	connect(connectButton, SIGNAL(clicked()), this, SLOT(connectdev()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDev()));

	connect(adapter, SIGNAL(remoteDeviceConnected(const QString&)), this, SLOT(setConnectedIcon(const QString&)));
	connect(adapter, SIGNAL(remoteDeviceDisconnected(const QString&)), this, SLOT(setDisconnectedIcon(const QString&)));

	chgLogoTimer = new QTimer(this);	
	chgLogoTimer->start(50);
	timer = 0;
	
}

InputWizard::~InputWizard() {
	if (adapter->isPeriodicDiscovery() && selfstart)
		adapter->stopPeriodicDiscovery();
	adapter->cancelDiscovery();

}


void InputWizard::connectdev() {

	kdDebug() << "button:" << connectButton->text() << endl;
	if (connectButton->text() == "&Disconnect") {
		disconnectdev();
		return;
	}

	busid = manager->activateService("input");
	InputDevice *tmp;
	QListViewItem *sel = deviceBox->selectedItem();

	/* No input device select */
	if (!sel)
		return;

	QString selName = sel->text(1);
	inputList = inman->listDevices();
	for (it = inputList.begin();it != inputList.end(); ++it) {
		tmp = new InputDevice(conn,busid,*it);
		QString name = adapter->getRemoteName(tmp->getAddress());
		kdDebug() << "Devicename: " << name << endl;
		kdDebug() << "Selname: " << selName << endl;
		kdDebug() << "IT: " << *it << endl;
		
		if (name == selName) { 
//			tmp->connectBlock();
			tmp->connect();
//			connect(tmp, SIGNAL(connected()), this, SLOT(connectMsg()));
//			connectMsg(connectnow,name);
//			return;
		} else 
			continue;
	}

}

void InputWizard::disconnectdev() {

//	if (connectButton->text() == "&connect")
//		return;

	QListViewItem *sel = deviceBox->selectedItem();

	/* No input device select */
	if (!sel)
		return;

	QString selName = sel->text(1);
	inputList = inman->listDevices();
	for (it = inputList.begin();it != inputList.end(); ++it) {
		InputDevice *tmp = new InputDevice(conn,busid,*it);
		QString name = adapter->getRemoteName(tmp->getAddress());
		kdDebug() << "Devicename" << name << endl;
		if (name == selName) { 
			tmp->disconnect();
			disconnectMsg(tmp->isConnected(),name); 
			return;
			
		} else 
			continue;
		
	}

}
void InputWizard::deleteDev() {

 	QListViewItem *sel = deviceBox->selectedItem();

	/* No input device select */
	if (!sel)
		return;

	inputList = inman->listDevices();
//	busid = manager->activateService("input");
	for (it = inputList.begin();it != inputList.end(); ++it) {
		InputDevice *tmp = new InputDevice(conn,busid,*it);
		QString name = adapter->getRemoteName(tmp->getAddress());
		if (name == sel->text(1)) {
			inman->removeDevice(*it);
			deviceBox->takeItem(sel);
		}
	}
}

void InputWizard::slotChangeButton(QListViewItem *dev) {
	
	if(dev == NULL) {
		connectButton->setEnabled(false);
		deleteButton->setEnabled(false);
		return;
	}

	connectButton->setEnabled(true);
	deleteButton->setEnabled(true);

	QString name = dev->text(1);
	inputList = inman->listDevices();
	for (it = inputList.begin();it != inputList.end(); ++it) {
		newdev = new InputDevice(conn, busid, *it);
		kdDebug() << "Devic-connect: " << newdev->isConnected() << endl;
		if (adapter->getRemoteName(newdev->getAddress()) == name) {
			if (newdev->isConnected()) {
				connectButton->setText(i18n("&Disconnect"));
			} else if (connectButton->text() != "&Connect") {
				connectButton->setText(i18n("&Connect"));
			}
		}
	}				
}


void InputWizard::slotChangeSetupButton(QListViewItem *dev) {

	if(dev == NULL)
		inputdlg_ext->setupButton->setEnabled(false);
	else
		inputdlg_ext->setupButton->setEnabled(true);
		
}

void InputWizard::configNew() {
	inputdlg_ext->newdevList->clear();
	inputdlg_ext->setupButton->setEnabled(false);
	inputdlg_ext->searchButton->setEnabled(false);
	inputdlg_ext->searchButton->setText("Searching...");
	inputdlg_ext->newdevList->setColumnWidth(0,inputdlg_ext->newdevList->columnWidth(0)-size1);
	inputdlg_ext->newdevList->setColumnWidth(1,inputdlg_ext->newdevList->columnWidth(1)-size2);
	if (setupNewDevList->size() > 0) 
		setupNewDevList->clear();
	
	if (addButton->isOn()) {
		addButton->setText(i18n("&Add New Device <<"));
		if (!adapter->isPeriodicDiscovery()) { 
		//	adapter->startPeriodicDiscovery();
			adapter->discoverDevices();
			selfstart = 1;
			connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));
		} else {
			connect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
		}
		connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this, SLOT(insertDev(const QString&, int, short)));
		connect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(updateDevName(const QString&, const QString&)));
		connect(chgLogoTimer,SIGNAL(timeout()),this,SLOT(updateLogo()));
	 	
	} else { 
		addButton->setText(i18n("&Add New Device >>"));
		adapter->cancelDiscovery();
		if (adapter->isPeriodicDiscovery() && selfstart) {
			adapter->stopPeriodicDiscovery();
			selfstart = 0;
		}
		
		disconnect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this, SLOT(insertDev(const QString&, int, short)));
		disconnect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(updateDevName(const QString&, const QString&)));
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
	}
		
	

}

void InputWizard::insertDev(const QString& addr, int /*devclass*/, short /*rssi*/) {
	if (setupNewDevList->grep(addr).size() == 1 )
		return;
	inputList = inman->listDevices();
	for (it = inputList.begin();it != inputList.end(); ++it) {
		newdev = new InputDevice(conn, busid, *it);
		kdDebug() << "Device-connect: " << newdev->isConnected() << endl;
		if (newdev->getAddress() == addr) 
			return;
	}
	QString name = adapter->getRemoteName(addr);
	if (adapter->getRemoteMajorClass(addr) == "peripheral") {
			
			fillNewDevList(addr,name);

	} else if (adapter->getRemoteMajorClass(addr) == "audio/video") {

			fillNewDevList(addr,name);

	} else if(adapter->getRemoteMajorClass(addr) == "computer" )  {
			return;
		
	} else if( adapter->getRemoteMajorClass(addr) == "phone"){

		KBluetooth::Adapter::ServiceHandleList dev_serves;
		KBluetooth::Adapter::ServiceHandleList::iterator each;

		dev_serves = adapter->getRemoteServiceHandles(addr, HID);

		if (!dev_serves.empty()) {
			fillNewDevList(addr,name);
		}
	}
}


void InputWizard::fillNewDevList(const QString& addr, QString& name) {

	QListViewItem *newdev = new QListViewItem(inputdlg_ext->newdevList,name, addr);

	QString iconName =  DeviceClassMimeConverter::classToIconName(adapter->getRemoteClass(addr));
	KPixmap pix2 = KGlobal::iconLoader()->loadIcon(
        iconName, KIcon::Small, 32);
	newdev->setPixmap(2,pix2);	

        inputdlg_ext->newdevList->insertItem(newdev);

	size1 = inputdlg_ext->newdevList->columnWidth(0);
	inputdlg_ext->newdevList->setColumnWidth(0,size1+15);

	size2 = inputdlg_ext->newdevList->columnWidth(1);
	inputdlg_ext->newdevList->setColumnWidth(1,size2+15);

        setupNewDevList->append(addr);
}


void InputWizard::updateDevName(const QString& addr, const QString& newname) {
        if (setupNewDevList->isEmpty() || setupNewDevList->grep(addr).size() < 1)
                        return;
        QListViewItem *tmp = inputdlg_ext->newdevList->findItem(addr,1,Qt::ExactMatch);

        if (tmp->text(0) == "") {
		const QPixmap* ico = tmp->pixmap(2);
                inputdlg_ext->newdevList->takeItem(tmp);
                QListViewItem *newdev = new QListViewItem(inputdlg_ext->newdevList,newname,addr);
		newdev->setPixmap(2,*ico);

                inputdlg_ext->newdevList->insertItem(newdev);
        }
}



void InputWizard::closeDialog() {
	if (adapter->isPeriodicDiscovery() && selfstart) 
		adapter->stopPeriodicDiscovery();
	adapter->cancelDiscovery();
	
	close();
	KApplication::kApplication()->quit();
}


void InputWizard::setupNewDev() {

	QListViewItem *confSel = inputdlg_ext->newdevList->currentItem();
	if (!confSel)
		QMessageBox::information(this, i18n("Creating Input Device"), i18n("You have to select a remote Device to setup!"), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
	else {
		
		SelectAddr = confSel->text(1);
		kdDebug() << "address: _____________" << SelectAddr << endl; 
		adapter->stopPeriodicDiscovery();
		adapter->cancelDiscovery();
		if (adapter->getRemoteMinorClass(SelectAddr) == "keyboard") {
			if (adapter->hasBonding(SelectAddr)) {
				adapter->removeBonding(SelectAddr);
			}
			adapter->createBondingBlock(SelectAddr);
/*
				QMessageBox::information(this, i18n("Pairing with Device failed"), i18n("Failed to pair with the input device %1!").arg(adapter->getRemoteName(SelectAddr)), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
                        return;
*/
		}
//		connect(inman, SIGNAL(deviceCreated(const QString&)), this, SLOT(setupNewDevCallback(const QString&)));
		QString remoteConn = inman->createDeviceBlock(SelectAddr);
		
		kdDebug() << "remoteConn: _____________" << remoteConn << endl; 

		if (remoteConn == "") {
		
			QMessageBox::information(this, i18n("Creating Input Device failed"), i18n("Failed to create the input device for %1!").arg(adapter->getRemoteName(SelectAddr)), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
			return;

		} else {
			QListViewItem* newDeviceBoxItem = new QListViewItem(deviceBox);
			newDeviceBoxItem->setText(1,confSel->text(0));

			QString iconName =  DeviceClassMimeConverter::classToIconName(adapter->getRemoteClass(SelectAddr));
			KPixmap pix4 = KGlobal::iconLoader()->loadIcon(
		        iconName, KIcon::Small, 32);
			newDeviceBoxItem->setPixmap(2,pix4);	

			deviceBox->insertItem(newDeviceBoxItem);
			inputdlg_ext->newdevList->takeItem(confSel);
			addButton->setOn(0);
			configNew();
			setupNewDevCallback(remoteConn);
		}
	}

}

void InputWizard::setupNewDevCallback(const QString &remoteConn) {

	InputDevice *tmp;

	kdDebug() << "REMOTECONNECTION:  " << remoteConn << endl;
        if (!remoteConn.isNull() ) {

//		busid = manager->activateService("input");
                tmp = new InputDevice(conn,busid,remoteConn);

        } else {
	        kdDebug() << "LISTDEVICE:  " << inman->listDevices() << endl;
        	inputList = inman->listDevices();
        	for (it = inputList.begin();it != inputList.end(); ++it) {
//			busid = manager->activateService("input");
                	tmp = new InputDevice(conn,busid,*it);
                	if (tmp->getAddress() != SelectAddr)
                        	continue;
		}
	
	}
	int connectit = QMessageBox::question( this, i18n("Connect now?"),
	i18n("Would you like to connect to %1 now?").arg(adapter->getRemoteName(SelectAddr)),
	QMessageBox::Yes|QMessageBox::Default,
	QMessageBox::No|QMessageBox::Escape,
	QMessageBox::NoButton);
	if (connectit == QMessageBox::Yes) { 
//	       	bool connectnow = tmp->connectBlock();
	       	tmp->connect();
//		connect(tmp, SIGNAL(connected()), this, SLOT(connectMsg(bool, QString)));
//		connectMsg(connectnow,adapter->getRemoteName(SelectAddr));
	}
        kdDebug() << "SETUP---READY:  " << adapter->getRemoteName(SelectAddr) << endl;

}


void InputWizard::restartPeriodicDiscovery() {


	kdDebug() << "RestartPeriodicDiscovery" << endl;
	if (!adapter->isPeriodicDiscovery()) {
		adapter->startPeriodicDiscovery();
		selfstart = 1;
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
	}
}
	
void InputWizard::connectMsg(bool connect, QString name) {

	if (connect) {
		QMessageBox::information(this, i18n("Connection succesfull"), i18n("Connection to %1 successful established!").arg(name), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
		connectButton->setText(i18n("&Disconnect"));
		
//		disconnect(tmp, SIGNAL(connected()), this, SLOT(connectMsg(bool, QString)));
	} else {
		QMessageBox::information(this, i18n("Connection failed"), i18n("Could not connect to ") + name + i18n("\n Check if the device is available and bluetooth enabled."), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
		connectButton->setText(i18n("&Connect"));

	}
}

void InputWizard::disconnectMsg(bool connect, QString name) {
	if (!connect) {
	QMessageBox::information(this, i18n("Disconnect succesfull"), i18n("Connection to ") + name + i18n(" successful disconnected!"), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
			connectButton->setText(i18n("&Connect"));
	} else {
		QMessageBox::information(this, i18n("Disconnect failed"), i18n("Connection to ") + name + i18n(" could not be disconnected!"), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
		connectButton->setText(i18n("&Disconnect"));
	}
}

void InputWizard::slotSearch() {

	adapter->discoverDevices();
	inputdlg_ext->searchButton->setText("Searching...");
	inputdlg_ext->searchButton->setEnabled(false);
	connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));
	connect(chgLogoTimer,SIGNAL(timeout()),this,SLOT(updateLogo()));

}

void InputWizard::slotEnableSearch() {


	inputdlg_ext->searchButton->setText("S&earch");
        inputdlg_ext->searchButton->setEnabled(true);
	inputdlg_ext->statusbar->setText("");
	pixmapLabel->setPixmap(pix);
	disconnect(chgLogoTimer,SIGNAL(timeout()),this,SLOT(updateLogo()));

}


void InputWizard::setConnectedIcon(const QString& addr) {

	QListViewItem* found = deviceBox->findItem(adapter->getRemoteName(addr),1,Qt::ExactMatch);		
	if (!found)
		return;
	 KPixmap pix = KGlobal::iconLoader()->loadIcon(
	"bookmark", KIcon::Small, 16);
	 found->setPixmap(0,pix);
			
}

void InputWizard::setDisconnectedIcon(const QString& addr) {

	QListViewItem* found = deviceBox->findItem(adapter->getRemoteName(addr),1,Qt::ExactMatch);		
	if (!found)
		return;
	found->setPixmap(0,NULL);
	kdDebug() << "setDisconnectedIcon()" << endl;
			
}

void InputWizard::setDiscoverStatusOn() {

	inputdlg_ext->statusbar->setText(i18n("Device Discovery started"));
	disconnect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));

}

void InputWizard::updateLogo() {

	if (timer == 0) {
		pixmapLabel->setPixmap(logoBlend);
		timer++;
	} else {
		pixmapLabel->setPixmap(pix);
		timer=0;
	}

}




#include "inputwizard.moc"

