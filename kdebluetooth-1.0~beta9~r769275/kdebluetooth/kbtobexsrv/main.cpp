//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdialogbase.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kurl.h>
//#include <libkbluetooth/configinfo.h>

#include "maindlg.h"

static const char *description =
    I18N_NOOP("KDE OBEX Push Server for Bluetooth");

static KCmdLineOptions options[] =
{
    {"rfcommconnection <sockfd>", I18N_NOOP( "Used by kbluetoothd" ), 0 },
    {"peeraddr <addr>", I18N_NOOP( "Used by kbluetoothd" ), 0 },
    {"peername <name>", I18N_NOOP( "Used by kbluetoothd" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KAboutData aboutData( "kbtobexsrv", description, 
        0,
	description, KAboutData::License_GPL,
        "(c) 2003, Fred Schaettgen", 0, 0, "kdebluetooth@0xF.de");
    aboutData.addAuthor("Fred Schaettgen",0, "kdebluetooth@0xF.de");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication a;
    bool bOk;
    int socket = QString(args->getOption("rfcommconnection"))
                 .toInt(&bOk);
    QString name = QString(args->getOption("peername"));
    if (bOk)
    {
        MainDialog *dlg = new MainDialog(NULL, socket, name);
        //dlg->show();

        a.setMainWidget(dlg);
        int ret = a.exec();
        delete dlg;
        return ret;
    }
    else
    {
        KMessageBox::information(NULL, 
            i18n("kbtobexsrv will be called automatically by kbluetoothd \
when another device tries to send you some files. You don't have to run it manually."));
        return 1;
    }
}
