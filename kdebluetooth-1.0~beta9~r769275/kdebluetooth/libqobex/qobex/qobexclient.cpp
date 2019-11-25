/*
    This file is part of libqobex.

    Copyright (c) 2003-2004 Mathias Froehlich <Mathias.Froehlich@web.de>

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

#include <qstring.h>
#include <qcstring.h>

#include "qobexheader.h"
#include "qobexobject.h"
#include "qobexapparam.h"
#include "qobexauth.h"

#include "qobextransport.h"

#include "qobexclient.h"

//#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

// #define STREAMING_GET
// #define STREAMING_PUT


QObexClient::QObexClient( QObexTransport * transport, QObject* parent, const char* name  )
  : QObexBase( transport, parent, name ) {
  myDebug(( "QObexClient::QObexClient( ... )" ));

  // FIXME review those
  // Client state
  mUseConnectionId = false;
  mCurrentRequest = 0;
  mConnected = false;
  mResponseCode = -1;
  // ????
  mServerIsReadOnly = false;
  mServerIsAuthenticated = false;

  // Client configuration options
  mKeepTransport = false;
  mInitiateAuth = false;
  mOpenObexSupport = false;

}

QObexClient::~QObexClient() {
  myDebug(( "QObexClient::~QObexClient()" ));
}

bool QObexClient::connectClient() {
  myDebug(( "QObexClient::connectClient()" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  // We are already connected. Don't do it twice ...
  if ( mConnected ) {
    error( AlreadyConnected );
    return false;
  }

  mUseConnectionId = false;
  // create a connect request
  QObexObject* req
    = new QObexObject( QObexObject::Connect, QObexObject::ProtocolVersion1, 0, 0 );
  
  // add the connection uuid if required
  if ( !mUUID.isEmpty() )
    req->addHeader( QObexHeader( QObexHeader::Target, mUUID ) );

  // add a authentication request header if required
  if ( mInitiateAuth ) {
    QObexAuthDigestChallenge auth( mClientAuthInfo );
    mPendingAuth.append( auth );
    req->addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
  }
  
  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::disconnectClient() {
  myDebug(( "QObexClient::disconnectClient( ... )" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  // Check if we are connected. Refuse if we are not.
  if ( !mConnected ) {
    error( NotConnected );
    return false;
  }

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( new QObexObject( QObexObject::Disconnect, true ) );
}

// start a get request for the given path
bool QObexClient::get( const QString& path, const QString& mimetype ) {
  myDebug(( "QObexClient::get( ... )" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  QObexObject* req = new QObexObject( QObexObject::Get, true );
  if ( !path.isNull() )
    req->addHeader( QObexHeader( QObexHeader::Name, path ) );
  if ( !mimetype.isNull() )
    req->addHeader( QObexHeader( QObexHeader::Type, mimetype ) );

#ifdef STREAMING_GET
  mStreamingOk = true;
  mStreamingBuffer.clear();
  mStreamingType = StreamingGet;
#endif

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::del( const QString& path ) {
  myDebug(( "QObexClient::del( ... )" ));

  QValueList<QObexHeader> headers;
  
  return del( path, headers );
}

bool QObexClient::del( const QString& path, 
			  const QValueList<QObexHeader>& headers ) {
  myDebug(( "QObexClient::del( ... )" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  QObexObject* req = new QObexObject( QObexObject::Put, true );
  req->addHeader( QObexHeader( QObexHeader::Name, path ) );
  req->appendHeaders( headers );
  
  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::put( const QString& path, Q_UINT32 size,
		       const QString& mimetype ) {
  myDebug(( "QObexClient::put( ... )" ));

  QValueList<QObexHeader> headers;
  if ( 0 < size )
    headers.append( QObexHeader( QObexHeader::Length, size ) );
  if ( !mimetype.isNull() )
    headers.append( QObexHeader( QObexHeader::Type, mimetype ) );

  return put( path, headers );
}

bool QObexClient::put( const QString& path,
		       const QValueList<QObexHeader>& headers ) {
  myDebug(( "QObexClient::put( ... )" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  QObexObject* req = new QObexObject( QObexObject::Put, false );
  if ( !path.isNull() )
    req->addHeader( QObexHeader( QObexHeader::Name, path ) );
  req->appendHeaders( headers );

#ifdef STREAMING_PUT
  mStreamingOk = true;
  mStreamingBuffer.clear();
  mStreamingType = StreamingPut;
#endif

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::setPath( const QString& path, bool backup, bool create ) {
  myDebug(( "QObexClient::setPath( .. )" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  Q_UINT8 flags = ( backup ? QObexObject::Backup : 0 )
    | ( create ? 0 : QObexObject::NoCreate );
  
  QObexObject* req = new QObexObject( QObexObject::SetPath, flags, 0 );
  
  if ( !path.isNull() )
    req->addHeader( QObexHeader( QObexHeader::Name, path ) );

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::setPath( bool backup, bool create ) {
  myDebug(( "QObexClient::setPath()" ));

  // Check if there is a request running.
  if ( mCurrentRequest ) {
    error( NotIdle );
    return false;
  }

  Q_UINT8 flags = ( backup ? QObexObject::Backup : 0 )
    | ( create ? 0 : QObexObject::NoCreate );

  QObexObject* req = new QObexObject( QObexObject::SetPath, flags, 0 );
  
  req->addHeader( QObexHeader( QObexHeader::Name, QString::null ) );

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( req );
}

bool QObexClient::abort() {
  myDebug(( "QObexClient::abort()" ));

  // Check if we are able to send commands.
  // It does not make sense to abort when there is no
  // transport available ...
  if ( !transport()->isConnected() ) {
    error( TransportNotConnected );
    return false;
  }

  // Make sure that the response code is stored.
  mIgnoreResponse = false;
  // Send the request and check if we were able to send.
  return sendRequest( new QObexObject( QObexObject::Abort, true ) );
}

void QObexClient::handleConnectResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handleConnectResponse( ... )" ));
  // Connect response packets are considered invalid if the final bit is not set.
  if ( resp.finalBit() && resp.code() == QObexObject::Success ) {
    // We got a success connect response from the server.
    // Check for the fields in the response.
    if ( resp.getMtu() < QObexTransport::MinimumMtu ) {
      // The MTU in the connect response is too small.
      myDebug(("QObexClient::handleConnectResponse(): ivalid mtu" ));
      mIgnoreResponse = true;
      mResponseCode = -1;
      sendRequest( new QObexObject( QObexObject::Disconnect, true ) );
      error( InvalidPacketRecieved );
    } else if ( resp.getVersion() != QObexObject::ProtocolVersion1
		&& !( mOpenObexSupport && resp.getVersion() == 0x11 ) ) {
      // Check for an error in the protocol version.
      // Note that openobex triggers this error ...
      myDebug(("QObexClient::handleConnectResponse(): ivalid protocol version" ));
      mIgnoreResponse = true;
      mResponseCode = -1;
      sendRequest( new QObexObject( QObexObject::Disconnect, true ) );
      error( InvalidObexVersionRecieved );
    } else {
      // Now we know we have valid extra fields in the connect request
      if ( !mUUID.isEmpty() ) {
	// We want a directed connection
	myDebug(( "QObexClient::handleConnectResponse(): We want a directed connection" ));
	if ( resp.hasHeader( QObexHeader::Who )
	     && resp.hasHeader( QObexHeader::ConnectionId )
	     && resp.getHeader( QObexHeader::Who ).arrayData() == mUUID ) {
	  // The connect response contains the correct who header
	  // and has a connection id.
	  QObexHeader id = resp.getHeader( QObexHeader::ConnectionId );
	  mConnectionId = id.uint32Data();
	  mUseConnectionId = true;
	  myDebug(( "QObexClient::handleConnectResponse(): Using connection id %d", mConnectionId ));
	}
      }
      myDebug(( "QObexClient::handleConnectResponse(): mConnected = true;"));
      mConnected = true;
      makeIdle();
      // tell the user what has happened
      response( resp );
      requestDone(Connect);
    }
  } else {
    // Unsuccessful connect response
    error( CanNotConnect );
    makeIdle();
  }
}

void QObexClient::handleDisconnectResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handleDisconnectResponse( ... )" ));
  // Disconnect response packets are considered invalid if the final
  // bit is not set.
  if ( resp.finalBit() ) {
    makeIdle();
    mConnected = false;
    mUseConnectionId = false;
    response( resp );
    mServerIsAuthenticated = false;
    mServerIsReadOnly = false;
    // Disconnect from transport if we have to
    if ( !mKeepTransport )
      transport()->disconnect();

    requestDone(Disconnect);
  } else {
    error( ServerError );
    makeIdle();
  }
}

void QObexClient::handleGetResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handleGetResponse( ... )" ));
  // Handle get responses.

  // see if we got some data.
#ifdef STREAMING_GET
  mStreamingBuffer += resp.getBodyData();
#else
  QValueList<QByteArray> bodies = resp.getBodyData();
#endif

  if ( resp.code() == QObexObject::Continue ) {
    myDebug(("QObexClient::handleGetResponse() Continue" ));
    // Tell the server that it should continue.
    response( resp );
#ifdef STREAMING_GET
    if ( mStreamingOk )
      sendRequest( new QObexObject( QObexObject::Get, true ) );
    else
      sendRequest( new QObexObject( QObexObject::Abort, true ) );
#else
    bool cont = true;
    if ( !bodies.isEmpty() )
      cont = data( bodies );
    myDebug(("QObexClient::handleGetResponse() cont = %d", cont ));
    if ( cont )
      sendRequest( new QObexObject( QObexObject::Get, true ) );
    else
      sendRequest( new QObexObject( QObexObject::Abort, true ) );
#endif
  } else if ( resp.code() == QObexObject::Success ) {
    myDebug(("QObexClient::handleGetResponse() Success" ));
    // We have finished.
    response( resp );
#ifdef STREAMING_GET
#else
    // don't check the return value of data() since we are ready anyway
    if ( !bodies.isEmpty() )
      data( bodies );
//     emit signalFinished( resp.code() );
#endif
    makeIdle();
    requestDone(Get);
  } else {
    myDebug(("QObexClient::handleGetResponse() Error" ));
    error( GotErrorResponse );
    mIgnoreResponse = true;
    sendRequest( new QObexObject( QObexObject::Abort, true ) );
  }
}

void QObexClient::handlePutResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handlePutResponse( ... )" ));
  // streaming put handling
  if ( resp.code() == QObexObject::Continue ) {
    response( resp );

    // This one is the only one where we add the connection id first.
    // We need this for body size calculation and sendRequest checks anyway.
    QObexObject* req = new QObexObject( QObexObject::Put, false );

#ifdef STREAMING_PUT
    if ( mStreamingOk ) {
      Q_ASSERT( !mStreamingBuffer.isEmpty() );
      myDebug(("QObexClient::handlePutResponse(): dataReq() = true" ));
      QByteArray data = mStreamingBuffer.first();
      mStreamingBuffer.pop_front();
      bool final = (0 == data.size());
      req->setFinalBit( final );
      req->addHeader( QObexHeader( final ? QObexHeader::BodyEnd : QObexHeader::Body, data ) );
      sendRequest( req );
#else
    // We know that we need 3 bytes for the core header, 5 bytes for
    // the connection id and additional 3 bytes for the body
    // header type and length.
    Q_UINT16 reserved = mUseConnectionId ? req->size() + 5 : req->size();
    Q_ULONG maxSize = getOptimumBodySize( reserved );
    QByteArray data;
    if ( dataReq( data, maxSize ) ) {
      myDebug(("QObexClient::handlePutResponse(): dataReq() = true" ));
      
      bool final = (0 == data.size());
      req->setFinalBit( final );
      req->addHeader( QObexHeader( final ? QObexHeader::BodyEnd : QObexHeader::Body, data ) );
      sendRequest( req );
#endif
    } else {
      myDebug(("QObexClient::handlePutResponse(): dataReq() = false" ));
      delete req;
      mIgnoreResponse = true;
      sendRequest( new QObexObject( QObexObject::Abort, true ) );
    }
  } else if ( resp.code() == QObexObject::Success ) {
    response( resp );
    makeIdle();
    requestDone(Put);
  } else {
    makeIdle();
    error( GotErrorResponse );
  }
}

void QObexClient::handleSetPathResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handleSetPathResponse( ... )" ));
  // SetPath response packets are considered invalid if the final bit is not set.
  if ( resp.finalBit() ) {
    response( resp );
    makeIdle();
    requestDone(SetPath);
  } else {
    error( InvalidPacketRecieved );
    makeIdle();
  }
}

void QObexClient::handleAbortResponse( QObexObject& resp ) {
  myDebug(("QObexClient::handleAbortResponse( ... )" ));
  // Abort response packets are considered invalid if the final bit is not set.
  if ( resp.finalBit() ) {
    response( resp );
    makeIdle();
    requestDone(Abort);
  } else {
    error( InvalidPacketRecieved );
    makeIdle();
  }
}

void QObexClient::transportError( QObexTransport::Error err ) {
  myDebug(("QObexClient::transportError( ... )" ));

  // Make sure that we don't report successful termination of the command
  // in case of an error in the transport ...
  makeIdle();
  mResponseCode = -1;
  switch ( err ) {
  case QObexTransport::ConnectionRefused:
    error( CanNotConnect );
    break;
  case QObexTransport::PacketTooBig:
    error( InternalError );
    break;
  case QObexTransport::ConnectionReset:
    // Really???
    // I hink yes, since the transport also resets itself in open state ...
    break;
  default:
    error( TransportError );
    break;
  }
}

void QObexClient::transportConnected() {
  myDebug(("QObexClient::transportConnected()" ));

  // Setup the socket notifiers.
  mReadNotifier
    = new QSocketNotifier( mTransport->socket(), QSocketNotifier::Read,
			   this, "Socketnotifier for transport" );
  mReadNotifier->setEnabled( true );
  QObject::connect( mReadNotifier, SIGNAL(activated(int)),
		    mTransport, SLOT(slotIOReady()) );

  mWriteNotifier
    = new QSocketNotifier( mTransport->socket(), QSocketNotifier::Write,
			   this, "Socketnotifier for transport" );
  mWriteNotifier->setEnabled( false );
  QObject::connect( mWriteNotifier, SIGNAL(activated(int)),
		    mTransport, SLOT(slotIOReady()) );

  // If we have a request waiting to be sent, do it now.
  if ( mCurrentRequest ) {
    if ( mCurrentRequest->getPacketType() == QObexObject::ConnectPacket )
      transport()->setParseHint( QObexObject::ConnectPacket );

    transport()->startCommand( 60000 /*FIXME*/);
    QObexBase::sendObject( mCurrentRequest );
  }
}

