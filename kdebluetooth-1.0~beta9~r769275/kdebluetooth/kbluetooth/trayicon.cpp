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

#include <kprocess.h>
#include <kstdaction.h>
#include "trayicon.h"


using namespace KBluetooth;
using namespace std;

TrayIcon::TrayIcon(KBluetoothApp* app)
{
    iconConnectingBlinkState = false;
    this->helpMenu = new KHelpMenu(this, KApplication::kApplication()->aboutData());
    this->app = app;

    manager = app->manager;
	
    
 	adapterRemoved = 0;

    //iconState = IDLE;
    KConfig *config = KGlobal::config();
    config->setGroup("UI");
//    alwaysVisible = config->readBoolEntry("alwaysVisible", true);
    alwaysVisible = true;
    acceptClose = true;
    obexserv = true;
//    blinkTimer = new QTimer(this);
//    connect(blinkTimer, SIGNAL(timeout()), this, SLOT(updateIcon()));

  // KBlueMon
    kbluemonAction = new KAction(i18n("KBlue&Mon"),
        KGlobal::iconLoader()->loadIcon("info", KIcon::Small, 16), 
	KShortcut::null(), this, "kbluemon");
    connect(kbluemonAction, SIGNAL(activated()), this, SLOT(slotkbluemonitor()));

  // KBlueLock
    kbluelockAction = new KAction(i18n("KBlue&Lock"),
        KGlobal::iconLoader()->loadIcon("kbluelock", KIcon::Small, 16), 
	KShortcut::null(), this, "kbluelock");
    connect(kbluelockAction, SIGNAL(activated()), this, SLOT(slotkbluelock()));

  // KBtobexclient
    kbtobexclientAction = new KAction(i18n("&Send File..."),
        KGlobal::iconLoader()->loadIcon("attach", KIcon::Small, 16), 
	KShortcut::null(), this, "kbtobexclient");
    connect(kbtobexclientAction, SIGNAL(activated()), this, SLOT(slotkbtobexclient()));

    // Configuration menu
    configActionMenu = new KActionMenu(i18n("&Configuration"),
        KGlobal::iconLoader()->loadIcon("configure", KIcon::Small, 16), this, "config_menu");

 // Help menu
    helpActionMenu = new KActionMenu(i18n("&Help"),
        KGlobal::iconLoader()->loadIcon("help", KIcon::Small, 16), this, "help_menu");

    // show documentation
    showHelpAction = KStdAction::help(this,
        SLOT(slotShowHelp()), actionCollection());

    // "About" menu item
    aboutAction = KStdAction::aboutApp(this,
        SLOT(slotAbout()), actionCollection());

    // Inputdevice configuration
    inputConfigAction = new KAction(i18n("&Input Devices..."),
        KGlobal::iconLoader()->loadIcon("configure", KIcon::Small, 16),
        KShortcut::null(), this, "input_config");
    connect(inputConfigAction, SIGNAL(activated()), this, SLOT(slotInputConfig()));

    // Bluetooth Device configuration
    deviceConfigAction = new KAction(i18n("&Adapters..."),
        KGlobal::iconLoader()->loadIcon("configure", KIcon::Small, 16),
        KShortcut::null(), this, "device_config");
    connect(deviceConfigAction, SIGNAL(activated()), this, SLOT(slotDeviceConfig()));

 // Paired device configuration
    pairedConfigAction = new KAction(i18n("&Paired/Trusted Devices..."),
        KGlobal::iconLoader()->loadIcon("configure", KIcon::Small, 16),
        KShortcut::null(), this, "paired_config");
    connect(pairedConfigAction, SIGNAL(activated()), this, SLOT(slotPairedConfig()));

    // KbtObexsrv switch
    showIconAction = new KToggleAction(this, "kbtobexsrv");
    connect(showIconAction, SIGNAL(toggled(bool)), this, SLOT(slotKbtObexSrv(bool)));
    showIconAction->setChecked(obexserv);
    showIconAction->setText(i18n("&Obex Server"));

	connect(manager, SIGNAL(adapterRemoved(const QString&)), SLOT(slotIconAdapterChanged(const QString&)));
    connect(manager, SIGNAL(adapterAdded(const QString&)), SLOT(slotIconAdapterChanged(const QString&)));

	if (!manager->listAdapters().count() || manager->defaultAdapter() == "") { 
	 	 noAdapter = 1;
	 	 setupTray(0);
	 } else {
	     adapter = app->adapter;  
	 	 noAdapter = 0;
	 	 setupTray(1);

	 }


    KPixmap logoPixmap = KGlobal::iconLoader()->loadIcon(
        "kdebluetooth", KIcon::Small, 22);
    iconIdle = logoPixmap;
    iconConnected = logoPixmap;
    iconNoAdapter = logoPixmap;
    KPixmapEffect::toGray(iconIdle);
    KPixmapEffect::fade(iconIdle, 0.3, QColor(255,255,255));
    KPixmapEffect::toGray(iconNoAdapter);
    KPixmapEffect::fade(iconNoAdapter, 0.5, QColor(128,128,128));


    updateIcon();

    connect(this, SIGNAL(quitSelected()), this, SLOT(slotQuitSelected()));

}

