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

#include "kbtserialchat.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <libkbluetooth/hcidefault.h>
#include <libkbluetooth/configinfo.h>

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char* version = KDEBluetoothConfig::version;

static KCmdLineOptions options[] =
    {
        {"rfcommconnection <sockfd>", I18N_NOOP("Used by kbluetoothd"), 0},
        {"peeraddr <addr>", I18N_NOOP("Used by kbluetoothd"), 0},
        {"peername <name>", I18N_NOOP("Used by kbluetoothd"), 0},
        {"+[sdpurl]", I18N_NOOP("Used by kio_sdp"), 0 },
        KCmdLineLastOption
    };

using namespace KBluetooth;

int main(int argc, char **argv)
{
    KAboutData about("kbtserialchat",
        I18N_NOOP("Bluetooth Serial Chat"),
        version, description,
        KAboutData::License_GPL,
        "(C) 2003 Fred Schaettgen", 0, 0,
        "kdebluetooth@0xF.de");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KBluetooth::HciDefault::addCmdLineOptions();
    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KGlobal::locale()->insertCatalogue("libkbluetooth");
    if (args->isSet("rfcommconnection") && args->isSet("peeraddr") &&
        args->isSet("peername")) {
        int socket = QString(args->getOption("rfcommconnection"))
            .toInt();
        QString name = QString(args->getOption("peername"));
        QString addr = QString(args->getOption("peeraddr"));

        MainDialog *mainDialog = new MainDialog(socket, addr, name);
        app.setMainWidget(mainDialog);
        mainDialog->show();
        return app.exec();
    }
    else if (args->count() == 1) {
        KURL url = args->arg(0);
        QString host = url.host();
        // Get the selected sdp service record handle
        QString uuid = url.queryItem("recordhandle");
        // Get the rfcomm channel
        QString channel = url.queryItem("rfcommchannel");

        MainDialog *mainDialog = new MainDialog(
            DeviceAddress(host), channel.toInt());
        app.setMainWidget(mainDialog);
        mainDialog->show();
        return app.exec();
    }
    else {
        MainDialog *mainDialog = new MainDialog();
        app.setMainWidget(mainDialog);
        mainDialog->show();
        return app.exec();
    }
    return 0;
}

