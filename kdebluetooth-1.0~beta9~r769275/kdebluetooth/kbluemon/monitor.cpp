/*
 *
 *  KDE Blutooth Monitor 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluemon.
 *
 *  kbluemon is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluemon is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluemon; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include "monitor.h"
#include <kdialogbase.h>
#include <qlistview.h>
#include <qdom.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <string>
#include <dbus/dbus.h>



Monitor::Monitor() : MonDialog()
{
	KBluetooth::DBusInit *dbus = new KBluetooth::DBusInit();
	conn = dbus->getDBus();
	manager = new KBluetooth::Manager(conn);
	adapter = new KBluetooth::Adapter(manager->defaultAdapter(), conn);
	

	mainlogo =  KGlobal::iconLoader()->loadIcon("kdebluetooth", KIcon::Small, 16);
	setIcon(mainlogo);

	statusbar = new QStatusBar(this);	
	MonDialogLayout->addWidget(statusbar);
	statusbar->setSizeGripEnabled(false);
	statusbar->setSizePolicy(QSizePolicy( (QSizePolicy::Minimum), (QSizePolicy::Fixed), 0, 0));
	statusbar->adjustSize();

	if (!adapter->isPeriodicDiscovery()) {
		adapter->discoverDevices();
		selfstart = 1;
		stopButton->setText( i18n("Scanning..."));
		stopButton->setEnabled(false);
		connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));
	
	}
	if (!adapter->getPeriodicDiscoveryNameResolving())
		adapter->setPeriodicDiscoveryNameResolving(1);
	logo =  KGlobal::iconLoader()->loadIcon("kbluetooth", KIcon::Small, 32);
	logoBlend = logo;
    	KPixmapEffect::blend(logoBlend,-1,red,KPixmapEffect::DiagonalGradient);
	pixLabel->setPixmap(logo);
	
	chgLogoTimer = new QTimer(this);	
	chgLogoTimer->start(50);
	timer = 0;



	connection();

	connect(adapter, SIGNAL(discoveryCompleted()),this, SLOT(enableStartSearch()));

	connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDisco()));
	connect(listView1, SIGNAL(clicked(QListViewItem*)), this, SLOT(chg_details(QListViewItem*)));
	connect(periodicCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotPeriodicScan(bool)));
	connect(sdpCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotSdpDiscovery(bool)));
	
	rx.setPattern("^\\d+");
	rx.setMinimal(TRUE);

	listView1->hideColumn(3);

	serviceDiscovery = 0;
	
	listView1->header()->hide();

	adjustSize();
	
		
	
}

Monitor::~Monitor() {

	kdDebug() << k_funcinfo << endl;
	if (adapter->isPeriodicDiscovery() && selfstart)
		adapter->stopPeriodicDiscovery();
}




void Monitor::newdev(const QString& name, int dev_class, short rssi) {

	kdDebug() << k_funcinfo << endl;

	rssi = 100.0/(128+20) * (rssi + 128);
	QString msg;
	QString signal = QString::number(rssi) + "%";
	QString devcl = QString::number(dev_class);
	for(it = deviceList.begin(); it!= deviceList.end(); ++it) {
		if ((**it).addr == name) {
			
			kdDebug() << k_funcinfo << "Version: " << (**it).version << endl;
			kdDebug() << k_funcinfo << "GetVersion: " << adapter->getRemoteVersion(name) << endl;
			kdDebug() << k_funcinfo << "Services.at(0): " << (**it).services.at(0) << endl;
			(**it).rssi = rssi;
			QListViewItem *tmp = listView1->findItem((**it).name,0,Qt::ExactMatch);
			tmp->setText(1,signal);
						
			if ((**it).version == "<i>not available</i>" || (**it).version == "") {

				getDetails(*it,name);	

			}
			if (dev_addr->text() == (**it).addr) {
				strength->setProgress(rssi);
				if ((**it).version == "<i>not available</i>" || (**it).version == "")
					chg_details(tmp);	
			}
			if (serviceDiscovery && ((**it).services.at(0) == "Service Discovery disabled") && ((**it).services.at(0) != "no Services discovered")) 
				getServices(*it);
			
			return;
		}
	}

	RemoteDevice* newDev = createDevice(name,rssi);
	if (serviceDiscovery) {
		kdDebug() << "START SERVICE DISCO" << endl;
		getServices(newDev);

	} else { 
		newDev->services.push_back( i18n ("Service Discovery disabled"));
		newDev->devInfo = i18n("<i>not available</i>");
	}
		
	
	if (deviceList.empty()) {
		strength->setProgress(rssi);
		dev_addr->setText(name);
		dev_version->setText(newDev->version);
		dev_revision->setText(newDev->revision);
		dev_manufacturer->setText(newDev->manufacturer);
		dev_category->setText(QString(newDev->dev_class + ", " + newDev->major_class));
		dev_Info->setText(newDev->devInfo);
		for (newDev->serv_it = newDev->services.begin(); newDev->serv_it!=newDev->services.end(); ++newDev->serv_it) {
			kdDebug() << k_funcinfo << *newDev->serv_it << endl;
			service_list->insertItem(*newDev->serv_it);
		}

	}

	deviceList.push_back(newDev);
	
	QListViewItem *dev = new QListViewItem(listView1, name, signal, newDev->major_class,name);

	QString iconName =  KBluetooth::DeviceClassMimeConverter::classToIconName(adapter->getRemoteClass(name));
	KPixmap pix = KGlobal::iconLoader()->loadIcon(
        iconName, KIcon::Small, 16);
	dev->setPixmap(4,pix);
	
	listView1->insertItem(dev);
	listView1->hideColumn(3);	
	
}

RemoteDevice* Monitor::createDevice(const QString& addr, short rssi ) {

	QString device_class(adapter->getRemoteMinorClass(addr));
	RemoteDevice *add = new RemoteDevice(addr,rssi,device_class);
	add->name = addr;
	getDetails(add,addr);

return add;


}

void Monitor::getDetails(RemoteDevice* dev,const QString& addr) {

	dev->version = adapter->getRemoteVersion(addr);
	if (dev->version == "")
		dev->version = i18n("<i>not available</i>");
	dev->manufacturer = adapter->getRemoteManufacturer(addr);
	if (dev->manufacturer == "")
		dev->manufacturer = i18n("<i>not available</i>");
	dev->revision = adapter->getRemoteRevision(addr);
	if (dev->revision == "")
		dev->revision = i18n("<i>not available</i>");
	dev->major_class = adapter->getRemoteMajorClass(addr);

}


void Monitor::getServices(RemoteDevice* add) {

	KBluetooth::Adapter::ServiceHandleList dev_serves;
	KBluetooth::Adapter::ServiceHandleList::iterator each;
	
	dev_serves = adapter->getRemoteServiceHandles(add->addr, "");
	add->services.clear();
	
	if (add->major_class == "computer" || add->major_class == "") {
		add->services.push_back( i18n ("no Services discovered"));
		add->devInfo = i18n("<i>not available</i>");
		return;
	}

	if (!dev_serves.empty()) {	

	
		for (each = dev_serves.begin(); each!= dev_serves.end(); ++each) {

		
			KBluetooth::ServiceRecord record = adapter->getRemoteServiceRecord(add->addr, *each);
			if ((rx.search( record.name() ) < 0) && (record.name() != "")) 
				add->services.push_back( record.name() );

			/* since bluez-libs 3.14 not needed */
