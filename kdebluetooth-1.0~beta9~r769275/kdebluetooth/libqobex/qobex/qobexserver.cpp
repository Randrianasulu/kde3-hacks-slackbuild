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

#include <qobject.h>
#include <qcstring.h>
#include <qsocketnotifier.h>

#include "qobexobject.h"
#include "qobextransport.h"
#include "qobexserverops.h"
#include "qobexnullserverops.h"
#include "qobexserverconnection.h"

#include "qobexserver.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexServer::QObexServer( QObject* parent, const char* name )
  : QObject( parent, name ) {
  myDebug(( "QObexServer::QObexServer()" ));
  
  // Want to clean up on exit.
  mServerOpsList.setAutoDelete( true );
}

QObexServer::~QObexServer() {
  myDebug(( "QObexServer::~QObexServer()" ));
  slotShutdown( true );
}

bool QObexServer::slotRegisterTransport( QObexTransport* transport ) {
  myDebug(( "QObexServer::slotRegisterTransport( ... )" ));

  // Handle where we already got an accepted connection.
  // Happens in inetd like operation mode.
  if ( transport->isConnected() ) {
    allocServerConnection( transport );
    return true;
  }

  // The usual 50 should be enough ...
  if ( !transport->listen( 50 ) )
    return false;

  QObject::connect( transport, SIGNAL(signalConnectionAccept(QObexTransport*)),
		    SLOT(slotConnectionAccept(QObexTransport*)) );

  QSocketNotifier* sn
    = new QSocketNotifier( transport->socket(), QSocketNotifier::Read,
			   transport, "Socketnotifier for transport" );
  QObject::connect( sn, SIGNAL(activated(int)),
		    transport, SLOT(slotReadyAccept(int)) );
  mTransports.append( transport );

  myDebug(( "QObexServer::slotRegisterTransport(): done" ));
  return true;
}

QObexTransport* QObexServer::slotUnregisterTransport( QObexTransport* ptr ) {
  myDebug(( "QObexServer::slotUnregisterTransport( ... )" ));
  int idx = mTransports.find( ptr );
  return 0 <= idx ? mTransports.take( idx ) : 0;
}

bool QObexServer::slotRegisterServerOps( QObexServerOps* ops ) {
  myDebug(( "QObexServer::slotRegisterServerOps( ... )" ));
  mServerOpsList.append( ops );
  return true;
}

QObexServerOps* QObexServer::slotUnregisterServerOps( const QByteArray& uuid ) {
  myDebug(( "QObexServer::slotUnregisterServerOps( ... )" ));
  for ( Q_ULONG i = 0; i < mServerOpsList.count(); ++i ) {
    if ( mServerOpsList.at( i )->canHandle( uuid ) ) {
      return mServerOpsList.take( i );
    }
  }
  if ( uuid.size() ) {
    for ( Q_ULONG i = 0; i < mServerOpsList.count(); ++i ) {
      if ( mServerOpsList.at( i )->canHandle( QByteArray() ) ) {
	return mServerOpsList.take( i );
      }
    }
  }
  return new QObexNullServerOps;
}

QObexServerOps* QObexServer::slotUnregisterServerOps( QObexServerOps* ptr ) {
  myDebug(( "QObexServer::slotUnregisterServerOps( ... )" ));
  int idx = mServerOpsList.find( ptr );
  return 0 <= idx ? mServerOpsList.take( idx ) : 0;
}

void QObexServer::slotShutdown( bool /* force */ ) {
  myDebug(( "QObexServer::slotShutdown()" ));
  QObexTransport* transport;
  // Shut down all the listening transports.
  while ( ( transport = mTransports.take( 0 ) ) ) {
    slotUnregisterTransport( transport );
    transport->disconnect();
    delete transport;
  }
  // FIXME wait for server connections???
}

void QObexServer::slotConnectionAccept( QObexTransport* transport ) {
  myDebug(( "QObexServer::slotConnectionAccept( ... )" ));
  // Accept the new connection and create a new serverconnection.
  QObexTransport* newtransport = transport->accept();
  if ( !newtransport )
    return;

  allocServerConnection( newtransport );
}    

void QObexServer::slotRequestServerOps( const QByteArray& uuid, QObexServerOps** ops_p ) {
  myDebug(( "QObexServer::slotRequestServerOps( ... )" ));
  for ( Q_ULONG i = 0; i < mServerOpsList.count(); ++i) {
    if ( mServerOpsList.at( i )->canHandle( uuid ) ) {
      *ops_p = mServerOpsList.at( i )->clone();
      return;
    }
  }
  *ops_p = 0;
}

void QObexServer::slotServerConnectionClosed( QObexServerConnection* sc ) {
  myDebug(( "QObexServer::slotServerConnectionClosed( ... )" ));
  delete sc;
}

void QObexServer::allocServerConnection( QObexTransport* transport ) {
  myDebug(( "QObexServer::allocServerConnection( ... )" ));

  QObexServerConnection* serverConnection
    = new QObexServerConnection( transport, this, "QObexServerConnection" );
  connect( serverConnection,
	   SIGNAL(signalRequestServerOps(const QByteArray&,QObexServerOps**)),
	   SLOT(slotRequestServerOps(const QByteArray&,QObexServerOps**)) );
  connect( serverConnection,
	   SIGNAL(signalServerConnectionClosed(QObexServerConnection*)),
	   SLOT(slotServerConnectionClosed(QObexServerConnection*)) );
}    

#include "qobexserver.moc"
