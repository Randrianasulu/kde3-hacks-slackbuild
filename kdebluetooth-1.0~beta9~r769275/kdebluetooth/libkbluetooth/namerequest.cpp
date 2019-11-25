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

#include "namerequest.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>

#include <asm/types.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "hcidefault.h"
#include <klocale.h>

namespace KBluetooth
{

QString NameRequest::resolve(DeviceAddress addr)
{
    bdaddr_t bdaddr;
    baswap(&bdaddr, strtoba(((QString)addr).ascii()));

    int dev_id = KBluetooth::HciDefault::defaultHciDeviceNum();
    if (dev_id < 0)
    {
        errorMessage = i18n("Device is not available.");
        return QString::null;
    }

    int dd = hci_open_dev(dev_id);
    if (dd < 0)
    {
        errorMessage = i18n("HCI device open failed.");
        return QString::null;
    }

    char name[255];
    QString ret;
    if (hci_read_remote_name(dd, &bdaddr,
                             sizeof(name), name, 25000) == 0)
    {
        errorMessage = QString::null;
        ret = QString(name);
    }
    else
    {
        errorMessage = i18n("HCI name request failed.");
        ret = QString::null;
    }
    hci_close_dev(dd);
    return ret;
}

QString NameRequest::lastErrorMessage()
{
    return errorMessage;
}

}
#include "namerequest.moc"
