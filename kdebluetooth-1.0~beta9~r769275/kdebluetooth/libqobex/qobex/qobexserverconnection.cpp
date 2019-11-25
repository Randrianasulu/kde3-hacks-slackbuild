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

#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <qptrlist.h>

#include <qsocketnotifier.h>
#include <qobject.h>
#include <qmap.h>
#include <qcstring.h>

#include "qobexheader.h"
#include "qobexauth.h"
#include "qobexobject.h"
#include "qobextransport.h"

#include "qobexbase.h"

#include "qobexserverconnection.h"
#include "qobexnullserverops.h"
#include "qobexserverops.h"

#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexServerConnection::QObexServerConnection( QObexTransport* transport, QObject* parent, const char* name )
  : QObexBase( transport, parent, name ) {
  myDebug(("QObexServerConnection::QObexServerConnection( ... )" ));
  
  // We don't want to block the whole application ...
  transport->setBlocking( false );
  
  mReadNotifier
    = new QSocketNotifier( transport->socket(), QSocketNotifier::Read,
			   this, "Socketnotifier for transport" );
  mReadNotifier->setEnabled( true );
  QObject::connect( mReadNotifier, SIGNAL(activated(int)),
		    transport, SLOT(slotIOReady()) );
  
  mWriteNotifier
    = new QSocketNotifier( transport->socket(), QSocketNotifier::Write,
			   this, "Socketnotifier for transport" );
  mWriteNotifier->setEnabled( false );
  QObject::connect( mWriteNotifier, SIGNAL(activated(int)),
		    transport, SLOT(slotIOReady()) );

  mPendingAuth.setAutoDelete( true );

  // connectionid's are random
  ::srand( ::time( 0 ) + ::rand() );
}

QObexServerConnection::~QObexServerConnection() {
  myDebug(("QObexServerConnection::~QObexServerConnection()" ));
  QMap<Q_UINT32,QObexServerOps*>::Iterator it;
  while ( it = mServerOps.begin(), it != mServerOps.end() ) {
    delete it.data();
    mServerOps.remove( it );
  }
}

void QObexServerConnection::transportConnected() {
  myDebug(("QObexServerConnection::transportConnected()" ));
}

void QObexServerConnection::transportConnectionClosed() {
  myDebug(("QObexServerConnection::transportConnectionClosed()" ));
  
  QMap<Q_UINT32,QObexServerOps*>::Iterator it;
  while ( it = mServerOps.begin(), it != mServerOps.end() ) {
    if ( it.data()->isConnected() )
      it.data()->disconnect( QObexObject() );
    delete it.data();
    mServerOps.remove( it );
  }
  deleteLater();
}

bool QObexServerConnection::errorResponse( QObexServerOps* serverOps, Q_UINT8 code, const char* desc ) {
  if ( serverOps ) {
    if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
      serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    serverOps->mHeaders.clear();
    serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
    serverOps->mPendingAuth.clear();
  }
  QObexObject resp( code, true );
  myDebug(("QObexServerConnection::errorResponse( %s, %s )",
	   resp.stringCode().ascii(), desc ? desc : "" ));
  if ( desc )
    resp.addHeader( QObexHeader( QObexHeader::Description, desc ) );
  return sendObject( &resp );
}

bool QObexServerConnection::connectErrorResponse( QObexServerOps* serverOps, Q_UINT8 code, const char* desc ) {
  if ( serverOps ) {
    serverOps->mHeaders.clear();
    serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
    serverOps->mPendingAuth.clear();
    serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;

    serverOps->mUuid = QByteArray();
    serverOps->mConnectionId = 0;
    serverOps->mConnected = false;
    serverOps->mClientIsReadOnly = false;
  }
  QObexObject resp( code, QObexObject::ProtocolVersion1, 0,
		    QObexTransport::MinimumMtu );
  myDebug(("QObexServerConnection::connectErrorResponse( %s, %s )",
	   resp.stringCode().ascii(), desc ? desc : "" ));
  if ( desc )
    resp.addHeader( QObexHeader( QObexHeader::Description, desc ) );
  return sendObject( &resp );
}