/*			
			if ( record.servId().grep("0x1124").size() >= 1) 
				
				add->services.push_back( "Human Interface Device" );
*/
			if (add->devInfo == "<i>not available</i>" && record.typ() != "")
				add->devInfo = record.typ();
			  
				
		}	
	} else {
		add->services.push_back( i18n ("no Services discovered"));
//		kdDebug() << adapter->getError() << endl << endl;;
	}

	if (!add->devInfo || add->devInfo == "" )
		add->devInfo = i18n("<i>not available</i>");


}


void Monitor::chg_name(const QString& add, const QString& name) {

	kdDebug() << k_funcinfo << endl;
	if (deviceList.empty()) {
		return;
	}
	for(it = deviceList.begin(); it!= deviceList.end(); ++it) {
		if ((**it).getAddr() == add) {
			if ((**it).name == name) {
				return;
			} else {
				if ((**it).getAddr() == dev_addr->text()) {
					dev_name->setText(name);
				}
				(**it).name = name;
				QListViewItem *tmp = listView1->findItem(add,0,Qt::ExactMatch);
				tmp->setText(0,name);
			}
		break;
		}
	}
}


void Monitor::deldev(const QString& add) {
	
	kdDebug() << k_funcinfo << endl;
	for(it = deviceList.begin(); it!= deviceList.end(); ++it) {
	        if ((**it).getAddr() == add) {
			QListViewItem *tmp = listView1->findItem((**it).name,0,Qt::ExactMatch);
			listView1->takeItem(tmp);
			if (deviceList.size() == 1) {
                              deviceList.clear();
                                dev_addr->setText("");
                                dev_name->setText("");
				dev_Info->setText("");
                                strength->setProgress(0);
                                dev_version->setText("");
                                dev_revision->setText("");
                                dev_manufacturer->setText("");
                                dev_category->setText("");
                                service_list->clear();
                        } else {
				if (deviceList.last() == *it) {
					deviceList.pop_back();
                                	*it = deviceList.first();
                                	tmp = listView1->findItem((**it).name,0,Qt::ExactMatch);
				} else {
                                	Monitor::RemoteDeviceList::iterator it2 = deviceList.erase(it);
                                	tmp = listView1->findItem((**it2).name,0,Qt::ExactMatch);
				}
					
	                       	listView1->setSelected(tmp,true);
                                chg_details(tmp);

                        }
		break;
			
		}

	}
}

