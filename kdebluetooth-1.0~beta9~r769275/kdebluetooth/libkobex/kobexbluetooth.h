/***************************************************************************
                          obexbluetooth.h  -  description
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

#ifndef KOBEXBLUETOOTH_H
#define KOBEXBLUETOOTH_H

#include <libkbluetooth/rfcommsocket.h>
#include <libkbluetooth/deviceaddress.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include "kobex.h"

#include <fcntl.h>

class ObexBluetooth : public Obex
{
Q_OBJECT

public:
	ObexBluetooth(KBluetooth::DeviceAddress addr, uint8_t c, QObject *parent=0);
	ObexBluetooth(QObject *parent = 0);
	~ObexBluetooth();
	virtual void transportConnect();
	virtual void serverConnect(int, QString);

signals:
    void transportError();
	void transportConnected();

private slots:
    void slotConnected();
	void slotError(int);

protected:
    KBluetooth::RfcommSocket *socket;

	KBluetooth::DeviceAddress addr;
	uint8_t channel;
};

#endif
