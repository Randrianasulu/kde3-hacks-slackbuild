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
#ifndef INPUTWIZARD_H
#define INPUTWIZARD_H

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/inputmanager.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/inputdevice.h>
#include <libkbluetooth/service.h>
#include <libkbluetooth/dbussignal.h>
#include <libkbluetooth/devicemimeconverter.h>
#include <qlistbox.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <klocale.h>
#include <kdialog.h>
#include <qdialog.h>
#include <qhbox.h>
#include <qtimer.h>
#include <kapplication.h>
#include <kpixmap.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kpixmapeffect.h>

#include <qlistview.h>
#include <qstringlist.h>


#include "inputdialog.h"
#include "inputdialog_ext.h"

using namespace KBluetooth;

class InputWizard : public InputDialog {
	
	Q_OBJECT

public:
	InputWizard();
	~InputWizard();

private: 

	DBusInit* dbus;
	DBusConnection *conn;
	Manager *manager;
	Adapter *adapter;

	InputDialog_Ext *inputdlg_ext;
	QString busid;
	InputManager *inman;
	InputManager::InputDeviceList inputList;
	InputManager::InputDeviceList::iterator it;
	InputDevice *newdev;

	QStringList *setupNewDevList;
	bool selfstart;
	QString SelectAddr;
	void fillNewDevList(const QString&,QString&);
	int size1;
	int size2;
	KPixmap logoBlend;
	KPixmap pix;
	KPixmap mainlogo;
	QTimer* chgLogoTimer;
	bool timer;

public slots:

	void slotChangeButton(QListViewItem*);
	void slotChangeSetupButton(QListViewItem*);
	void connectdev();
	void disconnectdev();
	void configNew();
	void closeDialog();
	void insertDev(const QString&, int, short);
	void setupNewDev();
	void setupNewDevCallback(const QString&);
	void deleteDev();
	void restartPeriodicDiscovery();
	void updateDevName(const QString&, const QString&);
	void disconnectMsg(bool, QString);
	void connectMsg(bool, QString);
	void slotSearch();
	void slotEnableSearch();
	void setConnectedIcon(const QString&);
	void setDisconnectedIcon(const QString&);
	void setDiscoverStatusOn();
	void updateLogo();


};

#endif