void QObexServerConnection::transportPacketRecieved( QObexObject& req ) {
  myDebug(( "QObexServerConnection::slotTransportPacketRecieved()" ));
  
  // Check if the packet received is valid so far.
  if ( !req.isValid() ) {
    errorResponse( 0, QObexObject::BadRequest, "Got invalid OBEX packet" );
    return;
  }
  
  // Check if the final bit is set.
  if ( !req.finalBit() && req.code() == QObexObject::Get && req.code() != QObexObject::Put ) {
    // We got an invalid request. Tell this the client.
    errorResponse( 0, QObexObject::BadRequest, "Final Bit not set" );
    return;
  }
  
  //////////////////////////////////////////////////////////////////////////
  // BEGIN CONNECT HANDLING
  //////////////////////////////////////////////////////////////////////////
  // At first do connect packet handling since it is different with respect
  // to the handling of the serverOps pointer.
  if ( req.code() == QObexObject::Connect ) {
    if ( req.getVersion() != QObexObject::ProtocolVersion1 ) {
      // Can only handle protocol version 1.0.
      connectErrorResponse( 0, QObexObject::BadRequest,
			    "Unsupported OBEX protocol version" );
      return;
    }
    if ( req.getMtu() < QObexTransport::MinimumMtu ) {
      // MTU must be at least 255
      connectErrorResponse( 0, QObexObject::BadRequest, "Invalid MTU" );
      return;
    }
    if ( req.hasHeader( QObexHeader::ConnectionId ) ) {
      // Connect packets shall not have a connection id.
      connectErrorResponse( 0, QObexObject::BadRequest,
			    "Connect request with connection id!" );
      return;
    }

    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got a valid connect packet!" ));

    // Get the target. Is empty if the header does not exist.
    QByteArray target = req.getHeader( QObexHeader::Target ).arrayData();


    QObexServerOps* serverOps = 0;
    // Check for an authentication response header.
    if ( req.hasHeader( QObexHeader::AuthResponse ) ) {
      // at the moment we support only one auth response header ...
      QObexAuthDigestResponse auth = req.getHeader( QObexHeader::AuthResponse ).arrayData();

      // search for a set of server ops in the list of pending connect
      // authentications which fits to the given response ...
      for ( Q_ULONG i = 0; i < mPendingAuth.count(); ++i ) {
	// Try oonly those pending requests which are responsible for the
	// same target id.
	if ( mPendingAuth.at( i )->canHandle( target ) ) {
	  // If we have a nonce in the auth response try only those
	  // which match this nonce.
	  if ( auth.hasNonce() &&
	       mPendingAuth.at( i )->havePendingAuthForNonce( auth.nonce() ) ) {
	    serverOps = mPendingAuth.take( i );
	    // Check if the secret matches. It is an authentication failure if
	    // this fails.
	    if ( !serverOps->authResponseOk( auth ) ) {
	      delete serverOps;
	      connectErrorResponse( 0, QObexObject::Unauthorized,
				    "Secret does not match!" );
	      return;
	    }
	    break;
	  }
	  else {
	    // Check if the secret matches. Since we don't know the
	    // original nonce we have to try every pending auth here.
	    // A faliure here is nonfatal since it only means that we have not
	    // found the oroginal nonce.
	    // FIXME is this a security hole???
	    if ( mPendingAuth.at( i )->authResponseOk( auth ) ) {
	      serverOps = mPendingAuth.take( i );
	      break;
	    }
	  }
	}
      }
      // if we cant find one something is wrong.
      if ( !serverOps ) {
 	connectErrorResponse( 0, QObexObject::BadRequest, "No pending Authentication Challenge or wrong secret!" );
 	return;
      }

      serverOps->mAuthStatus = QObexServerOps::Authenticated;
    }
    else if ( 0 == target.size() && mServerOps.contains( 0 ) ) {
      if ( mServerOps[0]->isConnected() ) {
	// The inbox is already connected.
	// Can not multiplex without connection id.
	connectErrorResponse( 0, QObexObject::BadRequest, "Already connected!" );
	return;
      }
      serverOps = mServerOps[0];
    }
    else {
      // Get a set of server ops matching the target uuid
      serverOps = getOpsForUuid( target );
    }
    // We are sure here to have a set of server ops.
    Q_ASSERT( serverOps != 0 );

    // Default response code. The serverops can overwrite them.
    Q_UINT8 defaultCode = req.hasHeader( QObexHeader::AuthChallenge ) &&
      !req.hasHeader( QObexHeader::AuthResponse )
      ? QObexObject::Unauthorized : QObexObject::Success;

    // Create a response packet. By default we use protocol version 1.0
    // and we support multiplexing several connections with one transport.
    QObexObject resp( defaultCode, QObexObject::ProtocolVersion1,
		      QObexObject::TransportMultiplexing,
		      min( req.getMtu(), getOptimumMtu() ) );

    // Call the client ops ...
    serverOps->connect( req, resp );
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got %s from server ops!", resp.stringCode().ascii() ));

    // Now that we know what the serveroops want to have ...
    if ( resp.code() == QObexObject::Success ) {
      serverOps->mConnected = true;

      if ( target.size() && serverOps->canHandle( target ) ) {
	// We have a server for a given uuid
	Q_UINT32 cid = allocConnectionId();
	myDebug(("QObexServerConnection::slotTransportPacketRecieved(): "
		 "Assigning connection id = %u", cid ));
	
	mServerOps[cid] = serverOps;
	serverOps->mUseConnectionId = true;
	serverOps->mConnectionId = cid;
	serverOps->mUuid = target;
	resp.addHeader( QObexHeader( QObexHeader::Who, target ) );
	resp.addHeader( QObexHeader( QObexHeader::ConnectionId, cid ) );
      }
      else {
	// The ones for the inbox connection.
	mServerOps[0] = serverOps;
      }

      // Append ahtentication response header if required.
      appendAuthResponse( req, resp, serverOps );
    }
    else if ( resp.code() == QObexObject::Unauthorized &&
	      mPendingAuth.count() < MaxPendingAuthentications ) {
      // Client is not connected
      QObexAuthDigestChallenge auth( serverOps->serverAuthInfo() );
      serverOps->mPendingAuth.append( auth );
      mPendingAuth.append( serverOps );
      resp.addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
    }
    else {
      // Client is not connected
      delete serverOps;
    }
    
    // finally send the response.
    sendObject( &resp );
    return;
  }
  //////////////////////////////////////////////////////////////////////////
  // END CONNECT HANDLING
  //////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////
  // COMMON CODE FOR NON CONNECT PACKETS
  //////////////////////////////////////////////////////////////////////////
  Q_ASSERT( req.code() != QObexObject::Connect );
  QObexServerOps *serverOps = 0;
  // Check if we already have serverops for this connection.
  if ( req.hasHeader( QObexHeader::ConnectionId ) ) {
    Q_UINT32 cid = req.getHeader( QObexHeader::ConnectionId ).uint32Data();
    // We don't assign the 0 connection id since we use it internally
    // for connectionless operation or inbox connections.
    if ( cid != 0 && mServerOps.contains( cid ) )
      serverOps = mServerOps[cid];
    else {
      errorResponse( 0, QObexObject::BadRequest, "Unknown connection id" );
      return;
    }
  }
  else {
    // No connection id.
    if ( !mServerOps.contains( 0 ) )
      mServerOps[0] = getOpsForUuid( QByteArray() );
    serverOps = mServerOps[0];
  }
  // At this point we have nonzero serverOps.
  Q_ASSERT( serverOps != 0 );

  // Check if we are successfully authenticated now.
  if ( req.hasHeader( QObexHeader::AuthResponse ) ) {
    if ( serverOps->mPendingAuth.isEmpty() ) {
      errorResponse( serverOps, QObexObject::BadRequest, "No Authentication challenge sent" );
      return;
    }

    QByteArray data = req.getHeader( QObexHeader::AuthResponse ).arrayData();
    if ( serverOps->authResponseOk( QObexAuthDigestResponse( data ) ) )
      serverOps->mAuthStatus = QObexServerOps::TempAuthenticated;
    else
      serverOps->mAuthStatus = QObexServerOps::AuthenticationFailed;
  }
  //////////////////////////////////////////////////////////////////////////
  // END COMMONE CODE FOR NON CONNECT PACKETS
  //////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////
  // ABORT PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::Abort ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved(): "
	      "Got abort packet!" ));
    serverOps->abort( req );
    QObexObject resp( QObexObject::Success, true );
    sendObject( &resp );
    serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
    if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
      serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    return;
  }


  //////////////////////////////////////////////////////////////////////////
  // DISCONNECT PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::Disconnect ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved(): "
	      "Got disconnect packet!" ));

    serverOps->disconnect( req );
    serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
    serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    QObexObject resp( QObexObject::Success, true );
    sendObject( &resp );
    // If we disconnect from a directed connection remove server ops.
    if ( req.hasHeader( QObexHeader::ConnectionId ) ) {
      Q_UINT32 cid = req.getHeader( QObexHeader::ConnectionId ).uint32Data();
      // just to be sure that no one can nuke our inbox server ops ...
      if ( cid != 0 ) {
	Q_ASSERT( mServerOps[cid] == serverOps );
	delete serverOps;
	mServerOps.remove( cid );
      }
    }
    return;
  }

  
  //////////////////////////////////////////////////////////////////////////
  // CHECK FOR FAILD AUTHENTICATION AND RESPOND UNAUTHORIZED
  //////////////////////////////////////////////////////////////////////////
  if ( serverOps->mAuthStatus == QObexServerOps::AuthenticationFailed ) {
    errorResponse( serverOps, QObexObject::Unauthorized, "Authentication failed, please reconnect" );
    return;
  }

  // Check for unexpected packets during streaming.
  if ( ( ( serverOps->mStreamingStatus == QObexServerOps::StreamingPut
	   || serverOps->mStreamingStatus == QObexServerOps::PreparingPut )
	 && req.code() != QObexObject::Put )
       || ( ( serverOps->mStreamingStatus == QObexServerOps::StreamingGet
	      || serverOps->mStreamingStatus == QObexServerOps::PreparingGet )
	    && req.code() != QObexObject::Get ) ) {
    serverOps->streamingError();
    errorResponse( serverOps, QObexObject::BadRequest, "Unexpected request in streaming operation" );
    if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
      serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    return;
  }


  //////////////////////////////////////////////////////////////////////////
  // GET PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::Get ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got get packet!" ));
    // Just to be sure ... FIXME 