void TrayIcon::setupTray(bool enable) {
    
	if (!kbluemonAction->isPlugged()) {
	     kbluemonAction->plug(contextMenu());
	 }
    kbluemonAction->setEnabled(enable);
	

	if (!kbluelockAction->isPlugged())
    	 kbluelockAction->plug(contextMenu());
    kbluelockAction->setEnabled(enable);

	if (!kbtobexclientAction->isPlugged())
	     kbtobexclientAction->plug(contextMenu());
    kbtobexclientAction->setEnabled(enable);

	if (!configActionMenu->isPlugged())
	     configActionMenu->plug(contextMenu());
    configActionMenu->setEnabled(enable);

	if (!helpActionMenu->isPlugged())
	     helpActionMenu->plug(contextMenu());


	if (!showHelpAction->isPlugged())
    	 showHelpAction->plug(helpActionMenu->popupMenu());

    // Report bug menu item
    /*
    KAction* reportBugAction = KStdAction::reportBug(this,
        SLOT(slotReportBug()), actionCollection());
    helpActionMenu->insert(reportBugAction);
    */

	if (!aboutAction->isPlugged())
    	 aboutAction->plug(helpActionMenu->popupMenu());


    // Menu entries:
    // -------------

	if (!inputConfigAction->isPlugged())
    	 inputConfigAction->plug(configActionMenu->popupMenu());

    // Service configuration
    /* TODO: create BlueZ service UI
    serviceConfigAction = new KAction(i18n("Configure &Services..."),
        KGlobal::iconLoader()->loadIcon("configure", KIcon::Small, 16),
        KShortcut::null(), this, "service_config");
    connect(serviceConfigAction, SIGNAL(activated()), this, SLOT(slotServiceConfig()));
    configActionMenu->insert(serviceConfigAction);
    */

	if (!deviceConfigAction->isPlugged())
    	 deviceConfigAction->plug(configActionMenu->popupMenu());

   	if (!pairedConfigAction->isPlugged())
    	 pairedConfigAction->plug(configActionMenu->popupMenu());

    // configure notifications
    /*
    KAction* notificationAction = KStdAction::configureNotifications(this,
        SLOT(slotConfigureNotifications()), actionCollection());
    configActionMenu->insert(notificationAction);
    */
/*
    // Always-visible switch
    showIconAction = new KToggleAction(this, "always_visible");
    connect(showIconAction, SIGNAL(toggled(bool)), this, SLOT(slotShowIconToggled(bool)));
    showIconAction->setChecked(alwaysVisible);
    showIconAction->setText(i18n("Always &Visible"));
    configActionMenu->insert(showIconAction);
*/

	if (!showIconAction->isPlugged())
    	 showIconAction->plug(configActionMenu->popupMenu());

	if (!noAdapter) {
		connect(app->adapter, SIGNAL(remoteDeviceConnected(const QString&)), SLOT(slotIconConnected(const QString&)));
		connect(app->adapter, SIGNAL(remoteDeviceDisconnected(const QString&)), SLOT(slotIconDisconnected(const QString&)));
		connect(app->adapter, SIGNAL(nameChanged(const QString&)), this, SLOT(slotUpdateToolTip(const QString&)));
		connect(app->adapter, SIGNAL(modeChanged(const QString&)), this, SLOT(slotUpdateToolTip(const QString&)));

		kdDebug() << "ADPATER TRAY POINTER" << app->adapter << endl;
	 } else if (adapterRemoved) {
	 	disconnect(app->adapter, SIGNAL(remoteDeviceConnected(const QString&)), this, SLOT(slotIconConnected(const QString&)));
		disconnect(app->adapter, SIGNAL(remoteDeviceDisconnected(const QString&)), this, SLOT(slotIconDisconnected(const QString&)));
		disconnect(app->adapter, SIGNAL(nameChanged(const QString&)), this, SLOT(slotUpdateToolTip(const QString&)));
		disconnect(app->adapter, SIGNAL(modeChanged(const QString&)), this, SLOT(slotUpdateToolTip(const QString&)));

	 }

}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::setAlwaysShowIcon(bool state) {
    showIconAction->setChecked(state);
    slotShowIconToggled(state);
}

