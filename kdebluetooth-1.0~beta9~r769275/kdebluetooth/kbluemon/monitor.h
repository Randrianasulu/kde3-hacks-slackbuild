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
#ifndef MONITOR_H
#define MONITOR_H



#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/dbussignal.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/devicemimeconverter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <time.h>
#include <qobject.h>
#include <qvaluevector.h>
#include "remotedevice.h"
#include <kprogress.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <qtimer.h>
#include <qheader.h>
#include <qstatusbar.h>
#include <qlineedit.h>


#include "mondialog.h"

class Monitor : public MonDialog
{
Q_OBJECT


private:
	typedef QValueVector<RemoteDevice*> RemoteDeviceList;
	RemoteDeviceList deviceList;
	RemoteDeviceList::iterator it;
	KBluetooth::Adapter *adapter;
	
	DBusConnection *conn;
	QString path;
	KBluetooth::Manager *manager;
	QRegExp rx;
	bool selfstart;	
	KPixmap logo;
	KPixmap mainlogo;
	KPixmap logoBlend;

	QTimer* chgLogoTimer;
	bool timer;
	bool serviceDiscovery;
	void connection();
	void disconnection();
	QStatusBar* statusbar;
	void getDetails(RemoteDevice*,const QString&);
	RemoteDevice* createDevice(const QString& addr, short rssi );
	void getServices(RemoteDevice* add);

public:
	Monitor();
	~Monitor();

public slots:
	void newdev(const QString&, int, short);
	void chg_name(const QString&, const QString&);
	void deldev(const QString&);
	void stopDisco();
	void restartPeriodicDiscovery();
	void chg_details(QListViewItem*);
	void updateLogo();
	void enableStartSearch();
	void slotPeriodicScan(bool);
	void slotSdpDiscovery(bool);
	void setDiscoverStatusOn();

};
	
#endif

