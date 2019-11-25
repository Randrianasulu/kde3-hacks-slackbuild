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

#ifndef DEVICECONFIG_H_
#define DEVICECONFIG_H_

#include "application.h"
#include "adapterconfig.h"

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/manager.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qdict.h>
#include <kdialogbase.h>


using namespace KBluetooth;

class DeviceConfig : public KDialogBase {

	Q_OBJECT

	public:
		DeviceConfig(KBluetoothApp *app);
		~DeviceConfig();
		QVBoxLayout *layout20;
		QGroupBox *groupBoxInfo2;

	public slots:
		void closeDeviceConfig();
		void addAdapter(const QString&);	
		void removeAdapter(const QString&);
		void slotChangeTab(const QString&);
		void cleanup();

	private:
		DBusConnection *conn;
		KBluetoothApp *app;
		Manager *manager;
		QDict<AdapterConfig> adapterList;

		QLabel *nodevice;

		void addAdapterDialog(Adapter *adapter);
		void addAdapterDialogInfo(Adapter &adapater, AdapterDialog *adapterDialog);

		typedef QPair<QString, QString> DeviceMode;
		typedef QValueList<DeviceMode> DeviceModeList; 
		DeviceModeList mDeviceModes;

		QTabWidget *tabWidget;
		AdapterDialog *adapterDialog;
};

#endif	// DEVICECONFIG_H_