void TrayIcon::slotShowIconToggled(bool state)
{
    KConfig *config = KGlobal::config();
    config->setGroup("UI");
    config->writeEntry("alwaysVisible", state);
    config->sync();
    alwaysVisible = state;
    if (state == false) {
        KMessageBox::information(this,
            i18n("KBluetooth will run in hidden mode now and only show up when there is \
a connection. To enable the permanent tray icon start KBluetooth again."),
            i18n("Hidden Mode"), "hiddenmode_messagebox");
    }
    updateIcon();
}

void TrayIcon::updateIcon()
{
    QString oldText = QToolTip::textFor(this);
    QString newText = "";
    Adapter::ConnectionList connList;
	newText = i18n("Not connected");
	if (alwaysVisible) {

	    if (manager->listAdapters().count() > 0) {
	        setPixmap(iconIdle);
		newText = tr("Adapter: %1\nAddress: %2\nMode: %3\n(Not connected)").arg(adapter->getDeviceName()).arg(adapter->getAddress()).arg(adapter->getMode());
		kdDebug() << "Name: " << adapter->getDeviceName() << endl;
	    }
	    else {
		setPixmap(iconNoAdapter);
	 	newText = tr("No Bluetooth Adapter");
	    }
	    show();
	} else {
	    hide();
	}
	//showMonitorAction->setEnabled(false);


//	Manager::AdapterList list = manager->listAdapters();
//	Manager::AdapterList::iterator it;
//	for (it = list.begin(); it != list.end(); ++it) {
//		a = new Adapter((*it), app->conn);

		//connection status only is checked for the default adapter
	 if (!noAdapter) {
		connList = adapter->listConnections();
				
		if (connList.size()) {
			setPixmap(iconConnected);
			newText = i18n("Adapter: %1\nAddress: %2\nMode: %3\nConnected to: ").arg(adapter->getDeviceName()).arg(adapter->getAddress()).arg(adapter->getMode());

			Adapter::ConnectionList::iterator it;
			kdDebug() << "activeConnections:_"<< connList.size() << endl;
			for (it = connList.begin(); it != connList.end(); ++it) {
				QString remoteName = adapter->getRemoteName(*it);
				(remoteName == "") ? remoteName = (*it) : remoteName;
				newText.append("\n* " + remoteName);
			}
		}

	} 

	/*
        bool connecting = false;
        if (connecting) {
            if (!blinkTimer->isActive()) 
		    blinkTimer->start(250);

            iconConnectingBlinkState = !iconConnectingBlinkState;

            if (iconConnectingBlinkState) {
                setPixmap(iconConnected);
            }
            else {
                setPixmap(iconIdle);
            }
        }
        else {
            blinkTimer->stop();
            setPixmap(iconConnected);
        }
	*/



        show();
        //showMonitorAction->setEnabled(true);
	/*
        QString toolTipText = QString("<b></b>")+i18n("Connected to ");
        for (conIt = connections.begin(); conIt != connections.end(); ++conIt) {
            QString devName;
            if (connections.size() > 1) {
                toolTipText += "<br/>";
            }
            if ((devName = app->deviceNameCache->getCachedDeviceName(conIt->first))
                != QString::null)
            {
                toolTipText += QString(
                    "<b>%1</b> [%2]").arg(devName).arg(QString(conIt->first));
            }
            else {
                toolTipText += QString("[<b>%1</b>]").arg(QString(conIt->first));
            }
        }

        toolTipText += "</p>";
        newText = toolTipText;
	*/

    if (oldText != newText) {
        QToolTip::remove(this);
        QToolTip::add(this, newText);
    }

}

void TrayIcon::slotServiceConfig()
{
    // We have to run the control center module with
    // KProcessInheritSocket, because otherwise the kcm
    // would inherit all open sockets and cause problems
    // when kbluetooth::MetaServer::reload is called somewhere
	/*
    KProcessInheritSocket process(0);
    process << "kcmshell" << "kcm_kbluetooth";
    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute the kbluetooth Control Center module."),
            i18n("KBluetooth"));

    }
    */
}

void TrayIcon::slotPairedConfig()
{
    KProcess process(0);
//    process << "kdesu" << "--nonewdcop" << "-i" << "kdebluetooth" << "kcmshell" << "kcm_btpaired";
    process << "kcmshell" << "kcm_btpaired";

    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute the btpaired Control Center module."),
            i18n("KBluetooth"));

    }
}

void TrayIcon::slotDeviceConfig()
{
    kdDebug() << __func__ << "()" << endl;
    deviceConfDialog = new DeviceConfig(app);
    deviceConfDialog->show();
}

