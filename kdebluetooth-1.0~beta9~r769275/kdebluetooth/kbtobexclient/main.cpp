/***************************************************************************
                             main.cpp  -  description
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
#include "mainwindow.h"

#include <kapplication.h>
#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>
//#include <libkbluetooth/configinfo.h>
#include <libkbluetooth/deviceaddress.h>
//#include <libkbluetooth/hcidefault.h>

static const char description[] =
    I18N_NOOP("A KDE Bluetooth Framework Application");

//static const char* version = KDEBluetoothConfig::version;

static KCmdLineOptions options[] =
    {
        {"+[sdpurl]", I18N_NOOP("Used by kio_sdp"), 0 },
        KCmdLineLastOption
    };

using namespace KBluetooth;

int main(int argc, char **argv)
{
    KAboutData about("kbtobexclient",
                     I18N_NOOP("Bluetooth OBEX Object Push client"),
                     0, description,
                     KAboutData::License_GPL,
                     "(C) 2003 Simone Gotti", 0, 0,
                     "simone.gotti@email.it");
    about.addAuthor("Simone Gotti",
                    I18N_NOOP("Author, Maintainer"),
                    "simone.gotti@email.it");
    KCmdLineArgs::init(argc, argv, &about);
    
    KCmdLineArgs::addCmdLineOptions( options );
//    KBluetooth::HciDefault::addCmdLineOptions();

    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KGlobal::locale()->insertCatalogue("libkbluetooth");

    KURL url;
    if (args->count() == 0) {
        MainWindow* myMainWindow = new MainWindow();
        app.setMainWidget(myMainWindow);
        myMainWindow->show();
        return app.exec();
    } else if (args->count() > 0) {
        url = args->arg(0);
        //Check If it is a sdp:// URL type
        if(url.protocol() == "sdp") {
            QString host = url.host();
            // Get the selected sdp service record handle
            QString name = url.queryItem("name");
            // Get the rfcomm channel
            QString channel = url.queryItem("rfcommchannel");

            MainWindow* myMainWindow = new MainWindow( DeviceAddress(host), channel.toInt(), NULL);
            app.setMainWidget(myMainWindow);
            myMainWindow->show();
            return app.exec();
        } else { // It's a list of URLs
            KURL::List urlList;
            for(int i = 0; i < args->count(); ++i) {
                urlList.append(args->arg(i));
            }
            MainWindow* myMainWindow = new MainWindow(urlList, NULL);
            app.setMainWidget(myMainWindow);
            myMainWindow->show();
            return app.exec();
        }
    } 
    return 0;
}