//     if ( serverOps->mStreamingStatus == QObexServerOps::NoStreaming ) {
//       serverOps->mHeaders.clear();
//     }

    if ( !req.finalBit() ) {
      if ( serverOps->mStreamingStatus == QObexServerOps::StreamingGet ) {
	serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
	errorResponse( serverOps, QObexObject::BadRequest, "Final bit not set in streaming operation" );
	return;
      }

      // As long as the request description is not complete dump
      // the headers here.
      serverOps->mStreamingStatus = QObexServerOps::PreparingGet;
      serverOps->mHeaders += req.getHeaders();

      QObexObject resp( QObexObject::Continue, true );
      sendObject( &resp );
      return;
    }

    // Final bit is set in the request ...

    QObexObject resp( QObexObject::Continue, true );
    if ( serverOps->mStreamingStatus == QObexServerOps::NoStreaming
	 || serverOps->mStreamingStatus == QObexServerOps::PreparingGet ) {
      // Create a fake request object containing all the collected
      // headers.
      req.prependHeaders( serverOps->mHeaders );
      
      // Ask the serverops what to do.
      serverOps->get( req, resp );

      if ( resp.code() == QObexObject::Unauthorized ) {
	QObexAuthDigestChallenge auth( serverOps->serverAuthInfo() );
	serverOps->mPendingAuth.append( auth );
	resp.addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
      }
      else if ( resp.code() == QObexObject::Continue ) {
	serverOps->mStreamingStatus = QObexServerOps::StreamingGet;
	serverOps->mHeaders.clear();
      }
      else {
	serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
	serverOps->mHeaders.clear();
      }
    }

    // Ok, ready to send data...
    if ( serverOps->mStreamingStatus == QObexServerOps::StreamingGet ) {
      QByteArray buffer;
      if ( !serverOps->dataReq( buffer, getOptimumBodySize( resp.size() ) ) ) {
	errorResponse( serverOps, QObexObject::Abort );
	return;
      }

      Q_UINT8 hdrType = QObexHeader::Body;
      // FIXME signal error
      if ( !buffer.size() ) {
	resp.setCode( QObexObject::Success );
	hdrType = QObexHeader::BodyEnd;
	serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
      }
      resp.addHeader( QObexHeader( hdrType, buffer ) );
    }

    sendObject( &resp );
    return;
  }
    
  //////////////////////////////////////////////////////////////////////////
  // PUT PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::Put ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got put packet!" ));

    // We will collect headers as long as we don't see a body header.
    // If we get a put without body headers it is meant to be a delete.
    if ( !req.finalBit() &&
	 !req.hasHeader( QObexHeader::Body ) &&
	 !req.hasHeader( QObexHeader::BodyEnd ) &&
	 serverOps->mStreamingStatus != QObexServerOps::StreamingPut ) {
      
      // As long as we see no bodies domp the headers here.
      serverOps->mStreamingStatus = QObexServerOps::PreparingPut;
      serverOps->mHeaders += req.getHeaders();
      
      QObexObject resp( QObexObject::Continue, true );
      sendObject( &resp );
      return;
    }

    // Assemble the request with all non body headers.
    if ( !serverOps->mHeaders.isEmpty() )
      req.prependHeaders( serverOps->mHeaders );

    // Check if it is a delete request
    if ( req.finalBit() &&
	 serverOps->mStreamingStatus != QObexServerOps::StreamingPut &&
	 !req.hasHeader( QObexHeader::Body ) &&
	 !req.hasHeader( QObexHeader::BodyEnd ) ) {
      myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
		" It is meat to delete." ));
      
      // Puh, ok, a put request with no body data.
      QObexObject resp( QObexObject::Success, true );
      serverOps->del( req, resp );
      
      if ( resp.code() == QObexObject::Unauthorized ) {
	// Authentication required ...
	QObexAuthDigestChallenge auth( serverOps->serverAuthInfo() );
	serverOps->mPendingAuth.append( auth );
	resp.addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
      }
      else {
	serverOps->mHeaders.clear();
	serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
      }
      sendObject( &resp );
      if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
	serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
      
      return;
    }
    
    // Completion of the put request.
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " It is meat put." ));
    QObexObject resp( req.finalBit() ? QObexObject::Success : QObexObject::Continue, true );

    serverOps->put( req, resp );
    
    if ( resp.code() == QObexObject::Unauthorized
	 && !serverOps->isAuthenticated() ) {
      // Authentication required ...
      QObexAuthDigestChallenge auth( serverOps->serverAuthInfo() );
      serverOps->mPendingAuth.append( auth );
      resp.addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
    }
    else {
      // If not unauthorized clear the collected headers since they 
      // are processed now.
      serverOps->mHeaders.clear();
    }
    // FIXME error condition??
    if ( !serverOps->data( req.getBodyData() ) ) {
      errorResponse( serverOps, QObexObject::Abort );
      return;
    }

    if ( resp.code() == QObexObject::Continue ) {
      serverOps->mStreamingStatus = QObexServerOps::StreamingPut;
    }
    else if ( resp.code() == QObexObject::Success ) {
      serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
      if ( !serverOps->data( QValueList<QByteArray>() ) ) {
	errorResponse( serverOps, QObexObject::Abort );
	return;
      }
    }
    else {
      serverOps->mStreamingStatus = QObexServerOps::NoStreaming;
    }

    sendObject( &resp );
    return;
  }
    
  //////////////////////////////////////////////////////////////////////////
  // SETPATH PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::SetPath ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got SetPath packet!" ));
    QObexObject resp( QObexObject::Success, true );
    serverOps->setPath( req, resp );
    if ( resp.code() == QObexObject::Unauthorized ) {
      // Authentication required ...
      QObexAuthDigestChallenge auth( serverOps->serverAuthInfo() );
      serverOps->mPendingAuth.append( auth );
      resp.addHeader( QObexHeader( QObexHeader::AuthChallenge, auth ) );
    }
    sendObject( &resp );
    if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
      serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  // SESSION PACKETS
  //////////////////////////////////////////////////////////////////////////
  if ( req.code() == QObexObject::Session ) {
    myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	      " Got Session packet!" ));
    errorResponse( serverOps, QObexObject::NotImplemented,
		   "Sorry, but sessions are not implemented!" );
    if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
      serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  // ERROR UNKNOWN PACKET
  //////////////////////////////////////////////////////////////////////////
  myDebug(( "QObexServerConnection::slotTransportPacketRecieved():"
	    " Got unknown packet!" ));
  errorResponse( serverOps, QObexObject::BadRequest );
  if ( serverOps->mAuthStatus == QObexServerOps::TempAuthenticated )
    serverOps->mAuthStatus = QObexServerOps::UnAuthenticated;
}

