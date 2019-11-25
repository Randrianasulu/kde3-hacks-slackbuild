/*
 *
 *  KDE Control Center Module for managing Bluetooth linkkeys 
 *
 *  Copyright (C) 2003  Fred Schaettgen <kdebluetooth@schaettgen.de>
 *  Copyright (C) 2006  Daniel Gollub <dgollub@suse.de>
 *
 *
 *  This file is part of kcm_btpaired.
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

#include "pairedtab.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <klistview.h>
#include <qlistview.h>
#include <kurllabel.h>
#include <kglobal.h>
#include <kpixmap.h>
#include <kiconloader.h>
#include <qpushbutton.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kconfig.h>
#include <krun.h>
#include <qtimer.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <qfile.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <kmessagebox.h>
#include <kdirwatch.h>
#include <kapplication.h>
#include <kurlrequester.h>

using namespace std;
using namespace KBluetooth;

struct HcidLinkKeyStruct {
        uint8_t  key[16];
        uint8_t  type;
        time_t   time;
};

PairedTab* PairedTab::_ctx = NULL;

PairedTab::PairedTab(QWidget *parent, const char* name) :
    PairedTabBase(parent, name)
{
    dbus = new DBusInit();
    conn = dbus->getDBus();

    _ctx = this;

    trustedDev = false;
    reloadList();
    updateGUI();

    removePairingButton->setEnabled(false);
    removeTrustButton->setEnabled(false);
    exportButton->setEnabled(false);

    connect(removePairingButton, SIGNAL(clicked()),
	this, SLOT(slotRemovePairing()));
    connect(removeTrustButton, SIGNAL(clicked()),
	this, SLOT(slotRemoveTrust()));
    connect(exportButton, SIGNAL(clicked()),
	this, SLOT(slotExportBonding()));
    connect(pairingListView, SIGNAL(selectionChanged()),
	this, SLOT(slotSelectionChanged()));


    exportBondingDialog = new ExportDialog(this);
            
    /*
    KDialogBase* baseDialog = new KDialogBase(this);
    baseDialog->hide();
    exportBondingDialog = new ExportDialog(baseDialog);

    */

    filter = new DBusFilter(conn);
    
    filter->addFilter((DBusHandleMessageFunction) filterFunction);
    filter->addMatch("type='signal', interface=org.bluez.Manager");
    filter->addMatch("type='signal', interface=org.bluez.Adapter");

    service = NULL;

}

PairedTab::~PairedTab()
{
	delete filter;
	delete service;
}

DBusHandlerResult PairedTab::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void * /*data*/)
{
	const char *member = dbus_message_get_member(msg);

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	kdDebug() << __func__ << ": " << member << endl;

	if (dbus_message_is_signal(msg, INTERFACE_ADAPTER, "BondingRemoved"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_ADAPTER, "BondingCreated"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_ADAPTER, "NameChanged"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_ADAPTER, "MinorClassChanged"))
		_ctx->slotListChanged();
	// ModeChanged is used instead AdapterAdded to make sure the Adapter is activated.
	else if (dbus_message_is_signal(msg, INTERFACE_ADAPTER, "ModeChanged"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_SERVICE, "TrustAdded"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_SERVICE, "TrustRemoved"))
		_ctx->slotListChanged();
	else if (dbus_message_is_signal(msg, INTERFACE_MANAGER, "AdapterRemoved"))
		_ctx->slotListChanged();
	
	return DBUS_HANDLER_RESULT_HANDLED;
}

void PairedTab::reloadList()
{
	kdDebug() << "reloadList()" << endl;
	Manager manager(conn);

	pairingList.clear();

	Manager::AdapterList list = manager.listAdapters();
	Manager::AdapterList::iterator it;
	for (it = list.begin(); it != list.end(); ++it) {

		Adapter a((*it), conn);
		localAddr =  a.getAddress();
		localName = a.getDeviceName();

		getBondingList(a);
		getServiceList(a);
		}

}

void PairedTab::getBondingList(Adapter& a)
{
		Adapter::BondingList bondings = a.listBondings();
		Adapter::BondingList::iterator it;
	
		for (it=bondings.begin(); it != bondings.end(); it++) {

			if ((*it).isEmpty())
				continue;

			const char *remoteAddr = QString((*it)).latin1();
			getDetails(remoteAddr,a);
		}
}

