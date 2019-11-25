/*
 *
 *  KDE Bluetooth Screen Locker 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluelock.
 *
 *  kbluelock is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluelock is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluelock; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "kbluelock.h"
#include <kdebug.h>


KBlueLock::KBlueLock() : LockDialog()
{	
	dbus = new DBusInit();
	conn = dbus->getDBus();
	manager = new Manager(conn);
	adapter = new Adapter(manager->defaultAdapter(), conn);

	kdDebug() << adapter << endl;
	
	setupNewDevList = new QStringList();

	dcopclient = KApplication::kApplication()->dcopClient();

	statusbar = new QStatusBar(this);	
	LockDialogLayout->addWidget(statusbar);
	statusbar->setSizeGripEnabled(false);
	statusbar->setSizePolicy(QSizePolicy( (QSizePolicy::Minimum), (QSizePolicy::Fixed), 0, 0));
	statusbar->adjustSize();

	setupTray();
	
	connect(okButton, SIGNAL(clicked()), this, SLOT(okSlot()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));
	connect(enableCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxSlot(bool)));


	connect(searchButton, SIGNAL(clicked()), this, SLOT(slotSearch()));

	connect(adapter, SIGNAL(discoveryCompleted()), this, SLOT(slotEnableSearch()));

	
	searchButton->setEnabled(false);
	searchButton->setText("&Search");
	deviceListView->header()->hide();	

	size = 0;
	unlock = 0;
	locked = 0;

	show();

}

KBlueLock::~KBlueLock() {
	if (adapter->isPeriodicDiscovery() && selfstart)
		adapter->stopPeriodicDiscovery();
	adapter->cancelDiscovery();
	close();
	KUniqueApplication::kApplication()->quit();
	

}
void KBlueLock::quitSysTray() {
	if (adapter->isPeriodicDiscovery() && selfstart)
		adapter->stopPeriodicDiscovery();
	adapter->cancelDiscovery();
	close();
	KUniqueApplication::kApplication()->quit();
}


void KBlueLock::checkBoxSlot(bool set) {

	if (set) {

		unlockCheckBox->setEnabled(true);
		deviceListView->setColumnWidth(0,deviceListView->columnWidth(0)-size);
		deviceListView->setEnabled(1);
		if (adapter->isPeriodicDiscovery()) {
			connect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
		} else {	
//                	adapter->startPeriodicDiscovery();
			adapter->discoverDevices();
			selfstart = 1;
			connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));
			
		}

		connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)), this, SLOT(insertDev(const QString&,int,  short)));
		connect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(updateDevName(const QString&, const QString&)));
		searchButton->setText("Searching...");

	} else {
		deviceListView->clear();
		setupNewDevList->clear();
		deviceListView->setEnabled(0);
		if (adapter->isPeriodicDiscovery() && selfstart) {
			adapter->stopPeriodicDiscovery();
			selfstart = 0;
		}
		
		adapter->cancelDiscovery();

		disconnect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)), this, SLOT(insertDev(const QString&,int,  short)));
		disconnect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(updateDevName(const QString&, const QString&)));
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
		searchButton->setEnabled(false);
		searchButton->setText("&Search");
		
	}
}
		
void KBlueLock::restartPeriodicDiscovery() {


	kdDebug() << "RestartPeriodicDiscovery" << endl;
	if (!adapter->isPeriodicDiscovery()) {
		adapter->startPeriodicDiscovery();
		selfstart = 1;
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
	}
}


void KBlueLock::insertDev(const QString &addr, int, short) {
	if (setupNewDevList->grep(addr).size() >= 1) 
			return;
	if (adapter->getRemoteMajorClass(addr) == "phone") {
		newdev = new QListViewItem(deviceListView,adapter->getRemoteName(addr),addr);
		deviceListView->insertItem(newdev);
		size = deviceListView->columnWidth(0);
		deviceListView->setColumnWidth(0,size+15);

		setupNewDevList->append(addr);

	}
}

void KBlueLock::updateDevName(const QString& addr, const QString& newname) {
	if (setupNewDevList->isEmpty() || setupNewDevList->grep(addr).size() < 1) 
			return;
	QListViewItem *tmp = deviceListView->findItem(addr,1,Qt::ExactMatch);
	
	if (tmp->text(0) == "") {
		deviceListView->takeItem(tmp);
		newdev = new QListViewItem(deviceListView,newname,addr);
		deviceListView->insertItem(newdev);
	}
}



void KBlueLock::cancelSlot() {

	if (addr == "" || !addr) {
		if (adapter->isPeriodicDiscovery() && selfstart)
    	    	    adapter->stopPeriodicDiscovery();
	    	close();
    		KApplication::kApplication()->quit();
	} else {
		systray->setInactive();
	}
}



void KBlueLock::okSlot() {
	if (enableCheckBox->isChecked()) {
		newdev = deviceListView->selectedItem();
		if (newdev != NULL) {
			if (addr == newdev->text(1)) {
				systray->show();
				systray->setInactive();
				if (unlockCheckBox->isChecked()) {
					connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this,SLOT(activateUnlock(const QString&, int, short)));
					unlock = 1;
				} else {
					disconnect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this,SLOT(activateUnlock(const QString&, int, short)));
					unlock = 0;
				}
				return;
			}
			name = newdev->text(0);
			addr = newdev->text(1);
					
			disableAction->setEnabled(1);
			systray->show();
			systray->setInactive();
			QToolTip::add(systray,( i18n("<p align='center'>KBlueLock <b>ON</b></p>\n<p align='center'><i>%1</i></p>" ).arg(name) ));
			connect(systray, SIGNAL(quitSelected()), SLOT(quitSysTray()));			
	 	 	if (!searchButton->isEnabled())
	 	 	 	connect(adapter, SIGNAL(discoveryCompleted()), SLOT(slotStartPeriodicDisco()));
	 	 	 else
	 	 	 	 slotStartPeriodicDisco();
	 		//adapter->cancelDiscovery();
			 		
		} else {
			QMessageBox::information(this, i18n("KBlueLock - No Device selectetd"), i18n("To enable KBlueLock you have to choose a Device."), QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
                return;
		}

	} else {

		if (adapter->isPeriodicDiscovery() && selfstart)
    	    		adapter->stopPeriodicDiscovery();
		adapter->cancelDiscovery();
		addr = "";
	    	cancelSlot();

	}
}

void KBlueLock::slotStartPeriodicDisco() {

	if (!adapter->isPeriodicDiscovery()) {
		adapter->startPeriodicDiscovery();
		selfstart = 1;
	}
	connect(adapter, SIGNAL(remoteDeviceDisappeared(const QString&)), this, SLOT(activateLock(const QString&)));


	if (unlockCheckBox->isChecked()) {
		connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),
	 	 	 	 this,SLOT(activateUnlock(const QString&, int, short)));
		unlock = 1;

	} else {
		disconnect(adapter, SIGNAL(remoteDeviceFound(const QString &, int, short)),
	 	 	 	 	 this ,SLOT(activateUnlock(const QString&, int, short)));
  	    unlock = 0;
	}
	 disconnect(adapter, SIGNAL(discoveryCompleted()), this, SLOT(slotStartPeriodicDisco()));

}

void KBlueLock::activateLock(const QString& rem_addr) {

	if (rem_addr != addr)
			return;
	else {
		kdDebug() << __func__ << "()" << endl;
		
		dcopclient->send("kdesktop", "KScreensaverIface", "setBlankOnly","0");
		dcopclient->send("kdesktop", "KScreensaverIface", "lock()","");
		locked = 1;

	}

}


void KBlueLock::activateUnlock(const QString& rem_addr,int devclass, short rssi) {


	if (rem_addr != addr)
			return;

	if (locked)
		kdDebug() << __func__ << "()" << "*UNLOCK*" << endl;
		kdDebug() << __func__ << "()" << endl;
		dcopclient->send("kdesktop", "KScreensaverIface", "quit()","");
		locked = 0;

}


void KBlueLock::disableActionSlot() {


	disconnect(adapter, SIGNAL(remoteDeviceDisappeared(const QString&)), this, SLOT(activateLock(const QString&)));
	on = 0;

	if (unlock)
		disconnect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this,SLOT(activateUnlock(const QString&, int, short)));
		
	if (adapter->isPeriodicDiscovery() && selfstart)
    	  	adapter->stopPeriodicDiscovery();

	enableAction->setEnabled(1);
	disableAction->setEnabled(0);
	QToolTip::remove(systray);
	QToolTip::add(systray,( i18n( "<p align='center'>KBlueLock <b>OFF</b></p>\n<p align='center'><i>%1</i></p>" ).arg(name) ));
}

void KBlueLock::enableActionSlot() {

	if (!adapter->isPeriodicDiscovery()) {
    		adapter->startPeriodicDiscovery();
		selfstart=1;
	}

	
	connect(adapter, SIGNAL(remoteDeviceDisappeared(const QString&)), this, SLOT(activateLock(const QString&)));
	on = 1;
	if (unlock)
		connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)),this,SLOT(activateUnlock(const QString&, int, short)));

	enableAction->setEnabled(0);
	disableAction->setEnabled(1);
	QToolTip::remove(systray);
	QToolTip::add(systray,( i18n( "<p align='center'>KBlueLock <b>ON</b></p>\n<p align='center'><i>%1</i></p>" ).arg(name) ));

}

void KBlueLock::slotSearch() {

	adapter->discoverDevices();
	searchButton->setText("Searching...");
	searchButton->setEnabled(false);
	connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));

}

void KBlueLock::slotEnableSearch() {

	searchButton->setText("&Search");
        searchButton->setEnabled(true);
	statusbar->clear();

}

void KBlueLock::setupTray() {

	systray = new KSystemTray(this);
    	helpMenu = new KHelpMenu(systray, KApplication::kApplication()->aboutData());
	KPixmap logoPixmap = KGlobal::iconLoader()->loadIcon("kbluelock", KIcon::Small, 22);
//	KPixmap logoPixmap(QPixmap("kbluelock.png"));

	systray->setPixmap(logoPixmap);

    	KAction* aboutAction = KStdAction::aboutApp(this,
 	    SLOT(slotAbout()), systray->actionCollection());
    	aboutAction->plug(systray->contextMenu());



	disableAction = new KAction(i18n("&Disable"),
        KGlobal::iconLoader()->loadIcon("button_cancel", KIcon::Small, 16),
        KShortcut::null(), systray, "disable");
    	connect(disableAction, SIGNAL(activated()), this, SLOT(disableActionSlot()));
    	disableAction->plug(systray->contextMenu());

	enableAction = new KAction(i18n("&Enable"),
        KGlobal::iconLoader()->loadIcon("button_ok", KIcon::Small, 16),
        KShortcut::null(), systray, "enable");
    	connect(enableAction, SIGNAL(activated()), this, SLOT(enableActionSlot()));
    	enableAction->plug(systray->contextMenu());

	disableAction->setEnabled(0);
	enableAction->setEnabled(0);

}

void KBlueLock::setDiscoverStatusOn() {

	statusbar->message(i18n("Device Discovery started"),3000);
	disconnect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));

}

#include "kbluelock.moc"