void QObexClient::transportConnectionClosed() {
  myDebug(("QObexClient::transportConnectionClosed()" ));
  makeIdle();
  mConnected = false;
  mUseConnectionId = false;
  mServerIsReadOnly = false;

  if(mReadNotifier)
    delete mReadNotifier;
  if(mWriteNotifier)
    delete mWriteNotifier;
}

void QObexClient::transportPacketWritten() {
  myDebug(("QObexClient::transportPacketWritten()" ));
  switch ( mStreamingType ) {
#ifdef STREAMING_GET
  case StreamingGet:
    {
      if ( !mStreamingBuffer.isEmpty() )
	mStreamingOk = data( mStreamingBuffer );
      mStreamingBuffer.clear();
      //     emit signalFinished( resp.code() );
      myDebug(("QObexClient::transportPacketWritten() cont = %d", mStreamingOk ));
    }
    break;
#endif
#ifdef STREAMING_PUT
  case StreamingPut:
    {
      // We know that we need 3 bytes for the core header, 5 bytes for
      // the connection id and additional 3 bytes for the body
      // header type and length.
      Q_UINT16 reserved = mUseConnectionId ? 3 + 5 : 3;
      Q_ULONG maxSize = getOptimumBodySize( reserved );
      QByteArray data;
      mStreamingOk = dataReq( data, maxSize );
      mStreamingBuffer.append( data );
    }
    break;
#endif
  default:
    break;
  }
}