void Monitor::stopDisco() {
	
	kdDebug() << k_funcinfo << endl;

	

		if (!adapter->isPeriodicDiscovery()) {
			adapter->discoverDevices();
			connection();
			stopButton->setText( i18n("Scanning..."));
			stopButton->setEnabled(false);
			connect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));

		} else if (adapter->isPeriodicDiscovery() && !selfstart && stopButton->text() == "St&art Scan") { 
			connection();
			connect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));	
			stopButton->setText( i18n("St&op Scan"));
			
			
		} else if (adapter->isPeriodicDiscovery() && !selfstart && stopButton->text() == "St&op Scan") { 

			disconnection();	
			stopButton->setText( i18n("St&art Scan"));
			
			pixLabel->setPixmap(logo);
			
		}
	
}

void Monitor::chg_details(QListViewItem* sel) {

	kdDebug() << k_funcinfo << endl;
	if (!sel)
		return;
	
	for(it = deviceList.begin(); it!= deviceList.end(); ++it) {
		if (((**it).name == sel->text(0)) && ((**it).getAddr() == sel->text(3))) {
			
//			kdDebug() << k_funcinfo << *it << endl;
			dev_addr->setText((**it).addr);
			dev_name->setText((**it).name);
			strength->setProgress((**it).rssi);
			dev_version->setText((**it).version);
			dev_revision->setText((**it).revision);
			dev_manufacturer->setText((**it).manufacturer);
			dev_category->setText(QString((**it).dev_class + ", " + (**it).major_class));
			dev_Info->setText((**it).devInfo);
			service_list->clear();
			for ((**it).serv_it = (**it).services.begin(); (**it).serv_it!=(**it).services.end(); ++(**it).serv_it) {
			
				service_list->insertItem(*(**it).serv_it);
			
			}

//		break;
		}

	}
		
}
void Monitor::restartPeriodicDiscovery() {


	kdDebug() << "RestartPeriodicDiscovery" << endl;
	if (!adapter->isPeriodicDiscovery()) {
		adapter->startPeriodicDiscovery();
		selfstart = 1;
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
	}
}


void Monitor::updateLogo() {

	if (timer == 0) {
		pixLabel->setPixmap(logoBlend);
		timer++;
	} else {
		pixLabel->setPixmap(logo);
		timer=0;
	}

}

void Monitor::slotPeriodicScan(bool on) {


	if (on) {
		if (!adapter->isPeriodicDiscovery()) {
			bool ps = adapter->startPeriodicDiscovery();
			if (ps) {
				statusbar->message(i18n("Periodic Discovery started"),3000);
			} else {
				statusbar->message(i18n("Failed to start Periodic Discovery! (") + adapter->getError() + ")",3000);
				periodicCheckBox->setChecked(0);
				return;
			}
			
			connection();
			selfstart = 1;
			stopButton->setText( i18n("St&op Scan"));			
			stopButton->setText( i18n("Scanning..."));
			stopButton->setEnabled(false);



		} else {
			connection();		
			connect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));
		}
	
		if (!adapter->getPeriodicDiscoveryNameResolving())
			adapter->setPeriodicDiscoveryNameResolving(1);


	} else {

		if (adapter->isPeriodicDiscovery()) {
			adapter->stopPeriodicDiscovery();
			selfstart = 0;

			stopButton->setEnabled(true);
			stopButton->setText( i18n("St&art Scan"));
			disconnection();	

			pixLabel->setPixmap(logo);
			statusbar->clear();
		}
	}
}

void Monitor::enableStartSearch() {


	stopButton->setText("St&art Scan");
        stopButton->setEnabled(true);
	disconnection();
	pixLabel->setPixmap(logo);
	statusbar->clear();

}


void Monitor::connection() {

		connect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)), this, SLOT(newdev(const QString&, int, short)));
		connect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(chg_name(const QString&, const QString&)));
		connect(adapter, SIGNAL(remoteDeviceDisappeared(const QString&)), this, SLOT(deldev(const QString&)));
		connect(chgLogoTimer,SIGNAL(timeout()),this,SLOT(updateLogo()));



}

void Monitor::disconnection() {

		disconnect(adapter, SIGNAL(remoteDeviceFound(const QString&, int, short)), this, SLOT(newdev(const QString&, int, short)));
		disconnect(adapter, SIGNAL(remoteNameUpdated(const QString&, const QString&)), this, SLOT(chg_name(const QString&, const QString&)));
		disconnect(adapter, SIGNAL(remoteDeviceDisappeared(const QString&)), this, SLOT(deldev(const QString&)));
		disconnect(adapter, SIGNAL(periodicDiscoveryStopped()), this, SLOT(restartPeriodicDiscovery()));

		disconnect(chgLogoTimer,SIGNAL(timeout()),this,SLOT(updateLogo()));




}

void Monitor::slotSdpDiscovery(bool val) {


	serviceDiscovery = val;
	kdDebug() << "VALUE of SERVICE DISCO = " << serviceDiscovery << endl;

}

void Monitor::setDiscoverStatusOn(void) {

	statusbar->message(i18n("Device Discovery started"),3000);
	disconnect(adapter, SIGNAL(discoveryStarted(void)), this, SLOT(setDiscoverStatusOn(void)));

}



#include "monitor.moc"
