/***************************************************************************
                          MainWindow.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
    email                : simone.gotti@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <libkbluetooth/serviceselectionwidget.h>
//#include <libkbluetoothd/mruservices.h>

#include "mainwindow.h"
#include "sender.h"
#include "fileiconview.h"
#include "fileview.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qvbox.h>
#include <kmainwindow.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kdockwidget.h>
#include <kaction.h>
#include <kxmlguifactory.h>
#include <kstdguiitem.h>

using namespace KBluetooth;

static MainWindow* s_MainWindow = 0;

MainWindow* MainApp()
{
    if( !s_MainWindow ) {
        kdDebug() << "No MainWindow found!" << endl;
        KApplication::exit(1);
    }

    return s_MainWindow;
}


MainWindow::MainWindow(KBluetooth::DeviceAddress addr, int channel, const char *name):KDockMainWindow (0L, name)
{
    s_MainWindow = this;
    m_config = kapp->config();

    sending = false;
    this->devAddr = addr;
    this->channel = channel;

    initView();
    initActions();
    createGUI();
	
    setStatusBar(devAddr, channel);
}

MainWindow::MainWindow(KURL::List urlList, const char *name):KDockMainWindow (0L, name)
{
    s_MainWindow = this;
    m_config = kapp->config();

    sending = false;
    this->devAddr = KBluetooth::DeviceAddress::invalid;
    this->channel = 0;

    initView();
	initActions();
	createGUI();

    KURL::List::iterator it;
    for(it = urlList.begin(); it != urlList.end(); ++it)
        fileSelector->insertItem(*it);

    fileSelector->updateView(true);
	
	setStatusBar(devAddr, channel);
}

void MainWindow::saveMru(KBluetooth::DeviceAddress addr, int channel)
{    
//    KBluetoothd::MRUServices::add(
 //   QStringList("kbtobexclient") << QString("sdp://[%1]/params?rfcommchannel=%2")
  //      .arg(QString(addr)).arg(channel), addr);
}

MainWindow::~MainWindow() {}

void MainWindow::showEvent( QShowEvent* e )
{
  slotCheckDockWidgetStatus();
  KDockMainWindow::showEvent( e );
}

void MainWindow::initView()
{
    resize(750,600);

	m_statusBar = statusBar();
    m_statusBar->insertItem(QString::null, 0);
    m_statusBar->setItemAlignment(0, Qt::AlignLeft);
    m_statusBar->insertItem(QString::null, 1, 100);
    m_statusBar->setItemAlignment(1, Qt::AlignLeft);
	
	// setup main docking things
	mainDock = createDockWidget( "project_view", SmallIcon("idea"), 0, kapp->makeStdCaption( i18n("Project View") ), i18n("Project View") );
  	setView( mainDock );
  	setMainDockWidget( mainDock );


	mainlogo =  KGlobal::iconLoader()->loadIcon("kdebluetooth", KIcon::Small, 16);
	setIcon(mainlogo);

	connect( mainDock, SIGNAL(iMBeingClosed()), this, SLOT(slotViewFileSelector()) );
   	connect( mainDock, SIGNAL(hasUndocked()), this, SLOT(slotViewFileSelector()) );
	
  	fileSelectorBox = new QVBox( mainDock );
  	mainDock->setWidget( fileSelectorBox );
  
  	fileSelectorLabel = new QLabel(i18n("File to send:"), fileSelectorBox, "kde toolbar widget");
  	fileSelector = new FileIconView(fileSelectorBox, NULL);
  	fileSelector->setSelectionMode(KFile::Extended);
  	fileSelector->showPreviews();
	
	sendButton = new QPushButton(fileSelectorBox);
    sendButton->setText(i18n("Send"));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(slotSendButtonClicked()));
  
	fileBrowserDock = createDockWidget( "file_view", SmallIcon("idea"), 0, kapp->makeStdCaption( i18n("Project View") ), i18n("Files View") );
	connect( fileBrowserDock, SIGNAL(iMBeingClosed()), this, SLOT(slotFileBrowserHidden()) );
    connect( fileBrowserDock, SIGNAL(hasUndocked()), this, SLOT(slotFileBrowserHidden()) );
	
	fileBrowser = new FileView(fileBrowserDock);
	fileBrowserDock->setWidget( fileBrowser );
	fileBrowserDock->manualDock( mainDock, KDockWidget::DockTop, 10 );
 	
  	deviceSelectorDock = createDockWidget( "devices_view", SmallIcon("idea"), 0, kapp->makeStdCaption( i18n("Project View") ), i18n("Devices View") );
  	connect( deviceSelectorDock, SIGNAL(iMBeingClosed()), this, SLOT(slotDeviceSelectorHidden()) );
    connect( deviceSelectorDock, SIGNAL(hasUndocked()), this, SLOT(slotDeviceSelectorHidden()) );
		
	deviceSelectorBox = new QVBox( deviceSelectorDock );
	deviceSelectorLabel = new QLabel(i18n("Device selector:"), deviceSelectorBox, "kde toolbar widget");
	QStringList uuids("0x1105");

	//If the address is already selected (the program was call from sdp:/ then we don't select anything in the serviceSelector View)

	if(devAddr != DeviceAddress::invalid) {
		serviceSelector = new ServiceSelectionWidget(deviceSelectorBox, uuids, true, true, false);
	} else {
		serviceSelector = new ServiceSelectionWidget(deviceSelectorBox, uuids, true, true, true);
	}
	connect(serviceSelector, SIGNAL(serviceChanged (KBluetooth::DeviceAddress, uint)), this, SLOT(slotServiceChanged(KBluetooth::DeviceAddress, uint)));
	
	// If the address is not select we take the select (if it exist) from the Service Selector Widget 
	if(devAddr == DeviceAddress::invalid) {
		devAddr = serviceSelector->selectedAddress();
		channel = serviceSelector->selectedChannel();
		
	}
	deviceSelectorDock->setWidget(deviceSelectorBox);
	deviceSelectorDock->manualDock( mainDock, KDockWidget::DockLeft, 10 );
	
}



void MainWindow::initActions()
{

	actionFileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
//	actionSettingsConfigure = KStdAction::preferences(this, SLOT(slotSettingsConfigure()), actionCollection() );
	
	actionViewDeviceSelector = new KToggleAction(i18n("Show Device Selector"), 0, this, SLOT(slotViewDeviceSelector()),
					    actionCollection(), "view_device_selector");
	actionViewFileSelector = new KToggleAction(i18n("Show File Selector"), 0, this, SLOT(slotViewFileSelector()),
					    actionCollection(), "view_file_selector");
	actionViewFileBrowser = new KToggleAction(i18n("Show File Browser"), 0, this, SLOT(slotViewFileBrowser()),
					    actionCollection(), "view_file_browser");
	
}

void MainWindow::setStatusBar(KBluetooth::DeviceAddress &address, int rfcommChannel) 
{
	if(devAddr != KBluetooth::DeviceAddress::invalid) {
        m_statusBar->changeItem("Device Address: " + QString(address), 0);
		m_statusBar->changeItem("Channel: " + QString::number(rfcommChannel), 1);
	} else {
		m_statusBar->changeItem("Device Address: NONE", 0);
		m_statusBar->changeItem("Channel: NONE", 1);
	}
}

void MainWindow::slotServiceChanged(KBluetooth::DeviceAddress address, uint channel) {
	
	this->devAddr = address;
    this->channel = channel;
	setStatusBar(devAddr, channel);
}

void MainWindow::slotSendButtonClicked()
{
    KFileItemList* fileList = (KFileItemList*) fileSelector->items();
    KURL::List urlList;

    if(!fileList->isEmpty()) {
        if(devAddr == DeviceAddress::invalid) 
            return;
      
        fileSended = 0;
        fileToSend = fileList->count();

        //make a KURL::List from a KFileItemList
        for(KFileItem* item = fileList->first(); item; item = fileList->next())
            urlList.append(item->url());

        //To avoid double clicks that will call this slot more than one time
        if(!sending) {
            sending = true;
            sendButton->hide();

            sender = new Sender(devAddr, channel, fileList, fileSelectorBox, 0);
            saveMru(devAddr, channel);    


            QObject::connect(sender, SIGNAL(fileSended(KFileItem*)), SLOT(slotFileSended(KFileItem*)));
            QObject::connect(sender, SIGNAL(connectionError(const QString&)), SLOT(slotConnectionError(const QString&)));
            QObject::connect(sender, SIGNAL(sendError(KFileItem*, const QString&)), SLOT(slotSendError(KFileItem*, const QString&)));
            QObject::connect(sender, SIGNAL(destroyed()), SLOT(slotSenderDestroyed()));

            sender->show();
            sender->send();
        }
    }
}

void MainWindow::slotFileSended(KFileItem* item)
{
    fileSended++;
    fileSelector->removeItem(item);
    sender->send();
}

void MainWindow::slotConnectionError(const QString& msg)
{
    // Display error message
    KMessageBox::error(this, msg);
    sender->deleteLater();
}

void MainWindow::slotSendError(KFileItem */*item*/, const QString& msg)
{
    fileSended++;
    // Display error message
    if(fileSended < fileToSend) {
        int rsp = KMessageBox::questionYesNo(this, msg+i18n("\n Continue with the remaining files?"), QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
        if( rsp == KMessageBox::Yes) {
            sender->send();
        } else
            sender->deleteLater();
    } else {
        KMessageBox::error(this, msg);
        sender->deleteLater();
    }
}

void MainWindow::slotSenderDestroyed()
{
    sending = false;
    sendButton->show();
}

void MainWindow::slotCheckDockWidgetStatus()
{
  actionViewDeviceSelector->setChecked( deviceSelectorDock->isVisible() );
  actionViewFileSelector->setChecked( mainDock->isVisible() );
  actionViewFileBrowser->setChecked( fileBrowserDock->isVisible());
}

void MainWindow::slotViewFileSelector()
{
  mainDock->changeHideShowState();
  slotCheckDockWidgetStatus();
}

void MainWindow::slotViewFileBrowser()
{
  fileBrowserDock->changeHideShowState();
  slotCheckDockWidgetStatus();
}

void MainWindow::slotViewDeviceSelector()
{
  deviceSelectorDock->changeHideShowState();
  slotCheckDockWidgetStatus();
}

void MainWindow::slotFileSelectorHidden()
{
	 actionViewFileSelector->setChecked( false );
}
void MainWindow::slotFileBrowserHidden()
{
	 actionViewFileBrowser->setChecked( false );
}
void MainWindow::slotDeviceSelectorHidden()
{
	 actionViewDeviceSelector->setChecked( false );
}

void MainWindow::slotFileQuit()
{
	this->close();
}

bool MainWindow::queryExit()
{
    saveOptions();
    return true;
}

void MainWindow::saveOptions()
{
    m_config->setGroup("General Options");
    fileBrowser->saveConfig(config());
}

#include "mainwindow.moc"
