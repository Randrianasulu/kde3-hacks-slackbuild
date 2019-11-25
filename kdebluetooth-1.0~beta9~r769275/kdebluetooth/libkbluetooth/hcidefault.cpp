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

#include "hcidefault.h"
#include <kcmdlineargs.h>
#include <stdlib.h>
#include <kdebug.h>
#include <klocale.h>
#include "adapter_old.h"

namespace KBluetooth
{

HciDefault::HciDefault()
{
    reInit();
}

void HciDefault::setDefaultDevice(int n)
{
    HciDefault::instance()->defaultDevice = n;
}

void HciDefault::reInit()
{
    QString devName = "hci0";
    Adapters adapters;
    if (adapters.count() > 0) {
        RAdapter adapter = adapters[0];
        devName = QString("hci%1").arg(adapter.getIndex());
    }
    else {
        kdWarning() << "No usable bluetooth device found." << endl;
    }
    QString envName = QString(getenv("HCI_DEVICE")).lower();
    if (envName != QString::null) {
        devName = envName;
    }

    if (cmdLineOption != QString::null) {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs("bluetooth");
        if (args->isSet(cmdLineOption.local8Bit())) {
            devName = args->getOption(cmdLineOption.local8Bit());
        }
    }

    if (devName != QString::null) {
        kdDebug() << QString("Using %1 as default bluetooth device.").arg(devName) << endl;
        if (devName.startsWith("hci")) {
            devName = devName.remove(0,3);
        }
        bool bOk;
        int devNum = devName.toInt(&bOk);
        if (bOk) defaultDevice = devNum;
    }
}

HciDefault* HciDefault::instance()
{
    static HciDefault inst;
    return &inst;
}

int HciDefault::defaultHciDeviceNum()
{
    return HciDefault::instance()->defaultDevice;
}

void HciDefault::addCmdLineOptions(QString option)
{
    static QString op(option+" <hciX>");
    static QCString cop = op.local8Bit();
    static KCmdLineOptions options[] = {
        {cop, I18N_NOOP("Use bluetooth device hciX (alternative: $HCI_DEVICE)"), 0},
        { 0, 0, 0}};
    KCmdLineArgs::addCmdLineOptions(options, "Bluetooth", "bluetooth");
    HciDefault::instance()->cmdLineOption = option;
    HciDefault::instance()->reInit();
}

HciDefault::~HciDefault()
{
}

}
#include "hcidefault.moc"