void PairedTab::getServiceList(Adapter& a)
{

	Service* inputService = new Service(conn, "/org/bluez/service_input");
	Service* audioService = new Service(conn, "/org/bluez/service_audio");
	Service* networkService = new Service(conn, "/org/bluez/service_network");
	Service* serialService = new Service(conn, "/org/bluez/service_serial");
	
	ServiceList.push_back(inputService);
	ServiceList.push_back(audioService);
	ServiceList.push_back(networkService);
	ServiceList.push_back(serialService);
	
	vector<Service*>::iterator ServIt;
	for(ServIt = ServiceList.begin(); ServIt != ServiceList.end(); ++ServIt) {

		Service::UserList services = (*ServIt)->listTrusts();
		kdDebug() << "Service name: " << (*ServIt)->getName() << endl;
		
		Service::UserList::iterator it;

		for (it=services.begin(); it != services.end(); it++) {

			if ((*it).isEmpty())
				continue;

			const char *remoteAddr = QString((*it)).latin1();
			kdDebug() << "Service User found: " << (*ServIt)->getName() << remoteAddr << endl;
			getDetails(remoteAddr,a);
		}
	}
}

void PairedTab::getDetails(const char *remoteAddr,Adapter& a)
{
	vector<PairingInfo>::iterator it;
	for (it = pairingList.begin(); it != pairingList.end(); ++it) {
		if (it->remoteAddr == remoteAddr)
			return;
	}

	kdDebug() << "remoteAddr: " << remoteAddr << endl;

	PairingInfo newInfo;
	newInfo.localAddr = localAddr;
	newInfo.localName = localName;

	newInfo.remoteAddr = QString(remoteAddr); 
	newInfo.remoteName = a.getRemoteName(remoteAddr);
	newInfo.remoteClass = a.getRemoteMajorClass(remoteAddr);

	newInfo.lastSeenTime = a.lastSeen(remoteAddr);
	newInfo.lastUsedTime = a.lastUsed(remoteAddr);

	pairingList.push_back(newInfo);

}


void PairedTab::updateGUI()
{

    kdDebug() << __func__ << "() >>>>>>>" << endl;	

    vector<PairingInfo>::iterator pairIt, remotIt;
    QString prevLocalAddr;
    pairingListView->clear();
    QListViewItem *viewItem = NULL;
    QString localName, remoteName;

    if (pairingList.empty())
		return;

    for (pairIt = pairingList.begin(); pairIt != pairingList.end(); ++pairIt) {

		if (pairIt->remoteAddr.isEmpty())
			continue;	

		if (prevLocalAddr != pairIt->localAddr) {
			viewItem = new QListViewItem(pairingListView);
			viewItem->setOpen(true);
			viewItem->setPixmap(0, KGlobal::iconLoader()->loadIcon("usbpendrive_unmount", KIcon::Small, 16));
			viewItem->setSelectable(true);
			pairIt->listViewItem = viewItem;
			if (pairIt->localName.isEmpty())
				localName = QString(pairIt->localAddr);
			else
				localName = QString("%1 (%2)").arg(pairIt->localName).arg(pairIt->localAddr);

			viewItem->setText(0, localName);
		}

		if (!viewItem)
			continue;

		QListViewItem *remoteAddrItem = new QListViewItem(viewItem);
		remoteAddrItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(
					"pda_blue", KIcon::Small, 16));
		remoteAddrItem->setSelectable(true);

		QString serviceName;
		for(vector<Service*>::const_iterator ServIt = ServiceList.begin();
		 	ServIt != ServiceList.end(); ++ServIt) {

			serviceName = (*ServIt)->getIdentifier();
			if ((*ServIt)->isTrusted(pairIt->remoteAddr)) {
				truststring = i18n("- trusted ") + serviceName + i18n(" device");
				remoteAddrItem->setText(2, serviceName);
				trustedDev = true;
				break;
			}
		}
		if (!trustedDev) {
			truststring = "";
			serviceName = "";
		}

		if (pairIt->remoteName.isEmpty()) {
			remoteName = QString(pairIt->remoteAddr);
		} else {
			remoteName = i18n("%1 (%2) %3").arg(pairIt->remoteName).arg(pairIt->remoteAddr).arg(truststring);
		}
		remoteAddrItem->setText(0, remoteName);
		remoteAddrItem->setText(1, pairIt->remoteAddr);
		remoteAddrItem->setText(2, serviceName);

		if (pairIt->lastSeenTime != QString::null) {
			QListViewItem *seenTimeItem = new QListViewItem(remoteAddrItem);
			seenTimeItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(
						"clock", KIcon::Small, 16));
			seenTimeItem->setSelectable(false);
			seenTimeItem->setText(0, i18n("Last Seen: %1").arg(pairIt->lastSeenTime));
		}

		if (pairIt->lastUsedTime != QString::null) {
			QListViewItem *usedTimeItem = new QListViewItem(remoteAddrItem);
			usedTimeItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(
						"clock", KIcon::Small, 16));
			usedTimeItem->setSelectable(false);
			usedTimeItem->setText(0, i18n("Last Used: %1").arg(pairIt->lastUsedTime));
		}

		prevLocalAddr = pairIt->localAddr;

    }
    
    kdDebug() << __func__ << "() <<<<<<<<<<<<" << endl;	

}