void QObexClient::transportSelectModeChanged( QObexTransport::SelectHint hint, Q_LONG ) {
  myDebug(( "QObexClient::transportSelectModeChanged( ... )" ));
  // FIXME no timeouts at the moment
  if ( hint == QObexTransport::SelectRead ) {
    mWriteNotifier->setEnabled( false );
    mReadNotifier->setEnabled( true );
  } else if ( hint == QObexTransport::SelectWrite ) {
    mWriteNotifier->setEnabled( true );
    mReadNotifier->setEnabled( false );
  } else {
    mWriteNotifier->setEnabled( false );
    mReadNotifier->setEnabled( false );
  }
}


void QObexClient::transportPacketRecieved( QObexObject& resp ) {
  myDebug(("QObexClient::transportPacketRecieved( obj )" ));

  // Save the response code of the first packet of a request.
  // Except it was an authentication resend.
  if ( !mIgnoreResponse )
    mResponseCode = resp.code();

  if ( !mCurrentRequest ) {
    error( InternalError );
    return;
  }

  myDebug(("QObexClient::transportPacketRecieved() at %d", __LINE__));
  if ( !resp.isValid() ) {
    makeIdle();
    error( InvalidPacketRecieved );
    return;
  }
  myDebug(("QObexClient::transportPacketRecieved() at %d, response code = %s",
	   __LINE__, resp.stringCode().ascii() ));
  
  // Check if the server requires authentication.
  if ( resp.code() == QObexObject::Unauthorized ) {
    if ( resp.hasHeader( QObexHeader::AuthChallenge ) ) {
      // The server wants that we authenticate ourself to the server.
      // Compute the digest answer and resend the previous request.
      QObexHeader authHdr = resp.getHeader( QObexHeader::AuthChallenge );
      QObexAuthDigestChallenge authChal( authHdr.arrayData() );
      mServerIsReadOnly = authChal.readOnly();
      // Callback: Give the callee the opportunity to set the correct secret.
      // and a serveruser 
      authenticationRequired( authChal.realm(), authChal.userIdRequired() );
      QObexAuthDigestResponse authResp( authChal.nonce(), mServerAuthInfo );
      mCurrentRequest->addHeader( QObexHeader( QObexHeader::AuthResponse, authResp ) );

      if ( mCurrentRequest->getPacketType() == QObexObject::ConnectPacket )
	transport()->setParseHint( QObexObject::ConnectPacket );
      QObexBase::sendObject( mCurrentRequest );
      return;
    }

    // if we don't get a challenge header we are not authorized ...
    makeIdle();
    error( AuthenticationRequired );
    return;
  }

  // Check if the server is correctly authenticated.
  if ( resp.hasHeader( QObexHeader::AuthResponse ) ) {
    QObexHeader authHdr = resp.getHeader( QObexHeader::AuthResponse );
    QObexAuthDigestResponse auth( authHdr.arrayData() );
    mServerIsAuthenticated = authResponseOk( auth );
  }

  // Check if we got an abort packet.
  if ( resp.code() == QObexObject::Abort ) {
    makeIdle();
    aborted( resp );
    return;
  }

  // Now we know that the packet is valid so far.
  // Each request needs its special handling ...
  switch ( mCurrentRequest->code() ) {
  case QObexObject::Connect:
    handleConnectResponse( resp );
    break;
  case QObexObject::Disconnect:
    handleDisconnectResponse( resp );
    break;
  case QObexObject::Get:
    handleGetResponse( resp );
    break;
  case QObexObject::Put:
    handlePutResponse( resp );
    break;
  case QObexObject::SetPath:
    handleSetPathResponse( resp );
    break;
  case QObexObject::Abort:
    handleAbortResponse( resp );
    break;
  default:
    // Ups this is an error ...
    error( InternalError );
    makeIdle();
    break;
  }
}

