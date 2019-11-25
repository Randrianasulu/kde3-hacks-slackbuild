/***************************************************************************
                          obexbluetooth.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
		From the original version of  James Stembridge <jstembridge@gmx.net>
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

#include <libkbluetooth/deviceaddress.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include "kobexbluetooth.h"

using namespace KBluetooth;

ObexBluetooth::ObexBluetooth(DeviceAddress addr, uint8_t c, QObject *parent): Obex(parent), channel(c)
{
    // Make internal copies of addresse
    this->addr = addr;

    // Initialize obex
    obex = OBEX_Init(OBEX_TRANS_FD, Obex_handleEvent, 0);

    // Store pointer to self for callbacks
    OBEX_SetUserData (obex, (void *) this);
}

ObexBluetooth::ObexBluetooth(QObject *parent): Obex(parent)
{
    // Set the address to an invalid address
    this->addr = KBluetooth::DeviceAddress::invalid;
    // Initialize obex
    obex = OBEX_Init(OBEX_TRANS_FD, Obex_handleEvent, 0);

    // Store pointer to self for callbacks
    OBEX_SetUserData (obex, (void *) this);
}

ObexBluetooth::~ObexBluetooth() {}

void ObexBluetooth::transportConnect()
{
    socket = new RfcommSocket(this, "rfcommsocket");
    connect(socket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(socket, SIGNAL(error(int)), this, SLOT(slotError(int)));

    socket->connectToHost(addr, channel);

}

void ObexBluetooth::serverConnect(int fd, QString /*peerName*/)
{
    fcntl(fd, O_NONBLOCK);

    //OBEX_SetUserData(handle, global_context);
    FdOBEX_TransportSetup(obex, fd, fd, 255); // TODO: get mtu from rfcommserversocket

    createNotifier();
}


void ObexBluetooth::slotConnected()
{
    int fd = socket->socket();

    FdOBEX_TransportSetup(obex, fd, fd, 255);
    //    transportConnected = TRUE;
    createNotifier();

    emit transportConnected();
}

void ObexBluetooth::slotError(int)
{
    emit transportError();
}

#include "kobexbluetooth.moc"