void PairedTab::slotListChanged()
{
    kdDebug() << "slotListChanged()" << endl;
    reloadList();
    updateGUI();
}

void PairedTab::slotRemovePairing()
{
	kdDebug() << "slotRemovePairing" << endl;

	int n = 0;
    for (QListViewItem* i=pairingListView->firstChild(); i != NULL; i = i->nextSibling()) {
	for (QListViewItem* j=i->firstChild(); j != NULL; j = j->nextSibling()) {
		if (j->isSelected()) {
		    kdDebug() << "Going to erase: " << n << ": " << j->text(1) << endl;
		    	Manager manager(conn);
			Manager::AdapterList list = manager.listAdapters();
			Adapter a(list[n], conn);
			a.removeBonding(j->text(1));
			slotSelectionChanged();
			slotListChanged();
		    return;
		}
	}
	n++;
    }

}

void PairedTab::slotRemoveTrust() {

	kdDebug() << "slotRemoveTrust" << endl;

    for (QListViewItem* i=pairingListView->firstChild(); i != NULL; i = i->nextSibling()) {
	for (QListViewItem* j=i->firstChild(); j != NULL; j = j->nextSibling()) {
		if (j->isSelected()) {
		    kdDebug() << "Going to remove Trust: " << j->text(1) << endl;

			if (!(j->text(2) == "")) {
			   QString path = "/org/bluez/service_" + j->text(2);
			   service = new Service(conn, path);
			   service->removeTrust(j->text(1));
			   slotSelectionChanged();
			   slotListChanged();
			}
					

		    return;
		}
	}
    }


}

void PairedTab::slotSelectionChanged()
{
    Manager man(conn);
    Adapter adapter(man.defaultAdapter(),conn);
    kdDebug() << __func__ << endl;
    for (QListViewItem* i=pairingListView->firstChild(); i != NULL; i = i->nextSibling()) {
	for (QListViewItem* j=i->firstChild(); j != NULL; j = j->nextSibling()) {
		
    		kdDebug() << "text(0)" << j->text(0) << endl;
    		kdDebug() << "text(1)" << j->text(1) << endl;
    		kdDebug() << "text(2)" << j->text(2) << endl;
		if (j->isSelected()) {

			if (adapter.hasBonding(j->text(1)))
				removePairingButton->setEnabled(true);
			else
				removePairingButton->setEnabled(false);
			if (!(j->text(2) == ""))
				removeTrustButton->setEnabled(true);
			else
				removeTrustButton->setEnabled(false);
			return;		
		} else 
			continue;
		
// TODO: Dialog is not ready.		    
//		    exportButton->setEnabled(true);
	}
    }
    removePairingButton->setEnabled(false);
    removeTrustButton->setEnabled(false);
    exportButton->setEnabled(false);
}

void PairedTab::slotExportBonding()
{
	kdDebug() << __func__ << endl;
	exportBondingDialog->show();
}

#include "pairedtab.moc"
