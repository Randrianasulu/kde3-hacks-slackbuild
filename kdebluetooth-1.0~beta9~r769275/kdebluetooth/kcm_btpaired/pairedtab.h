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


#ifndef PAIREDTAB_H
#define PAIREDTAB_H

#include "pairedtabbase.h"
#include <qcstring.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <vector>
#include <stdint.h>

#include "exportdialog.h"

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/dbusfilter.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/service.h>

class DCOPClient;
class KDirWatch;
class FileSettingsBase;

using namespace KBluetooth;

/**
@author Fred Schaettgen
*/
class PairedTab : public PairedTabBase
{
Q_OBJECT
public:
    PairedTab(QWidget *parent, const char* name);
    ~PairedTab();
private:
    struct PairingInfo {
        QString localAddr;
	QString localName;
        QString remoteAddr;
        QString remoteName;
        QString remoteClass;
        uint8_t linkKey[16];
        uint8_t type;
        QString lastUsedTime;
	QString lastSeenTime;
        QListViewItem *listViewItem;
    };
    std::vector<PairingInfo> pairingList;
    std::vector<Service*> ServiceList;
    
    void reloadList();
    void updateGUI();

    DBusInit *dbus;
    DBusFilter *filter;
    DBusConnection *conn;

    static DBusHandlerResult filterFunction(DBusConnection *conn, DBusMessage *msg, void *data);

    static PairedTab *_ctx; 

    ExportDialog *exportBondingDialog;
    Service* service;
    QString truststring;
    bool trustedDev;

    void getDetails(const char*,Adapter&);
    void getServiceList(Adapter&);
    void getBondingList(Adapter&);
    QString localAddr;
    QString localName;
    
private slots:
    void slotListChanged();
    void slotRemovePairing();
    void slotRemoveTrust();
    void slotSelectionChanged();
    void slotExportBonding();

};

#endif
