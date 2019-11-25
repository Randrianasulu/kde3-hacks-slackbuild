//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TRAYICON_H
#define TRAYICON_H

#include <qobject.h>
#include <dcopobject.h>
#include <qevent.h>
#include <qtooltip.h>

#include <map>
#include <algorithm>

#include <ksystemtray.h>
#include <kpixmap.h>
//#include <kaboutapplication.h>
#include <kaction.h>
#include <kapplication.h>
//#include <kbugreport.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kicontheme.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifydialog.h>
#include <knotifyclient.h>
#include <kpixmapeffect.h>
#include <kpopupmenu.h>
#include <kprocess.h>
#include <krun.h>
#include <kactionclasses.h>
#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/adapter.h>

#include "application.h"
#include "deviceconfig.h"

class QTimer;
class KToggleAction;
class KAction;
class KActionMenu;
class KPopupMenu;
class KHelpMenu;
class KBluetoothApp;
namespace KBluetooth {
    class DeviceAddress;
}

/**
@author Fred Schaettgen
*/
class TrayIcon : public KSystemTray
{
Q_OBJECT
public:
    TrayIcon(KBluetoothApp* app);
    ~TrayIcon();
    void setAlwaysShowIcon(bool state);

private slots:
   // void slotQuitSelected();
    void slotShowIconToggled(bool);
    void slotServiceConfig();
    void slotPairedConfig();
    void slotDeviceConfig();
    void slotInputConfig();
    void slotkbluemonitor();
    void slotkbluelock();
    void slotkbtobexclient();
    void slotConfigureNotifications() { KNotifyDialog::configure(this); };
    void slotQuitSelected();
    void slotReportBug() { helpMenu->reportBug(); };
    void slotAbout() { helpMenu->aboutApplication(); };
    void slotShowHelp();
    void updateIcon();
    void slotIconConnected(const QString&);
    void slotIconDisconnected(const QString&);
    void slotIconAdapterChanged(const QString&);
    void slotUpdateToolTip(const QString&);
    void slotKbtObexSrv(bool state);

        
protected:
    void mousePressEvent(QMouseEvent *e);

private:
    bool obexserv;
    bool alwaysVisible;
    bool hasAdapter;
    bool acceptClose;
    bool noAdapter;
	bool adapterRemoved;

    KPixmap iconIdle;
    KPixmap iconConnected;
    KPixmap iconNoAdapter;

    QTimer *blinkTimer;
    bool iconConnectingBlinkState;

    KToggleAction* showIconAction;

    KAction *serviceConfigAction;
    KAction *pairedConfigAction;
    KAction *deviceConfigAction;
    KAction *inputConfigAction;
    KAction *showHelpAction;
    KAction *kbluemonAction;
    KAction *kbluelockAction;
    KAction *kbtobexclientAction;
    KAction* aboutAction;
    KActionMenu *configActionMenu;
    KActionMenu *helpActionMenu;

    KBluetoothApp* app;

    KHelpMenu *helpMenu;

    DeviceConfig *deviceConfDialog;
    Adapter *adapter;            
    Manager *manager;            
    QString localAndEnglish(const QCString& s);
	void setupTray(bool);

signals:
   void setKbtobexSrv(bool);

};

#endif
