/***************************************************************************
                          obexclient.h  -  description
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

#ifndef _KOBEXCLIENT_H
#define _KOBEXCLIENT_H

#include "kobexbluetooth.h"

#include <libkbluetooth/deviceaddress.h>
#include <qobject.h>
#include <kurl.h>

class KURL;

class ObexClient : public QObject
{
    Q_OBJECT

public:
    ObexClient(KBluetooth::DeviceAddress bda, int, QObject* parent = 0);
    ~ObexClient();

    void startConnection();
    void stopConnection();
    int getState();
    bool connected();
    int sendFile(const KURL&);

signals:
    void sendError(const QString&);
    void connectionError(const QString&);
    void info(const QString&);
    void fileSended();
    void totalSize(int);
    void progress(int);

protected slots:
    void slotTransportConnected();
    void slotTransportError();

    void slotRequestDone(ObexObject&, Obex::Command, Obex::Response);
    void slotIdle();
    void slotStreamEmpty(ObexObject&);


protected:
    void disconnectClient();
    void finishOp(ObexObject&);


    Obex* obex;
    QString baseInfoMsg;
    bool fileReady;
    KURL filePath;

    void put(const KURL&);
    QString target;
    QByteArray buffer;
    uint current;

    static const int streamChunk = 512;

    enum State { Connecting,
                 Connected,
                 Ready,
                 Disconnecting,
                 Disconnected,
                 Putting,
                 Finished,
                 Error };

    State state;
};

#endif
