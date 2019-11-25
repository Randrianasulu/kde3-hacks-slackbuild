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

#ifndef ADAPTERCONFIG_H_
#define ADAPTERCONFIG_H_

#include "adapterdialog.h"

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/adapter.h>


#include <qlabel.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <klocale.h>
#include <qobject.h>
#include <qslider.h>

#include "application.h"

using namespace KBluetooth;

class AdapterConfig : public QObject {

	Q_OBJECT

	public:
		AdapterConfig(DBusConnection *conn, QString path);
		AdapterConfig(KBluetoothApp* app);
		~AdapterConfig();

		AdapterDialog *dialog() const { return adapterDialog; };
		QString getName() { return name; };

		void setupDialog();
	
	public slots:
		void slotChangeTimeout(int);
		void slotChangeAdapterTimeout(int);
//		void setAdapterTimeout(int bar);
		void slotChangeName(const QString&);
		void slotSetMode(int);
		void slotSetClass(const QString&);

	private:
		AdapterDialog *adapterDialog;
		Adapter *adapter;
		QString name;
		void addAdapterDialogInfo();
};

#endif // ADAPTERCONFIG_H_
