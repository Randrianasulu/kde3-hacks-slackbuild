/***************************************************************************
                          obex.h  -  description
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

#ifndef KOBEX_H
#define KOBEX_H

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qcstring.h>
#include <qguardedptr.h>

class ObexObject;

extern "C"
{
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <openobex/obex.h>
}

extern "C" void Obex_handleEvent(obex_t *, obex_object_t *, int, int, int, int);

class Obex : public QObject
{
    Q_OBJECT

public:
    Obex(QObject* parent = 0);
    virtual ~Obex();
    void request(ObexObject&);
    virtual void transportConnect() = 0;
    virtual void serverConnect(int, QString) = 0;

    void transportDisconnect();
    void createNotifier();
    void deleteNotifier();
    void pauseNotifier();
    void restartNotifier();
    void cleanup();
    void cancelRequest(int nice);
    bool isCreated();

    enum Command { Connect = OBEX_CMD_CONNECT,
                   Disconnect = OBEX_CMD_DISCONNECT,
                   Put = OBEX_CMD_PUT,
                   Get = OBEX_CMD_GET,
                   SetPath = OBEX_CMD_SETPATH,
                   Abort = OBEX_CMD_ABORT };

    enum Response {	Continue = OBEX_RSP_CONTINUE,
                    SwitchPro = OBEX_RSP_SWITCH_PRO,
                    Success = OBEX_RSP_SUCCESS,
                    Created = OBEX_RSP_CREATED,
                    Accepted = OBEX_RSP_ACCEPTED,
                    NoContent = OBEX_RSP_NO_CONTENT,
                    BadRequest = OBEX_RSP_BAD_REQUEST,
                    Unauthorized = OBEX_RSP_UNAUTHORIZED,
                    PaymentRequired = OBEX_RSP_PAYMENT_REQUIRED,
                    Forbidden = OBEX_RSP_FORBIDDEN,
                    NotFound = OBEX_RSP_NOT_FOUND,
                    MethodNotAllowed = OBEX_RSP_METHOD_NOT_ALLOWED,
                    Conflict = OBEX_RSP_CONFLICT,
                    InternalServerError = OBEX_RSP_INTERNAL_SERVER_ERROR,
                    NotImplemented = OBEX_RSP_NOT_IMPLEMENTED,
                    DatabaseFull = OBEX_RSP_DATABASE_FULL,
                    DatabaseLocked = OBEX_RSP_DATABASE_LOCKED };

    static QString responseToQString(Response);
    static QByteArray* qStringToUnicode(const QString&);
	static QString obexstringToQString(unsigned short* buf);

signals:
    void progressMade(ObexObject&);
    void requestAborted();
    void requestHint(ObexObject&, Obex::Command);
    void requestDone(ObexObject&, Obex::Command, Obex::Response);
    void request(ObexObject&, Obex::Command);
    void linkError();
    void streamEmpty(ObexObject&);
    void streamAvailble(ObexObject&);
    void acceptHint(Obex&);
    void idle();

protected:
    obex_t* obex;
    QGuardedPtr<QSocketNotifier> notifier;
    void handleEvent(obex_object_t*, int, int, int, int);
    friend class ObexObject;
    friend void Obex_handleEvent(obex_t *, obex_object_t *, int, int, int, int);

protected slots:
    void slotHandleInput(int);
    void slotIdle();
};

#endif //KOBEX_H
