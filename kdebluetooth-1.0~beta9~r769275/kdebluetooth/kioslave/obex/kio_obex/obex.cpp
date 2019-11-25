/*
    This file is part of kio_obex.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qobject.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qmap.h>
#include <dcopclient.h>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>
#include <kurl.h>
#include <kconfig.h>
#include <kmimetype.h>

#include <qobex/qobexclient.h>
#include <qobex/qobexheader.h>
#include <qobex/qobexapparam.h>
#include <qobex/qobexbfbtransport.h>
#include <qobex/qobexericssontransport.h>
#include <qobex/qobexserialtransport.h>
#include <qobex/qobexintransport.h>
#ifdef HAVE_QOBEX_BLUETOOTH
#include <qobex/qobexbttransport.h>
#endif
#ifdef HAVE_QOBEX_IRDA
#include <qobex/qobexirdatransport.h>
#endif
#include <qobex/qobexuuid.h>

#include "obex.h"

#define myDebug kdDebug(7127) << "pid = " << ::getpid() << " "

using namespace KIO;

extern "C" int kdemain( int argc, char **argv );

int
kdemain( int argc, char **argv ) {
  myDebug << "*** Starting kio_obex " << endl;

  KInstance instance( "kio_obex" );

  if (argc != 4) {
    myDebug << "Usage: kio_obex protocol domain-socket1 domain-socket2" << endl;
    exit(-1);
  }
  
  ObexProtocol slave(argv[2], argv[3], instance.config());
  slave.dispatchLoop();
  
  myDebug << "*** kio_obex done" << endl;
  return 0;
}

ObexProtocol::ObexProtocol(const QCString &pool_socket, const QCString &app_socket, KConfig* hc)
  : SlaveBase("kio_obex", pool_socket, app_socket) {
  myDebug << "ObexProtocol::ObexProtocol(" << pool_socket << ", "
	  << app_socket << ", " << hc << ")" << endl;

  mClient = 0;
  mOverridePerm = 0;
  mConnectionOriented = false;
  mCommandType = Idle;
  mDisconnectTimeout = DefaultDisconnectTimeout;

  // If these are not true something really wired has happend
  struct passwd *p = getpwuid( getuid() );
  if ( p ) {
    userName.setAscii( p->pw_name );
    struct group *g = getgrgid( p->pw_gid );
    if ( g )
      groupName.setAscii( g->gr_name );
  }
}

ObexProtocol::~ObexProtocol()
{
  myDebug << "ObexProtocol::~ObexProtocol()" << endl;
  if ( mClient )
    delete mClient;
}

// Is called if the url changes ...
void ObexProtocol::setHost( const QString& host, int port,
			    const QString& user, const QString& pass ) {
  myDebug << "ObexProtocol::setHost(" << host << ", " << port << ", "
	  << user << ", " << pass << ")" << endl;

  // Do a fast return if possible
  if ( mCurrentHost == host && mCurrentPort == port &&
       mCurrentUser == user && mCurrentPass == pass &&
       mClient )
    return;

  mCurrentHost = host;
  mCurrentPort = port;
  mCurrentUser = user;
  mCurrentPass = pass;

  // Destroy old client.
  if ( mClient )
    delete mClient;
  mClient = 0;

  // Clear all cached UDSEntries.
  mUDSEntryCache.clear();
  mCwd = QString::null;
  mCommandType = Idle;

  // Get all parameters and store them.
  mParamMap.clear();

  KConfig conf( "obexrc" );
  if ( conf.hasGroup( host ) ) {
    conf.setGroup( host );
    
    QString transport = conf.readEntry( "transport" );
    mParamMap.insert( "transport", transport );
    mParamMap.insert( "overridePerms", conf.readEntry( "overridePerms", "0" ) );
    mParamMap.insert( "disconnectTimeout", conf.readEntry( "disconnectTimeout", "1" ) );
    mParamMap.insert( "uuid", conf.readEntry( "uuid", "fbs" ) );
    if ( conf.hasKey( "serverSecret" ) )
      mParamMap.insert( "serverSecret", conf.readEntry( "serverSecret", QString::null ) );
    if ( conf.hasKey( "serverUserId" ) )
      mParamMap.insert( "serverUserId", conf.readEntry( "serverUserId", QString::null ) );
    
    if ( transport == "ip" ) {
      mParamMap.insert( "host", conf.readEntry( "peer" ) );
      mParamMap.insert( "port", conf.readEntry( "ipPort" ) );
    }
    else if ( transport == "irda" || transport == "bluetooth" ) {
      mParamMap.insert( "host", conf.readEntry( "peer" ) );
    }
    else if ( transport == "serial" ||
	      transport == "serialsiemens" ||
	      transport == "serialericsson" ) {
      mParamMap.insert( "device", conf.readEntry( "device" ) );
      mParamMap.insert( "speed", conf.readEntry( "speed" ) );
      mParamMap.insert( "rfCommChannel", conf.readEntry( "rfCommChannel" ) );
    }
    
  } else {
    bool matched = false;
#ifdef HAVE_QOBEX_IRDA
    if ( host.contains( ':' ) == 3 ) {
      QString tmphost = host;
      Q_UINT32 tmp = QObexIrDATransport::string2Address( tmphost );
      if ( tmphost.upper() == QObexIrDATransport::address2String( tmp ) ) {
	mParamMap.insert( "transport", "irda" );
	mParamMap.insert( "host", tmphost );
	matched = true;
      }
    }
    if ( host.contains( '-' ) == 3 ) {
      // We got an irda hardware address.
      QString tmphost = host;
      Q_UINT32 tmp = QObexIrDATransport::string2Address( tmphost.replace( '-', ':' ) );
      if ( tmphost.upper() == QObexIrDATransport::address2String( tmp ) ) {
	mParamMap.insert( "transport", "irda" );
	mParamMap.insert( "host", tmphost );
	matched = true;
      }
    }
#endif
    
#ifdef HAVE_QOBEX_BLUETOOTH
    if ( host.contains( ':' ) == 5 ) {
      QString tmphost = host;
      bdaddr_t tmp = QObexBtTransport::string2Address( tmphost );
      if ( tmphost.upper() == QObexBtTransport::address2String( tmp ) ) {
	mParamMap.insert( "transport", "bluetooth" );
	mParamMap.insert( "host", tmphost );
	if ( 0 < port )
	  mParamMap.insert( "rfCommChannel", QString::number( port ) );
	matched = true;
      }
    }
    if ( host.contains( '-' ) == 5 ) {
      // We got a bluetooth hardware address.
      QString tmphost = host;
      bdaddr_t tmp = QObexBtTransport::string2Address( tmphost.replace( '-', ':' ) );
      if ( tmphost.upper() == QObexBtTransport::address2String( tmp ) ) {
	mParamMap.insert( "transport", "bluetooth" );
	mParamMap.insert( "host", tmphost );
	if ( 0 < port )
	  mParamMap.insert( "rfCommChannel", QString::number( port ) );
	matched = true;
      }
    }
#endif
    
    // Default is a ip hostname, then the transport is tcp.
    if ( !matched && 0 < host.length() ) {
      mParamMap.insert( "transport", "tcp" );
      mParamMap.insert( "host", host );
    }
  }
}

void ObexProtocol::slotResponse( const QObexObject& resp ) {
  myDebug << "ObexProtocol::slotResponse(" << resp.stringCode() << ")" << endl;
  switch ( mCommandType ) {
  case Get:
    // A length header in a get response signals the total size
    // of the transfered file.
    if ( resp.hasHeader( QObexHeader::Length ) ) {
      QObexHeader hdr = resp.getHeader( QObexHeader::Length );
      KIO::filesize_t sz = hdr.uint32Data();
      totalSize( sz );
      mHaveTotalsize = true;
      myDebug << "ObexProtocol::slotResponse(...): got total size "
	      << hdr.uint32Data() << endl;
    }
    // A type header in a get response signals the mime type
    // of the transfered file.
    if ( resp.hasHeader( QObexHeader::Type ) ) {
      QObexHeader hdr = resp.getHeader( QObexHeader::Type );
      mimeType( hdr.stringData() );
      mHaveMimeType = true;
      myDebug << "ObexProtocol::slotResponse(...): got mime type "
	      << hdr.stringData() << endl;
    }
    break;

  default:
    break;
  }

  // Collect apparam headers and give it to the application ...
  if ( resp.hasHeader( QObexHeader::AppParameters ) ) {
    QObexHeader hdr = resp.getHeader( QObexHeader::AppParameters );
    QObexApparam ap = hdr.arrayData();
    if ( ap.hasParam( 1 ) ) {
      QByteArray tmp = ap.getParam( 1 );
      setMetaData( "APPARAM-1", QString::fromAscii( tmp.data(), tmp.size() ) );
    }
  }
}


void ObexProtocol::slotAuthenticationRequired( const QString& realm, bool requireUserid ) {
  myDebug << "ObexProtocol::AuthenticationRequired(" << realm
	  << ", " << requireUserid << ")" << endl;
  mAuthInfo.url.setProtocol( "obex" );
  mAuthInfo.url.setHost( mCurrentHost );
  if ( mAuthInfo.username.isEmpty() )
    mAuthInfo.username = mCurrentUser;
  mAuthInfo.realmValue = realm;
  if ( !mCachedAuthIsRead ) {
    if ( !checkCachedAuthentication( mAuthInfo ) ) {
      if ( !openPassDlg( mAuthInfo ) ) {
	infoMessage( i18n( "Authentication failed" ) );
	sendError( KIO::ERR_COULD_NOT_AUTHENTICATE );
	return;
      }
    }
    mCachedAuthIsRead = true;
  } else {
    if ( !openPassDlg( mAuthInfo ) ) {
      infoMessage( i18n( "Authentication failed" ) );
      sendError( KIO::ERR_COULD_NOT_AUTHENTICATE );
      return;
    }
  }
  // when connection we will fail and retry a client side triggered
  // authenticatoin sequence.
  mClient->setServerSecret( mAuthInfo.password );
  mClient->setServerUserId( mAuthInfo.username );
}

void ObexProtocol::slotData( const QValueList<QByteArray>& d, bool* cont ) {
  myDebug << "ObexProtocol::slotData(" << d.size() << ")" << endl;
  if ( wasKilled() ) {
    *cont = false;
  } else {
    *cont = true;
    switch ( mCommandType ) {
    case Get:
      {
	QValueList<QByteArray>::ConstIterator it;
	for ( it = d.begin(); it != d.end(); ++it )
	  if ( 0 < (*it).size() ) {
	    if ( !mHaveMimeType ) {
	      KMimeType::Ptr mt = KMimeType::findByContent( *it );
	      mimeType( mt->name() );
	      mHaveMimeType = true;
	      myDebug << "ObexProtocol::slotData(...): guessed mime type "
		      << mt->name() << endl;
	    }
	    data( *it );
	    mProcessedSize += (*it).size();
	    if ( mHaveTotalsize )
	      processedSize( mProcessedSize );
	  }
	break;
      }
    case Stat:
    case GetFolderListing:
      { // We got data. Store it in the folder listing buffer.
	uint oldSize = mDataBuffer.size();
	QValueList<QByteArray>::ConstIterator it;
	for ( it = d.begin(); it != d.end(); ++it ) {
	  mDataBuffer.resize( oldSize+(*it).size() );
	  ::memcpy( mDataBuffer.data()+oldSize, (*it).data(), (*it).size() );
	  oldSize += (*it).size();
	}
	break;
      }
    default:
      break;
    }
  }
}

void ObexProtocol::slotDataReq( QByteArray& data, size_t maxSize, bool* cont ) {
  myDebug << "ObexProtocol::slotDataReq( data, "
	  << maxSize << ", cont )" << endl;
  if ( wasKilled() ) {
    *cont = false;
  } else {
    *cont = true;
    switch ( mCommandType ) {
    case Put:
      {
	QByteArray buf;
	int result = 0;
	while ( mDataBuffer.size() < maxSize
		&& 0 < (dataReq(), result = readData( buf ) ) ) {
	  uint oldSize = mDataBuffer.size();
	  mDataBuffer.resize( oldSize+result );
	  ::memcpy( mDataBuffer.data()+oldSize, buf.data(), result );
	}
	if ( result < 0 ) {
	  *cont = false;
	  return;
	}
	if ( mDataBuffer.size() < maxSize )
	  maxSize = mDataBuffer.size();
	data.resize( maxSize );
	if ( 0 < maxSize ) {
	  ::memcpy( data.data(), mDataBuffer.data(), maxSize );
	  qmemmove( mDataBuffer.data(), mDataBuffer.data()+maxSize,
		    mDataBuffer.size()-maxSize );
	  mDataBuffer.resize( mDataBuffer.size()-maxSize );
	}

	mProcessedSize += data.size();
	processedSize( mProcessedSize );
      }
      break;
    default:
      break;
    }
  }
}

void ObexProtocol::slotAborted( const QObexObject& ) {
  myDebug << "ObexProtocol::slotAborted( ... )" << endl;
  // FIXME what to do???
  sendError( KIO::ERR_ABORTED );
//   error( KIO::ERR_USER_CANCELED, err );
}

void ObexProtocol::slotError( QObexClient::Error err ) {
  myDebug << "ObexProtocol::slotError(" << int(err) << ")" << endl;

  // Ignore errors from connection attempts
  if ( mCommandType == Connect )
    return;

  switch ( err ) {
  case QObexClient::GotErrorResponse:
    break;
  case QObexClient::InvalidMTURecieved:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "OBEX protocol error: Recieved invalid MTU." ) );
    break;
  case QObexClient::InvalidPacketRecieved:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "OBEX protocol error: Recieved invalid packet." ) );
    break;
  case QObexClient::InvalidObexVersionRecieved:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "OBEX protocol error: Invalid protocol version received." ) );
    break;
  case QObexClient::AuthenticationRequired:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Authentication required." ) );
    break;
  case QObexClient::CanNotConnect:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Can not connect." ) );
    break;
  case QObexClient::AlreadyConnected:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Internal error: Already connected." ) );
    break;
  case QObexClient::NotConnected:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Internal error: Not connedcted." ) );
    break;
  case QObexClient::NotIdle:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Internal error: Client not idle." ) );
    break;
  case QObexClient::CanNotSend:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Unable to send request." ) );
    break;
  case QObexClient::ServerError:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Got server error from server." ) );
    break;
  case QObexClient::TransportNotConnected:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Transport is not connected (anymore)." ) );
    break;
  case QObexClient::TransportError:
    error( KIO::ERR_CONNECTION_BROKEN, i18n( "Transport error." ) );
    break;
  case QObexClient::InternalError:
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Internal error." ) );
    break;
  default:
    sendError( KIO::ERR_SLAVE_DEFINED );
    break;
  }
}

void ObexProtocol::openConnection() {
  myDebug << "ObexProtocol::openConnection()" << endl;

  if ( connectClientIfRequired() ) {
    connected();
    mConnectionOriented = true;
  }
}

void ObexProtocol::closeConnection() {
  myDebug << "ObexProtocol::closeConnection()" << endl;
  if ( mClient && mClient->isConnected() ) {
    infoMessage( i18n( "Disconnecting" ) );
    mCommandType = Disconnect;
    mClient->disconnectClient();
    mCommandType = Idle;
    infoMessage( i18n( "Disconnected" ) );
  }

  mConnectionOriented = false;
  mCwd = QString::null;
}

bool ObexProtocol::connectClientIfRequired() {
  myDebug << "ObexProtocol::connectClientIfRequired()" << endl;

  if ( !mClient ) {
    QObexTransport *transport = 0;
    if ( getParam( "transport" ) == "serialsiemens" ) {
      myDebug << "ObexProtocol::setHost(): we have a serialsiemens transport" << endl;
      QObexBfbTransport *bfbTransport = new QObexBfbTransport();
      if ( hasParam( "device" ) )
	bfbTransport->setDevice( getParam( "device" ) );
      if ( hasParam( "speed" ) )
	bfbTransport->setSpeed( getParam( "speed" ).toInt() );
      transport = bfbTransport;
      
    } else if ( getParam( "transport" ) == "serialericsson" ) {
      myDebug << "ObexProtocol::setHost(): we have a serialericsson transport" << endl;
      
      QObexEricssonTransport *ericssonTransport = new QObexEricssonTransport();
      if ( hasParam( "device" ) )
	ericssonTransport->setDevice( getParam( "device" ) );
      if ( hasParam( "speed" ) )
	ericssonTransport->setSpeed( getParam( "speed" ).toInt() );
      transport = ericssonTransport;
      
    } else if ( getParam( "transport" ) == "serial" ) {
      myDebug << "ObexProtocol::setHost(): we have a serial transport" << endl;
      
      QObexSerialTransport *serialTransport = new QObexSerialTransport();
      if ( hasParam( "device" ) )
	serialTransport->setDevice( getParam( "device" ) );
      if ( hasParam( "speed" ) )
	serialTransport->setSpeed( getParam( "speed" ).toInt() );
      transport = serialTransport;
      
    } else if ( getParam( "transport" ) == "tcp" ) {
      myDebug << "ObexProtocol::setHost(): we have a tcp transport" << endl;
      // We got an ip host. Use ip transport for that.
      if ( !hasParam( "host" ) || getParam( "host" ).isEmpty() ) {
	sendError( KIO::ERR_MALFORMED_URL );
	return false;
      }
      
      QObexInTransport *inTransport = new QObexInTransport();
      inTransport->setHost( getParam( "host" ) );
      if ( 0 < mCurrentPort )
	inTransport->setPort( mCurrentPort );
      transport = inTransport;
      
    } else if ( getParam( "transport" ) == "bluetooth" ) {
      myDebug << "ObexProtocol::setHost(): we have a bluetooth transport" << endl;
#ifdef HAVE_QOBEX_BLUETOOTH
      QObexBtTransport *btTransport = new QObexBtTransport();
      if ( hasParam( "uuid" ) ) {
	if ( getParam( "uuid" ) == "inbox" )
	  btTransport->setUuid( QObexBtTransport::ObexPushServ );
	else if ( getParam( "uuid" ) == "irmcsync" )
	  btTransport->setUuid( QObexBtTransport::IrMCServ );
	else
	  // Default is folder browsing service
	  btTransport->setUuid( QObexBtTransport::ObexFileServ );
      } else
	btTransport->setUuid( QObexBtTransport::ObexFileServ );
      btTransport->setDestAddress( getParam( "host" ) );
      if ( hasParam( "rfCommChannel" ) )
	btTransport->setRfCommChannel( getParam( "rfCommChannel" ).toInt() );
      // FIXME additional connection parameters???
      
      // update kbluetoothd's list of most recently accessed services
      saveMru(KBluetooth::DeviceAddress(getParam("host")), getParam("rfCommChannel").toInt());
            
      transport = btTransport;
#else
      error( KIO::ERR_SLAVE_DEFINED, i18n( "Transport not available" ) );
      return false;
#endif
    } else if ( getParam( "transport" ) == "irda" ) {
      myDebug << "ObexProtocol::setHost(): we have a irda transport" << endl;
#ifdef HAVE_QOBEX_IRDA
      QObexIrDATransport *irDATransport = new QObexIrDATransport();
      irDATransport->setDestAddress( getParam( "host" ) );
      
      transport = irDATransport;
#else
      error( KIO::ERR_SLAVE_DEFINED, i18n( "Transport not available" ) );
      return false;
#endif    
    } else {
      sendError( KIO::ERR_MALFORMED_URL );
      return false;
    }
    
    Q_ASSERT( transport != 0 );
    
    // Set the overreidePermission mask
    mOverridePerm = 0;
    if ( hasParam( "overridePerms" ) )
      mOverridePerm = getParam( "overridePerms" ).toLong();
    
    // If we have this parameter override
    mDisconnectTimeout = DefaultDisconnectTimeout;
    if ( hasParam( "disconnectTimeout" ) )
      mDisconnectTimeout = getParam( "disconnectTimeout" ).toInt();
    
    // Make sure it is blocking FIXME
    transport->setBlocking( true );
    // Get a new client with the given transport
    mClient = new QObexClient( transport );
    mClient->setOpenObexSupport( true );
    
    // Set persistent authentication information
    if ( mCurrentPass.isEmpty() ) {
      if ( hasParam( "serverSecret" ) ) {
	mClient->setServerSecret( getParam( "serverSecret" ) );
	mClient->setInitiateAuthentication( true );
      }
    } else {
      mClient->setServerSecret( mCurrentPass );
      mClient->setInitiateAuthentication( true );
    }
    
    if ( mCurrentUser.isEmpty() ) {
      if ( hasParam( "serverUserId" ) )
	mClient->setServerUserId( getParam( "serverUserId" ) );
    } else
      mClient->setServerUserId( mCurrentUser );
    
    // default is folder browsing service
    if ( hasParam( "uuid" ) ) {
      if ( getParam( "uuid" ) == "inbox" )
	;
      else if ( getParam( "uuid" ) == "irmcsync" )
	mClient->setUuid( QObexUuidIrMCSync );
      else
	// Default is folder browsing service
	mClient->setUuid( QObexUuidFBS );
    } else
      mClient->setUuid( QObexUuidFBS );
    
    QObject::connect( mClient, SIGNAL(signalResponse(const QObexObject&)),
		      SLOT(slotResponse(const QObexObject&)) );
    QObject::connect( mClient, SIGNAL(signalAuthenticationRequired(const QString&,bool)),
		      SLOT(slotAuthenticationRequired(const QString&,bool)) );
    QObject::connect( mClient, SIGNAL(signalData(const QValueList<QByteArray>&,bool*)),
		      SLOT(slotData(const QValueList<QByteArray>&,bool*)) );
    QObject::connect( mClient, SIGNAL(signalDataReq(QByteArray&,size_t,bool*)),
		      SLOT(slotDataReq(QByteArray&,size_t,bool*)) );
    QObject::connect( mClient, SIGNAL(signalAborted(const QObexObject&)),
		      SLOT(slotAborted(const QObexObject&)) );
    QObject::connect( mClient, SIGNAL(signalError(QObexClient::Error)),
		      SLOT(slotError(QObexClient::Error)) );
  }
  
  if ( mClient->isConnected() )
    return true;

  infoMessage( i18n( "Connecting" ) );
  mCwd = QString::null;
  mCommandType = Connect;

  // The maximum time
  QDateTime deadline = QDateTime::currentDateTime().addSecs( connectTimeout() );
  mCachedAuthIsRead = false;
  do {
    myDebug << "ObexProtocol::connectClientIfRequired(): Try to connect" << endl;
    mClient->connectClient();
    if ( mClient->isConnected()
	 && ( mClient->isDirected() || mClient->uuid().isEmpty() )
	 && mClient->responseCode() == QObexObject::Success )
      break;
    myDebug << "ObexProtocol::connectClientIfRequired(): Hmm, something went wrong with connecting ..." << endl;

    // if unauthorized try again without delay
    if ( mClient->responseCode() == QObexObject::Unauthorized ) {
      mClient->setInitiateAuthentication( true );
      mClient->setServerSecret( mAuthInfo.password );
      mClient->setServerUserId( mAuthInfo.username );
    } else
      ::sleep( 1 );

    if ( wasKilled() )
      break;

  }  while ( QDateTime::currentDateTime() < deadline );

  mCommandType = Idle;
  
  if ( mClient->responseCode() == QObexObject::Success ) {
    if ( mClient->isDirected() || mClient->uuid().isEmpty() ) {
      infoMessage( i18n( "Connected" ) );
      return true;
    } else {
      mCommandType = Disconnect;
      mClient->disconnectClient();
      mCommandType = Idle;
      sendError( KIO::ERR_COULD_NOT_CONNECT );
      infoMessage( i18n( "Error" ) );
      return false;
    }
  } else if ( mClient->responseCode() == QObexObject::Unauthorized ) {
    infoMessage( i18n( "Authentication failed" ) );
    sendError( KIO::ERR_COULD_NOT_AUTHENTICATE );
    return false;
  } else {
    infoMessage( i18n( "Error" ) );
    sendError( KIO::ERR_COULD_NOT_CONNECT );
    return false;
  }
}

void ObexProtocol::startDisconnectTimer() {
  myDebug << "ObexProtocol::startDisconnectTimer()" << endl;
  if ( !mConnectionOriented ) {
    // disconnect with a one second timeout. This way we can handle
    // multiple requests from konquerror with one connection.
    // The user must be really fast when he is able to unplug the device
    // faster then one second ...
    QByteArray cmd( 1 );
    cmd[0] = 'd';
    setTimeoutSpecialCommand( mDisconnectTimeout, cmd );
  }
}

// It understands 4 commands.
// 'd' means disconnect.
// 'sb' means search bluetooth devices
// 'si' means search irda devices
// 'sa' means search all devices, bluetooth and irda
void ObexProtocol::special( const QByteArray& cmd ) {
  myDebug << "ObexProtocol::special( ... )" << endl;
  if ( !mClient ) {
    error( KIO::ERR_SLAVE_DEFINED, i18n( "Internal error: Not connedcted." ) );
    return;
  }

  if ( cmd.size() == 1 && cmd[0] == 'd' ) {
    // we got a disconnect command
    infoMessage( i18n( "Disconnecting" ) );
    mClient->disconnectClient();
    mUDSEntryCache.clear();
    infoMessage( i18n( "Disconnected" ) );
  } if ( cmd.size() == 2 && cmd[0] == 's' ) {
    int ndevs = 0;
#ifdef HAVE_QOBEX_BLUETOOTH
    // Serach Bluetooth devices
    if ( cmd[1] == 'b' || cmd[1] == 'a' ) {
      QValueList<bdaddr_t> devs = QObexBtTransport::inquireDevices();

      QValueList<bdaddr_t>::Iterator it;
      for ( it = devs.begin(); it != devs.end(); ++it ) {
	// Read the remote device name.
	QString devName = QObexBtTransport::getRemoteName( *it );
	QString addr = QObexBtTransport::address2String( *it );
	if ( 0 < QObexBtTransport::getRfCommChannel( *it, QObexBtTransport::ObexPushServ ) ) {
	  QString devKey = QString( "OBEX-DEVICE-%1" ).arg( ndevs++ );
	  setMetaData( devKey + "-TRANSPORT", "bluetooth" );
	  setMetaData( devKey + "-NAME", devName );
	  setMetaData( devKey + "-ADDRESS", addr );
	}
      }
    }
#endif
#ifdef HAVE_QOBEX_IRDA
    // Serach IrDA devices
    if ( cmd[1] == 'i' || cmd[1] == 'a' ) {
      QMap<Q_UINT32,QString> devs = QObexIrDATransport::discoverDevices();

      QMap<Q_UINT32,QString>::Iterator it;
      for( it = devs.begin(); it != devs.end(); ++it ) {
	// display discovered device.
	QString addr = QObexIrDATransport::address2String( it.key() );
	QString devKey = QString( "OBEX-DEVICE-%1" ).arg( ndevs++ );
	setMetaData( devKey + "-TRANSPORT", "irda" );
	setMetaData( devKey + "-NAME", it.data() );
	setMetaData( devKey + "-ADDRESS", addr );
      }
    }
#endif
    setMetaData( "OBEX-DISCOVERED-DEVICES", QString::number( ndevs ) );
  }
  finished();
}

void ObexProtocol::sendError( int defaultError ) {
  myDebug << "ObexProtocol::sendError(" << defaultError << ")" << endl;
  QString host = getParam( "host" );
  if ( mClient ) {
    switch ( mClient->responseCode() ) {
    case QObexObject::Unauthorized:
      error( KIO::ERR_COULD_NOT_AUTHENTICATE, host );
      return;
    case QObexObject::Forbidden:
      error( KIO::ERR_ACCESS_DENIED, host );
      return;
      
    default:
      break;
    }
  }

  // The default error report ...
  error( defaultError, host );
}

void ObexProtocol::get( const KURL& url )
{
  myDebug << "ObexProtocol::get(" << url.prettyURL() << ")" << endl;

  // Wired, but we have to check if this is a directory ...
  UDSEntry udsEnt = getCachedStat( url );
  if ( udsEnt.isEmpty() ) {
    error( KIO::ERR_DOES_NOT_EXIST, url.path() );
    return;
  } else {
    UDSEntry::Iterator it;
    for ( it = udsEnt.begin(); it != udsEnt.end(); ++it ) {
      if ( (*it).m_uds == KIO::UDS_FILE_TYPE ) {
	if ( (*it).m_long == S_IFDIR ) {
	  error( KIO::ERR_IS_DIRECTORY, url.path() );
	  return;
	}
      }
    }
  }

  if ( !connectClientIfRequired() )
    return;

  if ( !changeWorkingDirectory( url.directory( false ) ) )
    return;

  infoMessage( i18n( "Downloading" ) );

  // reset state values
  mCommandType = Get;
  mHaveTotalsize = false;
  mProcessedSize = 0;
  mHaveMimeType = false;

  mClient->get( url.filename( false ) );
  bool done = mClient->responseCode() == QObexObject::Success;
  mCommandType = Idle;
  myDebug << "ObexProtocol::get(): done = " << done << endl;
  if ( done ) {
    infoMessage( i18n( "Downloaded" ) );

    // Signal end of data
    data( QByteArray() );
    // send mimeType if not already done.
    if ( !mHaveMimeType )
      mimeType( KMimeType::defaultMimeType() );

    processedSize( mProcessedSize );
    finished();
  } else {
    infoMessage( i18n( "Error" ) );
    sendError( KIO::ERR_CANNOT_OPEN_FOR_READING );
  }

  mHaveMimeType = false;
  startDisconnectTimer();
}

void ObexProtocol::mkdir(const KURL& url, int permissions)
{
  // One can mkdir by simply going into this directory.
  myDebug << "ObexProtocol::mkdir(" << url.prettyURL() << ", "
	  << permissions << ")" << endl;

  if ( !connectClientIfRequired() )
    return;

  if ( !changeWorkingDirectory( url.directory( false ) ) )
    return;
  
  infoMessage( i18n( "Creating directory" ) );
  mCommandType = MkDir;
  mClient->setPath( url.fileName(), false, true );
  bool done = mClient->responseCode() == QObexObject::Success;
  mCommandType = Idle;
  if ( done ) {
    mCwd = mCwd.isEmpty() ? url.fileName() : ( mCwd + "/" + url.fileName() );
    infoMessage( i18n( "Created directory" ) );
    finished();
  } else {
    infoMessage( i18n( "Error" ) );
    sendError( KIO::ERR_COULD_NOT_MKDIR );
  }
  startDisconnectTimer();
}

void ObexProtocol::del(const KURL& url, bool isfile) {
  myDebug << "ObexProtocol::del(" << url.prettyURL()
	  << ", " << isfile << ")" << endl;

  if ( !connectClientIfRequired() )
    return;

  if ( !changeWorkingDirectory( url.directory( false ) ) )
    return;

  infoMessage( i18n( "Deleting" ) );
  mCommandType = Delete;
  mClient->del( url.fileName() );
  bool done = mClient->responseCode() == QObexObject::Success;
  mCommandType = Idle;
  if ( done ) {
    infoMessage( i18n( "Deleted" ) );
    finished();
  } else {
    infoMessage( i18n( "Error" ) );
    sendError( KIO::ERR_CANNOT_DELETE );
  }
  startDisconnectTimer();
}

void ObexProtocol::stat(const KURL &url) {
  myDebug << "ObexProtocol::stat(" << url.prettyURL() << ")" << endl;

  UDSEntry udsEnt = getCachedStat( url );
  if ( udsEnt.isEmpty() )
    sendError( KIO::ERR_DOES_NOT_EXIST );
  else {
    statEntry( udsEnt );
    finished();
  }
}

void ObexProtocol::listDir(const KURL& url) {
  myDebug << "ObexProtocol::listDir(" << url.prettyURL() << ")" << endl;

  if ( !connectClientIfRequired() )
    return;

  if ( !changeWorkingDirectory( url.path() ) )
    return;

  myDebug << "ObexProtocol::listDir() at line " << __LINE__ << endl;
  mCommandType = GetFolderListing;
  mHaveTotalsize = false;
  mProcessedSize = 0;
  mDataBuffer.resize( 0 );

  infoMessage( i18n( "Downloading" ) );
  mClient->get( QString::null, "x-obex/folder-listing" );
  infoMessage( i18n( "Downloaded" ) );
  myDebug << "ObexProtocol::listDir() at line " << __LINE__ << endl;
  bool done = false;
  if ( mClient->responseCode() == QObexObject::Success ) {
    // Parse the xml document returned from the phone.
    QDomDocument doc("current data");
    doc.setContent( mDataBuffer );
    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    
    UDSEntryList entries;
    while ( !node.isNull() ) {
      QDomElement element = node.toElement();
      if ( !element.isNull() && element.hasAttribute( "name" ) ) {
	UDSEntry ue = createUDSEntry( element );
	KURL tmpURL = url;
	tmpURL.addPath( element.attribute( "name" ) );
	myDebug << "ObexProtocol::listDir() storing UDSEntry with key " << tmpURL.path() << endl;
	mUDSEntryCache[ tmpURL.path() ] = ue;
	entries.append( ue );
      }
      node = node.nextSibling();
    }
    listEntries( entries );

    done = true;
  }
  mDataBuffer.resize( 0 );
	
  mCommandType = Idle;

  if ( done )
    finished();
  else
    sendError( KIO::ERR_CANNOT_OPEN_FOR_READING );
  startDisconnectTimer();
}

void
ObexProtocol::put(const KURL& url, int _mode, bool _overwrite, bool _resume) {
  myDebug << "ObexProtocol::put(" << url.prettyURL() << ", "
	  << _mode << ", " << _overwrite << ", " << _resume << ")" << endl;

  if ( !connectClientIfRequired() )
    return;

  if ( !changeWorkingDirectory( url.directory( false ) ) )
    return;

  infoMessage( i18n( "Uploading" ) );
  mCommandType = Put;
  mHaveTotalsize = false;
  mProcessedSize = 0;
  mDataBuffer.resize( 0 );
  mClient->put( url.fileName(), 0 );
  mDataBuffer.resize( 0 );
  bool done = mClient->responseCode() == QObexObject::Success;
  mCommandType = Idle;
  infoMessage( i18n( "Uploaded" ) );
  if ( done )
    finished();
  else
    sendError( KIO::ERR_CANNOT_OPEN_FOR_WRITING );
  startDisconnectTimer();
}

UDSEntry ObexProtocol::getCachedStat( const KURL &url ) {
  myDebug << "ObexProtocol::getCachedStat(" << url.prettyURL() << ")" << endl;

  // Create fake entry for / FIXME is it possible to get some info??
  if ( url.path() == "/" ) {
    UDSEntry entry;
    UDSAtom atom;

    // set the file type
    atom.m_uds = KIO::UDS_FILE_TYPE;
    atom.m_long = S_IFDIR;
    entry.append( atom );

    return entry;
  }

  // Try a fast exit if we already have the data.
  if ( mUDSEntryCache.contains( url.path() ) ) {
    myDebug << "ObexProtocol::getCachedStat() found UDSEntry with key "
	    << url.path() << endl;
    QDateTime now = QDateTime::currentDateTime();
    if ( mUDSEntryCache[ url.path() ].time.addSecs( UDSEntryCacheTimeout ) < now ) {
      // Cache entry is too old.
      // Remove it from the cache and proceed with the older listing ...
      mUDSEntryCache.erase( url.path() );
      myDebug << "ObexProtocol::getCachedStat() UDSEntry too old removing"
	      << endl;
    } else {
      // Cache entry is ok. Use it ...
      myDebug << "ObexProtocol::getCachedStat() UDSEntry ok, using cached one"
	      << endl;
      return mUDSEntryCache[ url.path() ].data;
    }
  }

  // Ok, we have to get the folder listing and search for the given file.
  if ( !connectClientIfRequired() )
    return UDSEntry();

  if ( !changeWorkingDirectory( url.directory( true, true ) ) )
    return UDSEntry();

  UDSEntry ret;
  //   infoMessage( i18n( "Downloading" ) );
  mCommandType = Stat;
  mDataBuffer.resize( 0 );
  myDebug << "ObexProtocol::getCachedStat() doing directory listing for "
	  << url.directory( true, true ) << endl;
  mClient->get( QString::null, "x-obex/folder-listing" );
  bool done = false;
//   infoMessage( i18n( "Downloaded" ) );
  if ( mClient->responseCode() == QObexObject::Success ) {
    // Parse the xml document returned from the phone.
    QDomDocument doc("current data");
    doc.setContent( mDataBuffer );
    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    
    QString file = url.fileName( true );
    while ( !node.isNull() ) {
      QDomElement element = node.toElement();
      if ( !element.isNull() ) {
	if ( element.hasAttribute("name") ) {
	  // Store the result in the UDSEntryCache.
	  UDSEntry ue = createUDSEntry( element );
	  QString key = url.directory( false, true )
	    + element.attribute( "name" );
	  myDebug << "ObexProtocol::getCachedStat(): " 
		  << "storing UDSEntry with key " << key << endl;
	  mUDSEntryCache[ key ] = ue;

	  // If we have found what we are searching for return the UDSEntry.
	  if ( element.attribute("name") == file ) {
	    ret = ue;
	    done = true;
	  }
	}
      }
      node = node.nextSibling();
    }
  }
  mDataBuffer.resize( 0 );
  mCommandType = Idle;
  startDisconnectTimer();

  return ret;
}

bool ObexProtocol::changeWorkingDirectory( const QString& to ) {
  myDebug << "ObexProtocol::changeWorkingDirectory( " << to << " ), mCwd = " << mCwd << endl;
  if ( !to.startsWith( "/" ) ) {
    sendError( KIO::ERR_MALFORMED_URL );
    return false;
  }
  if ( mCwd == to )
    return true;
  if ( !mClient ) {
    sendError( KIO::ERR_INTERNAL );
    return false;
  }
  if ( !mClient->isConnected() ) {
    sendError( KIO::ERR_CONNECTION_BROKEN );
    return false;
  }

  // split the path into components
  QStringList oldl = QStringList::split( "/", mCwd );
  QStringList tol = QStringList::split( "/", to );
  QStringList newl;
  
  // compute the common first part of the path.
  QStringList::Iterator oit, tit;
  for ( oit = oldl.begin(), tit = tol.begin();
	oit != oldl.end() && tit != tol.end(); ) {
    if ( (*oit) != (*tit) )
      break;
    newl.append( *oit );
    oit = oldl.remove( oit );
    tit = tol.remove( tit );
  }
  
  // Decide which one is faster.
  // cd .. oldl.size() times followed by cd <newdir> tol.size() times
  // or cd / followed by cd <newdir> into the whole new path.
  // that gives for the first cut
  //   if ( oldl.size() + tol.size() < 1 + newl.size() + tol.size() )
  // and simplyfied
  if ( oldl.size() <= newl.size() ) {
    // step back
    for ( oit = oldl.fromLast(); oit != oldl.end(); ) {
      myDebug << "ObexProtocol::changeWorkingDirectory(): leaving " << *oit << endl;
      // cd ..
      mClient->setPath( QString::null, true );
      if ( !mClient->isSuccessfull() ) {
	newl += oldl;
	mCwd = newl.join( "/" );
	if ( mCwd == "/" ) mCwd = QString::null;
	sendError( KIO::ERR_CANNOT_ENTER_DIRECTORY );
	return false;
      }
      oldl.remove( oit );
      oit = oldl.fromLast();
    }
  } else {
    myDebug << "ObexProtocol::changeWorkingDirectory(): entering /" << endl;
    // cd /
    mClient->setPath();
    if ( !mClient->isSuccessfull() ) {
      sendError( KIO::ERR_CANNOT_ENTER_DIRECTORY );
      return false;
    }
    newl.clear();
    tol = QStringList::split( "/", to );
  }
    
  // step forward into to
  for ( tit = tol.begin(); tit != tol.end(); ++tit ) {
    myDebug << "ObexProtocol::changeWorkingDirectory(): entering " << *tit << endl;
    // cd *tit
    mClient->setPath( *tit );
    if ( !mClient->isSuccessfull() ) {
      mCwd = newl.join( "/" );
      if ( mCwd == "/" ) mCwd = QString::null;
      sendError( KIO::ERR_CANNOT_ENTER_DIRECTORY );
      return false;
    }
    newl.append( *tit );
  }
  
  mCwd = newl.join( "/" );
  if ( mCwd == "/" ) mCwd = QString::null;
  myDebug << "ObexProtocol::changeWorkingDirectory(): success, mCwd = " << mCwd << endl;
  return true;
}

UDSEntry ObexProtocol::createUDSEntry( const QDomElement &element ) {
  myDebug << "ObexProtocol::createUDSEntry( ... )" << endl;

  UDSEntry entry;
  UDSAtom atom;

  if ( element.hasAttribute( "name" ) ) {
    myDebug << "ObexProtocol::createUDSEntry() for name  = " << element.attribute( "name" ) << endl;
    atom.m_uds = KIO::UDS_NAME;
    atom.m_str = element.attribute( "name" );
    entry.append( atom );
  }

  if ( element.hasAttribute( "size" ) ) {
    atom.m_uds = KIO::UDS_SIZE;
    atom.m_long = element.attribute("size").toLong( );
    entry.append(atom);
  }

  if ( element.hasAttribute( "modified" ) ) {
    atom.m_uds = KIO::UDS_MODIFICATION_TIME;
    atom.m_long = stringToTime_t( element.attribute("modified"));
    entry.append( atom);
  }

  if (element.hasAttribute( "created")) {
    atom.m_uds = KIO::UDS_CREATION_TIME;
    atom.m_long = stringToTime_t( element.attribute("created"));
    entry.append( atom);
  }

  if (element.hasAttribute( "accessed")) {
    atom.m_uds = KIO::UDS_ACCESS_TIME;
    atom.m_long = stringToTime_t( element.attribute("accessed"));
    entry.append( atom);
  }

  if (element.hasAttribute( "type")) {
    atom.m_uds = KIO::UDS_MIME_TYPE;
    atom.m_str = element.attribute("type");
    entry.append( atom );
  }

  if (element.hasAttribute( "xml:lang")) {
    // xml:lang ( == encoding bei owner group ...)
    // FIXME
    ;
  }

  if ( element.hasAttribute( "owner" ) ) {
    atom.m_uds = KIO::UDS_USER;
    atom.m_str = element.attribute( "owner" );
    entry.append( atom );
  } else {
    // FIXME??
    atom.m_uds = KIO::UDS_USER;
    atom.m_str = userName;
    entry.append( atom );
  }

  if ( element.hasAttribute( "group" ) ) {
    atom.m_uds = KIO::UDS_GROUP;
    atom.m_str = element.attribute( "group" );
    entry.append( atom );
  } else {
    // FIXME??
    atom.m_uds = KIO::UDS_GROUP;
    atom.m_str = groupName;
    entry.append( atom );
  }

  // If overridePerm contains some mode values,
  // ignore those in the directory listing
  long mode = mOverridePerm;
  if ( !mode ) {
    if ( element.hasAttribute( "user-perm" ) ) {
      QString val = element.attribute( "user-perm" );
      if ( val.contains( 'r', FALSE ) )
	mode |= S_IRUSR;
      if ( val.contains( 'd', FALSE ) && val.contains( 'w', FALSE ) )
	mode |= S_IWUSR;
    }
    if ( element.hasAttribute( "group-perm" ) ) {
      QString val = element.attribute( "group-perm" );
      if ( val.contains( 'r', FALSE ) )
	mode |= S_IRGRP;
      if ( val.contains( 'd', FALSE ) && val.contains( 'w', FALSE ) )
	mode |= S_IWGRP;
    }
    if ( element.hasAttribute( "other-perm" ) ) {
      QString val = element.attribute( "other-perm" );
      if ( val.contains( 'r', FALSE ) )
	mode |= S_IROTH;
      if ( val.contains( 'd', FALSE ) && val.contains( 'w', FALSE ) )
	mode |= S_IWOTH;
    }

    if ( !element.hasAttribute( "user-perm" ) &&
	 !element.hasAttribute( "group-perm" ) &&
	 !element.hasAttribute( "other-perm" ) ) {
      // Fallback solution: If we do not get any permissions,
      // just tell konquerror that the user will have read and write access.
      // This will definitly help many mobiles out there ...
      mode |= S_IRUSR | S_IWUSR;
    }
  }
  
  if ( element.tagName() == "file" ) {
    atom.m_uds = KIO::UDS_ACCESS;
    atom.m_long = mode;
    entry.append( atom );
    
    // set the file type
    atom.m_uds = KIO::UDS_FILE_TYPE;
    atom.m_long = S_IFREG;
    entry.append( atom );
  } else if ( element.tagName() == "folder" ) {
    if ( mode & S_IRUSR )
      mode |= S_IXUSR;
    if ( mode & S_IRGRP )
      mode |= S_IXGRP;
    if ( mode & S_IROTH )
      mode |= S_IXOTH;

    atom.m_uds = KIO::UDS_ACCESS;
    atom.m_long = mode;
    entry.append( atom );
    
    // set the file type
    atom.m_uds = KIO::UDS_FILE_TYPE;
    atom.m_long = S_IFDIR;
    entry.append( atom );
  }

  return entry;
}

time_t ObexProtocol::stringToTime_t( QString str ) {
//   myDebug << "ObexProtocol::stringToTime_t(" << str << ")" << endl;
  str.insert(13, ':');
  str.insert(11, ':');
  str.insert(6, '-');
  str.insert(4, '-');
  QDateTime time = QDateTime::fromString(str, Qt::ISODate);
  return time.toTime_t();
}

QString ObexProtocol::getParam( const QString& key ) {
//   myDebug << "ObexProtocol::getParam( " << key << " )" << endl;
  if ( mParamMap.contains( key ) )
    return mParamMap[ key ];
  else {
    if ( hasMetaData( key ) )
      return metaData( key );
    else
      return QString::null;
  }
}

bool ObexProtocol::hasParam( const QString& key ) {
//   myDebug << "ObexProtocol::hasParam( " << key << " )" << endl;
  return mParamMap.contains( key ) || hasMetaData( key );
}

void ObexProtocol::saveMru(KBluetooth::DeviceAddress addr, int channel)
{
    DCOPClient* dc = dcopClient();
    if (!dc) return;
    QByteArray param;
    QDataStream paramStream(param, IO_WriteOnly);
    QStringList command("konqueror");
    command << QString("obex://[%1]:%2/").arg(QString(addr)).arg(channel);
    paramStream << i18n("OBEX FTP") 
        << command
        << QString("nfs_unmount")
        << QString(addr);
    QByteArray retData;
    QCString retType;
    dc->call("kbluetoothd", "MRUServices", "mruAdd(QString,QStringList,QString,QString)",
        param, retType, retData, false);
}


#include "obex.moc"
