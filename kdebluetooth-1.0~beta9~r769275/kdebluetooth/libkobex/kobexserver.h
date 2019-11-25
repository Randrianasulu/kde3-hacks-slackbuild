/***************************************************************************
                          qobexserverchild.h  -  description
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

#ifndef _KOBEXSERVER_H
#define _KOBEXSERVER_H

#include "kobexbluetooth.h"

#include <libkbluetooth/deviceaddress.h>

#include <qobject.h>
#include <qthread.h>
#include <qbuffer.h>

class Obex;
class ObexObject;

class ObexServer : public QObject
{
    Q_OBJECT

public:
    ObexServer (int fd, QString peerName, QObject *parent=0, const char *name=0);
    ~ObexServer();

    void start();
    void stop();
    void pause();
    void restart();

    void acceptFile(bool);
    QByteArray& getObject();

signals:
    void progress(long);
    void error(const QString&);
    void info(const QString&);
    void done();
    void handleInput();
    void putRequest(QString&, long, QString&);
    void received(QString&, QByteArray);
    void finished();

protected slots:
    void slotRequest(ObexObject&, Obex::Command);
    void slotRequestHint(ObexObject&, Obex::Command);
    void slotRequestDone(ObexObject&, Obex::Command, Obex::Response);
    void slotStreamAvailble(ObexObject& object);
    void slotServerLinkError();

protected:
    void disconnect(ObexObject&);
    void put(ObexObject& object);
    void parseHeader(ObexObject& object);

    bool firstStream;
    bool fileAccept;

    Obex* obex;

    QByteArray stream;
    QBuffer objectData;
    QByteArray objectDataArray;
    QByteArray object;

    QString baseInfoMsg;
    QString sender;
    QString fileName;
	QString mimeType;
	QString peerName;
	int fd;

    long fileSize;
    long currentSize;

    enum State { Starting,
	             Connecting,
                 Connected,
                 Disconnecting,
                 Disconnected,
                 WaitingAcceptFile,
                 Receiving,
                 Received,
				 Waiting,
                 Error };

    State state, oldState;
};

#endif // KOBEXSERVER_H
