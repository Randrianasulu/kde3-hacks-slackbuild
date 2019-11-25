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
#ifndef KBLUELOCK_H
#define KBLUELOCK_H

#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/dbussignal.h>
#include <libkbluetooth/dbusinit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <klocale.h>
#include <ksystemtray.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qmainwindow.h>
#include <qtooltip.h>
#include <kapplication.h>
#include <kuniqueapplication.h>
#include <kpixmap.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <khelpmenu.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <dcopclient.h>
#include <qheader.h>
#include <qstatusbar.h>
#include <qlayout.h>
#include <qcstring.h>

#include "lockdialog.h"

using namespace KBluetooth;

class KBlueLock : public LockDialog {
	
	Q_OBJECT

public:
	KBlueLock();
	~KBlueLock();
	DBusInit* dbus;
	DBusConnection *conn;
	Manager *manager;
	Adapter *adapter;
	KApplication* Kapp;
	KHelpMenu *helpMenu;
	QStringList *setupNewDevList;
	QListViewItem *newdev;
	KSystemTray *systray;
	QToolTip *tooltip;

private:
	QString name;
	QString addr;
	DCOPClient* dcopclient;
	
	KAction* disableAction;
	KAction* enableAction;
	bool on;
	bool selfstart;
	bool unlock;
	bool locked;
	int size;
	QStatusBar* statusbar;

public slots:

	void insertDev(const QString&, int, short);
	void cancelSlot();
	void okSlot();
	void checkBoxSlot(bool);
	void activateLock(const QString&);
        void slotAbout() { helpMenu->aboutApplication(); };
	void setupTray();
	void updateDevName(const QString&, const QString&);
	void disableActionSlot();
	void enableActionSlot();
	void restartPeriodicDiscovery();
	void quitSysTray();
	void slotSearch();
	void slotEnableSearch();
	void setDiscoverStatusOn();
	void activateUnlock(const QString&,int, short);
	void slotStartPeriodicDisco();

};

#endif
