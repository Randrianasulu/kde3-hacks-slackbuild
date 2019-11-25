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

#ifndef QOBEXCLIENT_H
#define QOBEXCLIENT_H

#include <qobject.h>
#include <qstring.h>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qsocketnotifier.h>

#include "qobexobject.h"
#include "qobexauth.h"
#include "qobexbase.h"
#include "qobextransport.h"

class QObexClient
  : public QObexBase {
  Q_OBJECT
public:
  enum Error {
    InvalidMTURecieved,
    InvalidPacketRecieved,
    InvalidObexVersionRecieved,
    AuthenticationRequired,
    CanNotConnect,
    AlreadyConnected,
    NotConnected,
    NotIdle,
    CanNotSend,
    ServerError,
    TransportNotConnected,
    TransportError,
    GotErrorResponse,
    InternalError
  };

  enum Request {
    Connect,
    Disconnect,
    Get,
    Put,
    SetPath,
    Abort
  };

  /**
     Creates a new QObexClient using @ref transport.
     Note that the transport should be allocated with new since it is deleted
     on destruction of a client.
  */
  QObexClient( QObexTransport *, QObject* = 0, const char* = 0 );
  /**
     Destructor. Disconnects from server if required.
  */
  ~QObexClient();
  
  /**
     Start a connect request. If a uuid is set it is used to connect to
     that specific service.
  */
  bool connectClient();
  
  /**
     Start a disconnect request.
  */
  bool disconnectClient();
  
  /**
     Start a get request for the given @ref name and mimetype @ref type
  */
  bool get( const QString& name, const QString& type = QString::null );
  
  /**
     Start an empty put request. According to the specs this shuold
     delete the object given by @name.
  */
  bool del( const QString& );
  
  /**
     Start an empty put request. According to the specs this shuold
     delete the object given by @name.
  */
  bool del( const QString&, const QValueList<QObexHeader>& );
  
  /**
     Start a put request for name.
  */
  bool put( const QString&, Q_UINT32 = 0, const QString& = QString::null );
  
  /**
     Start a put request for name.
  */
  bool put( const QString&, const QValueList<QObexHeader>& );
  
  /**
     Start a set path request.
  */
  bool setPath( const QString&, bool = false, bool = false );
  
  /**
     Start a set path request with empty name. That means cd / ...
  */
  bool setPath( bool = false, bool = false );

  /**
     Start an abort request.
   */
  bool abort();

  /**
     Sets the target uuid of a client. When a connect packet is sent It tries
     to connect to the service with the given uuid. Note that this call
     does not change the uuid of an existing connection.
  */
  void setUuid( const QByteArray& uuid ) { mUUID = uuid; }
  void setTarget( const QByteArray& uuid ) { mUUID = uuid; }
  
  /**
     Get the target uuid of the client. This is the uuid which was set
     by setUuid and may not be the one we are connected to.
  */
  const QByteArray uuid() const { return mUUID; }
  const QByteArray target() const { return mUUID; }
  
  
  /**
     Return the connection id. One can't set it. We get it
     from the obex server.
  */
  Q_UINT32 connectionId() const { return mConnectionId; }
  
  /**
     Returns wether the client is connected or not.
  */
  bool isConnected() const { return mConnected; }
  
  /**
     Returns If we have a directed connection.
  */
  bool isDirected() const { return mConnected && mUseConnectionId; }
  
  /**
     Returns if the last command completed successfully.
  */
  bool isSuccessfull() const { return mResponseCode == QObexObject::Success; }
  
  /**
     Returns if the client is currently idle.
  */
  bool isIdle() const { return mCurrentRequest == 0; }

  /**
   */
  int responseCode() const { return mResponseCode; }
  
  
  /**
     Set the auth info used to authenticate the client on the server.
     If not already set you should set it in signalAuthenticationRequired()
     once it is called.
  */
  void setServerAuthInfo( const QObexAuthDigestResponse::AuthInfo& ai ) {
    mServerAuthInfo = ai;
  }

  /**
     Authentication of the client on the server.
     Returns the secret previously set by setServerSecret.
  */
  QObexAuthDigestResponse::AuthInfo serverAuthInfo() const {
    return mServerAuthInfo;
  }

  /**
     Set the auth info used to authenticate the client on the server.
     If not already set you should set it in signalAuthenticationRequired()
     once it is called.
     This data is part if serverAuthInfo(). The function is provided for
     convenience.
   */
  void setServerSecret( const QByteArray& secret ) {
    mServerAuthInfo.secret = secret;
  }
  void setServerSecret( const QString& secret ) {
    mServerAuthInfo.secret.duplicate( secret.ascii(), secret.length() );
  }
  /**
     Set the secret used to authenticate the client on the server.
     This data is part if serverAuthInfo(). The function is provided for
     convenience.
   */
  QByteArray serverSecret() const { return mServerAuthInfo.secret; }

  /**
     Set the auth info used to authenticate the client on the server.
     If not already set you should set it in signalAuthenticationRequired()
     once it is called.
     This data is part if serverAuthInfo(). The function is provided for
     convenience.
   */
  void setServerUserId( const QString& userId ) {
    mServerAuthInfo.userId = userId;
  }
  /**
     Set the userId used to authenticate the client on the server.
     This data is part if serverAuthInfo(). The function is provided for
     convenience.
   */
  QString serverUserId() const { return mServerAuthInfo.userId; }


  /**
     True if the server gives the client read only access with this
     authentication.
  */
  bool serverIsReadOnly() const { return mServerIsReadOnly; }
  
  /**
     Returns true if the server is successfuly authenticated.
  */
  bool serverIsAuthenticated() const { return mServerIsAuthenticated; }
  

  /**
     Set the authinfo used to authenticate the server.
  */
  void setClientAuthInfo( const QObexAuthDigestChallenge::AuthInfo& ai ) {
    mClientAuthInfo = ai;
  }
  /**
     Get the authinfo used to authenticate the server.
  */
  QObexAuthDigestChallenge::AuthInfo clientAuthInfo() const {
    return mClientAuthInfo;
  }

  /**
     Set the realm (usually username) used to authenticate the server.
     This data is part of the clientAuthInfo.
  */
  void setClientRealm( const QString& realm ) {
    mClientAuthInfo.realm = realm;
  }
  /**
     Get the realm (usually username) used to authenticate the server.
     This data is part of the clientAuthInfo.
  */
  QString clientRealm() const { return mClientAuthInfo.realm; }
  
  /**
     Set the options used to authenticate the server.
     This data is part of the clientAuthInfo.
  */
  void setClientOptions( Q_UINT8 opts ) { mClientAuthInfo.options = opts; }
  /**
     Get the options used to authenticate the server.
     This data is part of the clientAuthInfo.
  */
  Q_UINT8 clientOptions() const { return mClientAuthInfo.options; }

  /**
     Set the secret used to authenticate the server.
  */
  void setClientSecret( const QByteArray& secret ) { mClientSecret = secret; }
  QByteArray clientSecret() const { return mClientSecret; }
  
  /**
     If set the client will send an authentication
     challenge with the connect request.
  */
  void setInitiateAuthentication( bool enable ) { mInitiateAuth = enable; }

  /**
     Read the above configuration option.
  */
  bool initiateAuthentication() const { return mInitiateAuth; }
  
  /**
     OpenObex sends a wrong protocol version. If this is true the client
     accepts this wrong protocol version.
   */
  void setOpenObexSupport( bool enable ) { mOpenObexSupport = enable; }

  /**
     Returns the above configuration setting.
   */
  bool openObexSupport() const { return mOpenObexSupport; }

signals:
  /**
     Callback functions for an implementation of a client.
     Once those are called one can assume that the response is valid
     in the sense of the obex specification (FIXME, I hope so).
     Use those calls if you want to extract data like mime types,
     total length information or description headers from the response.
     Note that the data streaming is handled via data and dataReq.
  */
  void signalResponse( const QObexObject& );

  /**
     It is called when the client needs to authenticate itself with the server.
   */
  void signalAuthenticationRequired( const QString&, bool );

  /**
     Is called when data has arrived from the server.
     When true is returned streaming continues. If false is returned
     the operation is aborted.
  */
  void signalData( const QValueList<QByteArray>&, bool* );

  /**
     Is called when the client needs data to send to the server.
     When finished send an empty array here.
     When true is returned the data is sent. If false is returned
     the operation is aborted.
  */
  void signalDataReq( QByteArray&, size_t, bool* );

  /**
     Is emitted when the request has successfuly finished.
   */
  void signalRequestDone( QObexClient::Request );

  /**
     If we get an abort packet from the server this function is called.
  */
  void signalAborted( const QObexObject& );

  /**
     If we get an error while parsing or unexpected packets this
     function is called.
  */
  void signalError( QObexClient::Error );

protected:
  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual void response( const QObexObject& resp ) {
    emit signalResponse( resp );
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual void authenticationRequired( const QString& realm, bool uidreq ) {
    emit signalAuthenticationRequired( realm, uidreq );
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual bool data( const QValueList<QByteArray>& bl ) {
    bool b = true;
    emit signalData( bl, &b );
    return b;
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual bool dataReq( QByteArray& d, size_t sz ) {
    bool b = true;
    emit signalDataReq( d, sz, &b );
    return b;
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
   */
  virtual void requestDone( QObexClient::Request req ) {
    emit signalRequestDone( req );
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual void aborted( const QObexObject& resp ) {
    emit signalAborted( resp );
  }

  /**
     We support both signals and virtual inheritence.
     If you don't override those functions the default ones emit signals ...
  */
  virtual void error( QObexClient::Error e = InternalError ) {
    emit signalError( e );
  }

private:
  /**
     Reimplemented from QObexBase. Is connected to the transports error signal.
   */
  void transportError( QObexTransport::Error );

  /**
     Reimplemented from QObexBase. Is connected to the transports
     connected signal.
   */
  void transportConnected();

  /**
     Reimplemented from QObexBase. Is connected to the transports
     connection closed signal.
   */
  void transportConnectionClosed();

  /**
     Reimplemented from QObexBase. Is connected to the transports
     transportWritten signal.
   */
  void transportPacketWritten();

  /**
     Reimplemented from QObexBase. Is connected to the transports
     packet received signal.
   */
  void transportPacketRecieved( QObexObject& );
  /**
     Reimplemented from QObexBase. Is connected to the transports
     packet received signal.
   */
  void transportSelectModeChanged( QObexTransport::SelectHint, Q_LONG );

  /**
    Delete the current request and finishes execution.
  */
  void makeIdle();

  /**
     Sends a new request and, if it is called the first time in a synchronous
     command, cycles until the command has finished.
  */
  bool sendRequest( QObexObject* );

  void handleConnectResponse( QObexObject& );
  void handleDisconnectResponse( QObexObject& );
  void handleGetResponse( QObexObject& );
  void handlePutResponse( QObexObject& );
  void handleSetPathResponse( QObexObject& );
  void handleAbortResponse( QObexObject& );

  /**
     Keep track if the client is connected. (Not the transport!)
   */
  bool mConnected;

  /**
     The response code of the response.
     FIXME review this
     May be the client does some error handling itself which returns
     success. This is _NOT_ recorded here.
  */
  int mResponseCode;
  bool mIgnoreResponse;

  /**
     The pending request.
     Since the protocol has state we have to know what we have sent
     just to parse the incoming response.
  */
  QObexObject *mCurrentRequest;

  /**
     Directed connection handling.
     If the client should connect to a directed connection mUUID contains
     the uuid to use. If the client can not connect to this directed peer
     it disconnects and emit an error.
  */
  QByteArray mUUID;

  /**
     Append the connection id if we have one ...
  */
  Q_UINT32 mConnectionId;
  bool mUseConnectionId;

  /**
     If set the transport is left connected if we have finished
     a disconnect request.
  */
  bool mKeepTransport;

  /**
     If set the client sends an authentication challenge with the connect
     request. This way on triggers a permanent authentication sequence
     in the server.
  */
  bool mInitiateAuth;

  /**
     Variables used for authentication of the client by the server.
   */
  QObexAuthDigestResponse::AuthInfo mServerAuthInfo;
  bool mServerIsReadOnly;

  /**
     Variables used for authentication of the server by the client.
   */
  QObexAuthDigestChallenge::AuthInfo mClientAuthInfo;
  QByteArray mClientSecret;
  QValueList<QObexAuthDigestChallenge> mPendingAuth;
  bool mServerIsAuthenticated;

  /**
     Helper function for authentication. Returns true if the auth response
     matches one of our pending requests.
   */
  bool authResponseOk( const QObexAuthDigestResponse& );

  /**
     True if we should also accept wrong protocol version 0x11.
   */
  bool mOpenObexSupport;

  /**
     For better streaming the data write and read routines are processed in
     a slow codepath. These are required to do that.
   */
  QValueList<QByteArray> mStreamingBuffer;
  bool mStreamingOk;
  enum StreamingType {
    StreamingPut,
    StreamingGet,
    StreamingNone
  };
  StreamingType mStreamingType;

  /**
     Disable assignment
  */
  QObexClient( const QObexClient& );
  /**
     Disable assignment
  */
  QObexClient& operator=( const QObexClient& );

  QSocketNotifier* mReadNotifier;
  QSocketNotifier* mWriteNotifier;
};

#endif