void TrayIcon::slotInputConfig()
{
    kdDebug() << __func__ << "()" << endl;
    KProcess process(0);

    process << "kinputwizard";

    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute kinputwizard."),
            i18n("KBluetooth"));
    }	
	
}
void TrayIcon::slotkbluemonitor()
{
    kdDebug() << __func__ << "()" << endl;
    KProcess process(0);

    process << "kbluemon";

    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute KBlueMon."),
            i18n("KBluetooth"));
    }	
}
void TrayIcon::slotkbluelock()
{
    kdDebug() << __func__ << "()" << endl;
    KProcess process(0);

    process << "kbluelock";

    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute KBlueLock."),
            i18n("KBluetooth"));
   }
}

void TrayIcon::slotkbtobexclient()
{
    kdDebug() << __func__ << "()" << endl;
    KProcess process(0);

    process << "kbtobexclient";

    if (!process.start(KProcess::DontCare)) {
        KMessageBox::information(this,
            i18n("Could not execute KBtobexclient."),
            i18n("KBluetooth"));
   }
}

void TrayIcon::slotKbtObexSrv(bool state) {

	emit setKbtobexSrv(state);

}


void TrayIcon::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == QMouseEvent::LeftButton) {
        e->accept();
        KProcess proc;
        proc << "kfmclient" << "openURL" << "bluetooth:/";
        proc.start(KProcess::DontCare);
    }
    else {
        KSystemTray::mousePressEvent(e);
    }
}

void TrayIcon::slotQuitSelected()
{
    // Ask if the user want to simply quit or disable
    // automatic start of kbluetooth
    int autoStart = KMessageBox::questionYesNoCancel( 0,
                                                      i18n("Should KBluetooth still be restarted when you login?"),
                                                      i18n("Automatically Start KBluetooth?"),i18n("Start"), i18n("Do Not Start") );
    acceptClose = true;
    KConfig *config = KGlobal::config();
    config->setGroup("General");
    if (autoStart == KMessageBox::Yes) {
        config->writeEntry("AutoStart", true);
    }
    else if (autoStart == KMessageBox::No) {
        config->writeEntry("AutoStart", false);
    } else {
        acceptClose = false;
        return;
    }

    config->sync();
}


void TrayIcon::slotShowHelp()
{
    // TODO: This is surely not the correct way to jump to the
    // right chapter. Do I really have to mention the html-file,
    // or is the id enough?
    KApplication::kApplication()->invokeHelp("",
        "kdebluetooth/components.html#components.kbluetooth");
}

QString TrayIcon::localAndEnglish(const QCString& s)
{
    if (QString(s) != i18n(s)) {
        return QString("%1 (\"%2\")").arg(i18n(s)).arg(s);
    }
    else {
        return s;
    }
}

void TrayIcon::slotIconConnected(const QString& address)
{
//	if (alwaysVisible) {
		setPixmap(iconConnected);
		kdDebug() << "DEBUG:____ slotIconConnected" << endl;

//	}
		KNotifyClient::event(KApplication::kApplication()->mainWidget()->winId(), "IncomingConnection",
            i18n("Connected to <b>%1</b><br/>(<b>%2</b>)")
            .arg(adapter->getRemoteName(address)).arg(address));

		updateIcon();

}

void TrayIcon::slotIconDisconnected(const QString& /*address*/)
{
//	if (alwaysVisible) {
        	setPixmap(iconIdle);
		kdDebug() << "DEBUG:____ slotIconDisconnected" << endl;
		updateIcon();
//	}

	
}

void TrayIcon::slotIconAdapterChanged(const QString& /*address*/)
{
	if (!alwaysVisible)
		return;

	if (!manager->listAdapters().size()) {
		setPixmap(iconNoAdapter);
	 	noAdapter = 1;
	 	adapterRemoved = 1;
	 	setupTray(0);
	 	
    } else if (noAdapter) {
		setPixmap(iconIdle);
	 	adapter = app->adapter;
	 	noAdapter = 0;
	 	adapterRemoved = 0;
	 	setupTray(1);
	 	 
	} else
		setPixmap(iconIdle);

	updateIcon();
}

void TrayIcon::slotUpdateToolTip(const QString& name) 
{
	kdDebug() << "DEBUG:____ UpdateToolTip " << name << endl;
	updateIcon();

}


#if 0 
// TODO : make use of KNofityClient 
void TrayIcon::slotConnectionComplete(int status, QString addr)
{
    KNotifyClient::event(
#if (QT_VERSION >= 0x030200)
        KApplication::kApplication()->mainWidget()->winId(),
#endif
        "ConnectionError",
        i18n("<i>Bluetooth Monitor</i><br/> Problem connecting with <b>%1</b>:")
            .arg(name)+"<br/>"+errorMessage);
}
#endif

#include "trayicon.moc"