void QObexServerConnection::transportPacketWritten() {
  myDebug(( "QObexServerConnection::transportPacketWritten()" ));
}

void QObexServerConnection::transportError( QObexTransport::Error e ) {
  myDebug(( "QObexServerConnection::transportError( %d )", int( e ) ));
}

void QObexServerConnection::transportConnectionAccept() {
  myDebug(( "QObexServerConnection::transportConnectionAccept()" ));
  emit signalNewConnection( transport() );
}

void QObexServerConnection::transportSelectModeChanged( QObexTransport::SelectHint hint, Q_LONG ) {
  myDebug(( "QObexServerConnection::transportSelectModeChanged( ... )" ));
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


void QObexServerConnection::appendAuthResponse( const QObexObject& req, QObexObject& resp, QObexServerOps* serverOps ) {
  myDebug(( "QObexServerConnection::appendAuthResponse()" ));
  if ( resp.code() == QObexObject::Success
       && req.hasHeader( QObexHeader::AuthChallenge ) ) {
    // The client wants that we authenticate ourself to the client.
    // Compute the digest answer and resend the previous request.
    QObexHeader authHdr = req.getHeader( QObexHeader::AuthChallenge );
    QObexAuthDigestChallenge authChal( authHdr.arrayData() );
    // Don't know if this makes sense, but make this information
    // accessible to the server ops ...
    serverOps->mClientIsReadOnly = authChal.readOnly();
    
    // Get the secret and user for realm.
    QObexAuthDigestResponse::AuthInfo ai =
      serverOps->clientAuthInfo( authChal.realm(), authChal.userIdRequired() );
    QObexAuthDigestResponse authResp( authChal.nonce(), ai );
    resp.addHeader( QObexHeader( QObexHeader::AuthResponse, authResp ) );
  }
}
      
Q_UINT32 QObexServerConnection::allocConnectionId() {
  myDebug(( "QObexServerConnection::allocConnectionId()" ));
  // Get a new connection id. 0 is internally reserved for connectionless
  // operation and inbox connections. Since the server assigns connection ids
  // it is save to do so.
  Q_UINT32 id;
  do {
    id = ::rand();
  } while ( mServerOps.contains( id ) && id == 0 );
  return id;
}

QObexServerOps* QObexServerConnection::getOpsForUuid( const QByteArray& uuid ) {
  myDebug(( "QObexServerConnection::getOpsForUuid( ... )" ));
  QObexServerOps* ops = 0;
  signalRequestServerOps( uuid, &ops );
  if ( !ops )
    ops = new QObexNullServerOps;
  return ops;
}

#include "qobexserverconnection.moc"
