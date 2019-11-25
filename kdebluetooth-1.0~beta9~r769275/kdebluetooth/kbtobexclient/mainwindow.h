/***************************************************************************
                         MainWindow.h  -  description
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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <kapplication.h>
#include <kmainwindow.h>
#include <qwidget.h>
#include <qlabel.h>
#include <kparts/dockmainwindow.h>

#include <libkbluetooth/rfcommsocket.h>
#include <libkbluetooth/deviceaddress.h>
#include <libkbluetooth/serviceselectionwidget.h>

#include <kiconloader.h>
#include <kglobal.h>
#include <kpixmap.h>


#include "kurl.h"

class MainWindow;
class Obex;
class QPushButton;
class FileIconView;
class KFileItem;
class Sender;
class QGridLayout;
class FileView;
class KURL;
class DevicesView;
class KStatusBar;
class QVBox;

/** Access to the "lonely" MainWindow Object */
MainWindow* MainApp();

using namespace KBluetooth;

class MainWindow : public KDockMainWindow
{
    Q_OBJECT
public:
    MainWindow( KBluetooth::DeviceAddress addr = KBluetooth::DeviceAddress::invalid, int channel = 0, const char* name = 0 );
	MainWindow(KURL::List urlList, const char *name);
    virtual ~MainWindow();

    KConfig* config() const { return m_config; }
	
	void showEvent( QShowEvent* e );

public slots:
    void slotSendButtonClicked();
    void slotFileSended( KFileItem* );
    void slotSendError( KFileItem*, const QString& );
    void slotConnectionError( const QString& );
    void slotSenderDestroyed();
	void slotServiceChanged(KBluetooth::DeviceAddress, uint);
	void slotCheckDockWidgetStatus();
	void slotViewFileSelector();
	void slotViewFileBrowser();
	void slotViewDeviceSelector();
	void slotFileSelectorHidden();
	void slotFileBrowserHidden();
	void slotDeviceSelectorHidden();
	void slotFileQuit();

protected:
    bool queryExit();
	void initView();
    void initActions();
    void saveOptions();
	void setStatusBar(KBluetooth::DeviceAddress&, int );
    void saveMru(KBluetooth::DeviceAddress addr, int channel);
	
    QWidget *page;
    FileView *fileBrowser;
    FileIconView* fileSelector;
    QPushButton *sendButton;
    KToolBar *toolbar;
    KStatusBar *m_statusBar;

	DevicesView* devicesView;

    Sender* sender;
    KBluetooth::DeviceAddress devAddr;

    int channel;

    int fileToSend;
    int fileSended;

    bool sending;
	
	QLabel* fileSelectorLabel;
	QLabel* deviceSelectorLabel;
	
	KBluetooth::ServiceSelectionWidget* serviceSelector;
	
	KDockWidget* mainDock;
	KDockWidget* deviceSelectorDock;
	KDockWidget* fileBrowserDock;
	
	QVBox* fileSelectorBox;
	QVBox *deviceSelectorBox;
	
	KAction* actionFileQuit;
	KAction* actionSettingsConfigure;
	
	KToggleAction* actionViewDeviceSelector;
	KToggleAction* actionViewFileSelector;
	KToggleAction* actionViewFileBrowser;
	
	KPixmap mainlogo;	
	
    /** the configuration object of the application */
    KConfig *m_config;

	
	
private slots:

};

#endif // _MainWindow_H_
