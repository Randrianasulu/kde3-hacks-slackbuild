/***************************************************************************
                          obex.cpp  -  description
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
#include "kobex.h"
#include "kobexobject.h"

#include <kdebug.h>
#include <qtimer.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>


Obex::Obex (QObject *parent): QObject(parent)
{}

Obex::~Obex () {}

void
Obex::request (ObexObject & obj)
{
    OBEX_Request (obex, obj.object);
}

void
Obex::transportDisconnect ()
{
    delete notifier;
}

QString Obex::responseToQString (Obex::Response rsp)
{

    // Get latin response string
    char *ptr = OBEX_ResponseToString (int (rsp));

    QString tmp (ptr);

    return tmp;
}

QByteArray* Obex::qStringToUnicode (const QString & src)
{

    // Get latin string
    const char *orig = src.latin1 ();

    // Create array to hold unicode
    int length = 2 * (src.length () + 1);
    QByteArray *dst = new QByteArray (length);
    char *dst_array = dst->data ();

    // Convert
    OBEX_CharToUnicode ((uint8_t *) dst_array, (uint8_t *) orig, length);

    return dst;
}

QString Obex::obexstringToQString(unsigned short* buf)
{
    // Obex unicode strings are in network byte order.
    for (int pos=0; buf[pos]!=0; ++pos) {
        buf[pos] = ntohs(buf[pos]);
    }
    QString ret = QString::fromUcs2(buf);
    for (int pos=0; buf[pos]!=0; ++pos) {
        buf[pos] = htons(buf[pos]);
    }
    return ret;
}

void Obex::createNotifier ()
{
    int fd = OBEX_GetFD (obex);
    notifier = new QSocketNotifier (fd, QSocketNotifier::Read);
    connect (notifier, SIGNAL (activated (int)), this,
             SLOT (slotHandleInput (int)));
}

void Obex::deleteNotifier ()
{
    delete notifier;
}

void Obex::pauseNotifier ()
{
    if (notifier)
        notifier->setEnabled(FALSE);
}

void Obex::restartNotifier ()
{
    if (notifier)
        notifier->setEnabled(TRUE);
}

void Obex::cleanup ()
{
    OBEX_Cleanup (obex);
}

void Obex::cancelRequest(int nice)
{
    OBEX_CancelRequest(obex, nice);
}

void Obex::handleEvent (obex_object_t * obj, int /* mode */ , int event,
                        int obex_cmd, int obex_rsp)
{
    ObexObject object (obex, obj);

    switch (event) {
    case OBEX_EV_PROGRESS:
        emit progressMade (object);
        break;

    case OBEX_EV_ABORT:
        emit requestAborted ();
        break;

    case OBEX_EV_REQDONE:
        emit requestDone (object, Obex::Command (obex_cmd),
                          Obex::Response (obex_rsp));

        /* This is a hack to emulate the use of postEvent and
         * customEvent which don't appear to work */
        QTimer::singleShot (0, this, SLOT (slotIdle ()));
        break;
    case OBEX_EV_REQHINT:
        emit requestHint (object, Obex::Command (obex_cmd));

        break;

    case OBEX_EV_REQ:
        emit request (object, Obex::Command (obex_cmd));
        break;


    case OBEX_EV_LINKERR:
        QTimer::singleShot(0, this, SIGNAL(linkError()));
        //emit linkError ();
        break;

    case OBEX_EV_STREAMEMPTY:
        emit streamEmpty (object);
        break;

    case OBEX_EV_STREAMAVAIL:
        emit streamAvailble (object);
        break;

    case OBEX_EV_ACCEPTHINT:

        //    emit acceptHint (serverAccept ());

        break;

    default:
        // Unknown event - ignore for the mean time
        break;
    }
}

void Obex::slotHandleInput (int/* fd*/)
{
    kdDebug() << "slotHandleInpout" << endl;
    if (OBEX_HandleInput (obex, 0) < 0) {
        if (notifier) {
            int socket = notifier->socket();
            delete notifier;
            ::close(socket);
            //QTimer::singleShot(0, this, SIGNAL(linkError()));
        }
        kdDebug() << "slotHandleInpout Error" << endl;
    }
    kdDebug() << "slotHandleInpout end" << endl;
}


void Obex::slotIdle ()
{
    emit idle ();
}

extern "C" void Obex_handleEvent (obex_t * handle, obex_object_t * obj,
                                      int mode, int event, int obex_cmd, int obex_rsp)
{

    // Retrieve pointer to QObex object stored in obex_t
    Obex *obex = (Obex *) OBEX_GetUserData (handle);

    // Call QObex to handle event
    obex->handleEvent (obj, mode, event, obex_cmd, obex_rsp);
}

#include "kobex.moc"