void QObexClient::makeIdle() {
  myDebug(("QObexClient::makeIdle()" ));
  if ( mCurrentRequest )
    delete mCurrentRequest;
  mCurrentRequest = 0;
}

bool QObexClient::sendRequest( QObexObject* req ) {
  myDebug(( "QObexClient::sendRequest( ... )" ));

  // If you would call this function with a 0 argument you would better
  // call makeIdle()
  Q_ASSERT( req != 0 );
  if ( mUseConnectionId )
    req->addHeader( QObexHeader( QObexHeader::ConnectionId, mConnectionId ) );

  if ( mCurrentRequest ) {
    // In this case we are already in this function. Don't
    // create a long recursion. Simply send the object and return if
    // sending was successful. We expect that the transport is
    // connected here, since there is a request runing.
    delete mCurrentRequest;
    mCurrentRequest = req;
    
    if ( req->getPacketType() == QObexObject::ConnectPacket )
      transport()->setParseHint( QObexObject::ConnectPacket );

    // We expect an error signal from the transport if we can not send.
   	 transport()->startCommand( 60000 /*FIXME*/);
    return QObexBase::sendObject( req );
  } else {
    // Here a completely new request is started.
    // Go in the event loop if we are a synchronous client.

    // The first response is important, thus signal that
    // we should store the next code we get.
    mResponseCode = -1;
    mCurrentRequest = req;
    // Check if we are able to send commands.
    if ( transport()->isConnected() ) {
      // We expect an error signal from the transport if we can not send.
      if ( req->getPacketType() == QObexObject::ConnectPacket )
	transport()->setParseHint( QObexObject::ConnectPacket );
	myDebug(("Transport_connected: %i",transport()->isConnected()));
      transport()->startCommand( 60000 /*FIXME*/);
      if ( !QObexBase::sendObject( req ) )
	return false;
      
    } else {
      // If transport is not connected, connect to it.
      // If we have a request to process, transportConnected()
      // will send the request.
      if ( !transport()->connect() )
	return false;
    }

    // If we are a synchronous client, loop here until the request has finished
    // or the transport is disonnected.
    if ( transport()->blocking() ) {
      myDebug(( "QObexClient::sendRequest(): transport()->blocking(): TRUE" ));
      while ( mCurrentRequest ) {
 	myDebug(( "QObexClient::sendRequest() at line %d", __LINE__ ));
	int ret = transport()->doSelect();
	if ( ret < 0 )
	  break;
	else if ( ret == 0 )
	  transport()->slotTimeout();
	else
	  transport()->slotIOReady();
      }

      myDebug(( "QObexClient::sendRequest() at line %d", __LINE__ ));
      // in blocking operations we can check this ...
      Q_ASSERT( mStreamingBuffer.isEmpty() );
      return isSuccessfull();
    } else {
      myDebug(( "QObexClient::sendRequest(): transport()->blocking(): FALSE" ));
      return true;
    }
  }
}

bool QObexClient::authResponseOk( const QObexAuthDigestResponse& auth ) {
  if ( mPendingAuth.isEmpty() )
    return false;
  
  if ( auth.hasNonce() ) {
    QByteArray nonce = auth.nonce();
    QValueList<QObexAuthDigestChallenge>::Iterator it;
    for ( it = mPendingAuth.begin(); it != mPendingAuth.end(); ++it ) {
      if ( (*it).nonce() == nonce ) {
	// FIXME may be something like clientSecret( auth.userId() )
	if ( auth.authenticate( mClientSecret, nonce ) ) {
	  mPendingAuth.clear();
	  return true;
	}
      }
    }
  }
  else {
    if ( auth.authenticate( mClientSecret, mPendingAuth.first().nonce() ) ) {
      mPendingAuth.clear();
      return true;
    }
  }
  
  mPendingAuth.clear();
  return false;
}

#include "qobexclient.moc"
