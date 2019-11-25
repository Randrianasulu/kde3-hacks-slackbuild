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

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "rfcommsocket.h"
#include "deviceaddress.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <kdebug.h>

namespace KBluetooth {

void RfcommSocket::connectToHost(
    const DeviceAddress& addr, int channel)
{
    this->m_addr = addr;
    kdDebug() << QString("RfcommSocket::connectToHost(%1, %2)")
        .arg(QString(addr)).arg(channel) << endl;
    //TODO: connectToHost should return immediately.
    sockaddr_rc rem_addr, loc_addr;
    int s;

    if( (s = ::socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0 ) {
        emit error(errno);
        kdDebug() << QString("Can't create socket. %1(%2)")
            .arg(strerror(errno)).arg(errno) << endl;
        return;
    }

    memset(&loc_addr, 0, sizeof(loc_addr));
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = DeviceAddress::any.getBdaddr();
    if (bind(s, (struct sockaddr *) &loc_addr, sizeof(loc_addr)) < 0 ) {
        emit error(errno);
        kdDebug() << QString("Can't bind socket. %1(%2)")
            .arg(strerror(errno)).arg(errno) << endl;
        return;
    }

    memset(&rem_addr, 0, sizeof(rem_addr));
    rem_addr.rc_family = AF_BLUETOOTH;
    rem_addr.rc_bdaddr = addr.getBdaddr();
    rem_addr.rc_channel = channel;
    if(::connect(s, (struct sockaddr *)&rem_addr, sizeof(rem_addr)) < 0 ){
            emit error(errno);
            kdDebug() << QString("Can't connect. %1(%2)")
                .arg(strerror(errno)).arg(errno) << endl;
            ::close(s);
            return;
    }
    kdDebug() << "Connected." << endl;
    setSocket(s);
    //writeBlock("hello world", 12);
    //::write(s, "hello world", 12);
    emit connected();
}

RfcommSocket::RfcommSocket(QObject* parent, const char* name) :
    QSocket(parent, name)
{
    this->m_addr = KBluetooth::DeviceAddress::invalid;
}

DeviceAddress RfcommSocket::addr() const
{
    return m_addr;
}

RfcommSocket::~RfcommSocket()
{

}


};
#include "rfcommsocket.moc"
