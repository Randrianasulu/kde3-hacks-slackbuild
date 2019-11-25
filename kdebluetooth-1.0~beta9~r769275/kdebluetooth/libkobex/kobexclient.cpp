/***************************************************************************
                          obexclient.cpp  -  description
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

#include "kobexclient.h"

#include "kobexobject.h"
#include "kobexheader.h"

#include <qfile.h>
#include <qfileinfo.h>

#include <klocale.h>

using namespace KBluetooth;

ObexClient::ObexClient (DeviceAddress bda, int channel, QObject *parent) :
        QObject(parent)
{
    state = Disconnected;
    fileReady = FALSE;

    //Create the Obex connection

    obex = new ObexBluetooth(bda, channel, this);

    connect (obex, SIGNAL (requestDone(ObexObject &, Obex::Command, Obex::Response)),
             this, SLOT (slotRequestDone(ObexObject &, Obex::Command, Obex::Response)));
    connect (obex, SIGNAL (idle ()), this, SLOT (slotIdle ()));
    connect (obex, SIGNAL (streamEmpty (ObexObject &)), this,
             SLOT (slotStreamEmpty (ObexObject &)));

	connect (obex, SIGNAL (transportConnected()), this, SLOT (slotTransportConnected()));
	connect (obex, SIGNAL (transportError()), this, SLOT (slotTransportError()));


}

ObexClient::~ObexClient () {}

int ObexClient::getState()
{
    return state;
}

bool ObexClient::connected()
{
    if(state != Disconnected && state != Disconnecting)
        return TRUE;
    else
        return FALSE;
}

void
ObexClient::slotRequestDone (ObexObject &/* object*/, Obex::Command cmd,
                             Obex::Response rsp)
{

    if (cmd == Obex::Connect) {
        if (rsp == Obex::Success)
            state = Ready;
        else {
            state = Error;
            QString
            msg (i18n ("Error connecting to phone\nResponse message: "));
            msg += Obex::responseToQString (rsp);
            emit connectionError (msg);

        }
    } else if (cmd == Obex::Disconnect) {
        state = Disconnected;
        obex->transportDisconnect ();
    } else if (cmd == Obex::Put) {
        if (rsp == Obex::Success) {
            state = Finished;
            emit fileSended();
        } else {
            state = Error;
            QString msg(i18n("Error sending file %1 to phone\nResponse message: ")
                .arg(filePath.fileName()));
            msg += Obex::responseToQString (rsp);
            emit sendError (msg);

        }
    } else if (cmd == Obex::Get) {
        if (rsp == Obex::Success) {
            state = Finished;
        } else {
            state = Error;
            QString
            msg (i18n ("Error getting data from phone\nResponse message: "));
            msg += Obex::responseToQString (rsp);
            emit sendError (msg);

        }
    }
}


void
ObexClient::slotIdle ()
{

    if (state == Finished || state == Error) {
        state = Ready;
        emit info (baseInfoMsg + i18n ("Done"));
    }

    if (state == Ready) {
        if(fileReady) {
            fileReady = FALSE;
            put(filePath);
        }
    }
}


void
ObexClient::startConnection ()
{
    // Set state
    state = Connecting;

    // Let everyone know what we're up to
    emit info (baseInfoMsg + i18n ("Connecting..."));

    // Attempt to connect transport
    obex->transportConnect ();
}

void ObexClient::slotTransportConnected()
{
    // Create connect object
    ObexObject object (*obex, Obex::Connect);

    // Create who header and add to header
    const char *who = "Linux";
    ObexHeader whoHeader (ObexHeader::Who, (uint8_t *) who, 6);
    object.addHeader (whoHeader, ObexObject::FitOnePacket);

    // Send request
    obex->request (object);

    //return TRUE;

}

void ObexClient::slotTransportError()
{
    emit connectionError (i18n ("Error connecting transport"));
}

void
ObexClient::stopConnection ()
{

    // Set state
    state = Disconnecting;

    // Let everyone known what we're up to
    emit info (baseInfoMsg + i18n ("Disconnecting..."));

    // Create disconnect object
    ObexObject object (*obex, Obex::Disconnect);

    // Send request
    obex->request (object);


}


void
ObexClient::put (const KURL& src)
{

    // Find file name
    QFileInfo fileInfo (src.path());
    QString dst = fileInfo.fileName ();

    // Open file
    QFile file (src.path());
    file.open (IO_ReadOnly);

    // Read data
    buffer = file.readAll ();

    // Close file
    file.close ();

    // Store some things
    current = 0;
    target = dst;
    baseInfoMsg = i18n ("Sending ") + src.path() + i18n (" to phone\n\n");

    // Set state
    state = Putting;

    // Inform others of size of file
    emit totalSize (buffer.size ());

    // Let everyone know what we're up to
    emit info (baseInfoMsg + i18n ("Requesting stream start..."));

    // Create object
    ObexObject object (*obex, Obex::Put);

    // Create name header and add to object
    QByteArray *unicode = Obex::qStringToUnicode (target);
    ObexHeader nameHeader (ObexHeader::Name, (uint8_t *) unicode->data (),
                           unicode->size ());
    object.addHeader (nameHeader, ObexObject::None);
    delete unicode;

    // Create length header and add to object
    ObexHeader lengthHeader (ObexHeader::Length, (uint32_t) buffer.size (),
                             4);
    object.addHeader (lengthHeader, ObexObject::None);

    // Signify that this is the start of a stream
    ObexHeader streamHeader (ObexHeader::Body, (uint8_t *) NULL, 0);
    object.addHeader (streamHeader, ObexObject::StreamStart);

    // Send object
    obex->request (object);

}


void
ObexClient::slotStreamEmpty (ObexObject & object)
{

    // Emit signal for current position in file
    emit progress (current);

    /* Test for current is workaround for getting stream empty signal
     * before file has been accepted on the phone */
    if (current != 0) {
        emit info (baseInfoMsg + i18n ("Streaming data..."));
    }
    // Check to see if everything has already been sent
    if (current == buffer.size ()) {
        // Add end stream header to object
        ObexHeader endHeader (ObexHeader::Body,
                              (uint8_t *) buffer.data () + current, 0);
        object.addHeader (endHeader, ObexObject::StreamEnd);
    } else {
        // Create body header to and add to object
        int remain = buffer.size () - current;
        if (remain < streamChunk) {
            ObexHeader dataHeader (ObexHeader::Body,
                                   (uint8_t *) (buffer.data () + current),
                                   remain);
            object.addHeader (dataHeader, ObexObject::StreamData);
            current += remain;
        } else {
            ObexHeader dataHeader (ObexHeader::Body,
                                   (uint8_t *) (buffer.data () + current),
                                   streamChunk);
            object.addHeader (dataHeader, ObexObject::StreamData);
            current += streamChunk;
        }
    }
}


int
ObexClient::sendFile(const KURL &src)
{

    if(state != Ready ) {
        fileReady = TRUE;
        filePath = src;
    } else {
        // Put file
        put (src);
    }

    if(state == Connecting)
        return 1;
    else
        return 0;

}


void
ObexClient::finishOp (ObexObject & /* object */ ) {}

#include "kobexclient.moc"
