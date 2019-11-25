/***************************************************************************
                          qobexserverchild.cpp  -  description
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

#include "kobexserver.h"

#include "kobexbluetooth.h"
#include "kobexobject.h"
#include "kobexheader.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>


#include <klocale.h>


ObexServer::ObexServer (int fd, QString peerName, QObject *parent, const char *name) :
        QObject(parent, name)
{
    this->fd = fd;
    this->peerName = peerName;

    objectData.open(IO_WriteOnly);
    currentSize = 0;
}

ObexServer::~ObexServer () {}

QByteArray& ObexServer::getObject()
{
    object = objectData.buffer();
    return object;
}

void
ObexServer::slotRequest (ObexObject & object, Obex::Command cmd)
{
    if (cmd == Obex::SetPath) {
        printf ("Received SETPATH command\n");
        object.setResponse (Obex::BadRequest, Obex::BadRequest);
    } else if (cmd == Obex::Connect) {
        state = Connecting;
        printf ("Received CONNECT command\n");
        object.setResponse (Obex::Success, Obex::Success);
    } else if (cmd == Obex::Disconnect) {
        state = Disconnecting;
        object.setResponse (Obex::Success, Obex::Success);
    } else if (cmd == Obex::Put) {
        printf ("Received PUT command\n");
        state = Receiving;
        object.setResponse (Obex::Success, Obex::Success);
    } else if (cmd == Obex::Get) {
        printf ("Received GET command\n");
        object.setResponse (Obex::BadRequest, Obex::BadRequest);
    } else
        object.setResponse (Obex::BadRequest, Obex::BadRequest);
}

void
ObexServer::slotRequestHint (ObexObject & object, Obex::Command cmd)
{
    switch (cmd) {
    case OBEX_CMD_CONNECT:
    case OBEX_CMD_DISCONNECT:
        object.setResponse (Obex::Continue, Obex::Success);
        break;
    case OBEX_CMD_PUT:
        firstStream = TRUE;
        fileAccept = FALSE;
        // Enable the streaming read
        object.enableReadStream();
        object.setResponse (Obex::Continue, Obex::Success);
        break;
    default:
        object.setResponse (Obex::NotImplemented, Obex::NotImplemented);
        break;
    }
}

void
ObexServer::slotRequestDone (ObexObject& /*object*/, Obex::Command cmd, Obex::Response/* rsp*/)
{
    if (cmd == Obex::Connect) {
        state = Connected;
    } else if (cmd == Obex::Disconnect) {
        state = Disconnected;
        obex->deleteNotifier ();
        emit finished ();
    } else if (cmd == Obex::Put) {
        state = Received;
        objectData.close();
        emit(received(fileName, objectData.buffer()));
        objectDataArray = QByteArray();
        objectData.setBuffer(objectDataArray);
        objectData.open(IO_WriteOnly);
    }
}

void
ObexServer::parseHeader (ObexObject & object)
{
    ObexHeader *header;
    const uint8_t *bs;
    uint32_t bq4;
    uint32_t size;
    QByteArray buffer;

    while ((header = object.getNextHeader ()) != 0) {
        switch (header->type ()) {

        case ObexHeader::Who:
            header->data (bs, size);
            fileName = Obex::obexstringToQString((unsigned short*)bs);
            break;
        case ObexHeader::Name:
            header->data (bs, size);
            fileName = Obex::obexstringToQString((unsigned short*)bs);
            break;
        case ObexHeader::MimeType:
            header->data (bs, size);
            mimeType = Obex::obexstringToQString((unsigned short*)bs);
            break;
        case ObexHeader::Length:
            header->data (bq4, size);
            fileSize = bq4;
            break;
        default:
            break;
        }
    }
}

void
ObexServer::slotStreamAvailble (ObexObject & object)
{
    int streamSize;

    if (state == Disconnecting)  {
        disconnect(object);
    } else {
        streamSize = object.readStream (stream);
        objectData.writeBlock(stream);
        if (firstStream) {
            firstStream = FALSE;
            currentSize = 0;
            parseHeader (object);
			obex->pauseNotifier();
            emit putRequest(fileName, fileSize, mimeType);
        }
        currentSize += streamSize;
        emit(progress(currentSize));
    }
}

void
ObexServer::start ()
{
    // Set state Connecting
    state = Starting;

    // Let everyone know what we're up to
    emit info (baseInfoMsg + i18n ("Connecting..."));

    obex = new ObexBluetooth(this);

    obex->serverConnect(fd, peerName);

    connect (obex, SIGNAL (requestHint (ObexObject &, Obex::Command)),
             this, SLOT (slotRequestHint (ObexObject &, Obex::Command)));
    connect (obex, SIGNAL (request (ObexObject &, Obex::Command)),
             this, SLOT (slotRequest (ObexObject &, Obex::Command)));
    connect (obex, SIGNAL (requestDone (ObexObject&, Obex::Command, Obex::Response)),
             this, SLOT (slotRequestDone (ObexObject&, Obex::Command, Obex::Response)));
    connect (obex, SIGNAL (streamAvailble (ObexObject &)), this,
             SLOT (slotStreamAvailble (ObexObject &)));
    connect (obex, SIGNAL (linkError()), this,
             SLOT (slotServerLinkError ()));
}

void ObexServer::pause()
{
    oldState = state;
    state = Waiting;
    obex->pauseNotifier();
}

void ObexServer::restart()
{
    state = oldState;
    obex->restartNotifier();
}

void ObexServer::stop()
{
    if(state!=Disconnected) {
        state = Disconnecting;
        obex->restartNotifier();
    }
}

void
ObexServer::disconnect (ObexObject& object)
{
    // Let everyone known what we're up to
    emit info (baseInfoMsg + i18n ("Disconnecting..."));

    object.setResponse (Obex::BadRequest, Obex::BadRequest);

    obex->deleteNotifier ();
    state = Disconnected;
}

void
ObexServer::acceptFile (bool accept)
{
    if (accept)
        state = Receiving;
    else {
        state = Disconnecting;
    }
    // Reactivate the notifier
    obex->restartNotifier ();
}

void
ObexServer::slotServerLinkError()
{
    emit error("Bluetooth link error");
}

#include "kobexserver.moc"

