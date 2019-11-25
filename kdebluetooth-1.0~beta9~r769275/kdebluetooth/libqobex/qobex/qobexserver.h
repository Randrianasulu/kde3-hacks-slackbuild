/*
    This file is part of libqobex.

    Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef QOBEXSERVER_H
#define QOBEXSERVER_H

#include <qobject.h>
#include <qptrlist.h>
#include <qcstring.h>

class QObexTransport;
class QObexServerOps;
class QObexServerConnection;

class QObexServer
  : public QObject {
  Q_OBJECT
public:
  QObexServer( QObject* = 0, const char* = 0 );
  ~QObexServer();

public slots:
  /**
     Call this to register new transport with this server.
   */
  bool slotRegisterTransport( QObexTransport* );

  /**
     Call this to remove a transport from this server.
   */
  QObexTransport* slotUnregisterTransport( QObexTransport* );

  /**
     Call this to register a set of server ops with this server.
   */
  bool slotRegisterServerOps( QObexServerOps* );

  /**
     Call this to remove a set of server ops from this server.
   */
  QObexServerOps* slotUnregisterServerOps( QObexServerOps* );

  /**
     Call this to remove the _first_ set of server ops which claims
     to be responsible for the given uuid.
   */
  QObexServerOps* slotUnregisterServerOps( const QByteArray& );

  /**
     Call this to shut down this server. Means unregister all transports
     and terminate.
   */
  void slotShutdown( bool );

  /**
     Gets connected to the transports read notification.
   */
  void slotConnectionAccept( QObexTransport* );

  /**
     A QObexServerConnection asks for QObexServerOps with this slot.
     It always returns valid serverops.
   */
  void slotRequestServerOps( const QByteArray&, QObexServerOps** );

  void slotServerConnectionClosed( QObexServerConnection* );

private:
  /**
     Allocate a new serverconnection class using the transport given
     in the argument.
   */
  void allocServerConnection( QObexTransport* );

  /**
     Used to send shutdown events.
   */
  enum {
    CustomEventNumber = QEvent::User + 17
  };

  /**
     The list of active transports.
   */
  QPtrList<QObexTransport> mTransports;

  /**
     The list of active serverops.
   */
  QPtrList<QObexServerOps> mServerOpsList;
};

#endif
