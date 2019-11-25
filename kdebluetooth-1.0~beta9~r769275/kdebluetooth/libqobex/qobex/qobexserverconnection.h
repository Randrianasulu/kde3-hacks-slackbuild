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

#ifndef QOBEXSERVERCONNECTION_H
#define QOBEXSERVERCONNECTION_H

#include <qobject.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qptrlist.h>
#include <qcstring.h>
#include <qsocketnotifier.h>

#include "qobexobject.h"
#include "qobexauth.h"
#include "qobextransport.h"
#include "qobexbase.h"
#include "qobexserverops.h"

class QObexServerConnection
  : public QObexBase {
  Q_OBJECT
public:
  QObexServerConnection( QObexTransport* transport, QObject* = 0, const char* = 0 );
  ~QObexServerConnection();
  void transportConnected();
  void transportConnectionClosed();
  void transportPacketRecieved( QObexObject& req );
  void transportPacketWritten();
  void transportError( QObexTransport::Error );
  void transportConnectionAccept();
  void transportSelectModeChanged( QObexTransport::SelectHint, Q_LONG );
  
signals:
  // Upcall to get serverops from the server frame class.
  void signalRequestServerOps( const QByteArray&, QObexServerOps** );
  void signalNewConnection( QObexTransport* );
  void signalServerConnectionClosed( QObexServerConnection* );

public:
  void appendAuthResponse( const QObexObject&, QObexObject&, QObexServerOps* );

  bool errorResponse( QObexServerOps*, Q_UINT8 code, const char* desc = 0 );
  bool connectErrorResponse( QObexServerOps*, Q_UINT8 code, const char* desc = 0 );
  Q_UINT32 allocConnectionId();
  QObexServerOps* getOpsForUuid( const QByteArray& uuid );

  // Should be really enough since this is meant for a single transport
  // connection. But it is useful to protect against denial of service ...
  enum { MaxPendingAuthentications = 100 };

  // Maps target uuids to nonce's used in auth challenges. This is only used
  // for connect packets. Since authentication information for established
  // connections is stored in the server ops class.
  QPtrList<QObexServerOps> mPendingAuth;

  QMap<Q_UINT32,QObexServerOps*> mServerOps;
  QSocketNotifier* mReadNotifier;
  QSocketNotifier* mWriteNotifier;
};

#endif
