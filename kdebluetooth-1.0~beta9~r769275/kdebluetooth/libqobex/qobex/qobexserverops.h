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

#ifndef QOBEXSERVEROPS_H
#define QOBEXSERVEROPS_H

#include <qcstring.h>
#include <qvaluelist.h>

#include "qobexauth.h"
#include "qobexobject.h"
#include "qobexheader.h"

class QObexServerOps {
public:
  /**
     Create a new set of QObexServerOps.
   */
  QObexServerOps();

  /**
     Create a new set of QObexServerOps.
   */
  virtual ~QObexServerOps();

  /**
     Is called by the server when a connect request successfully arrived.
   */
  virtual void connect( const QObexObject&, QObexObject& );

  /**
     Is called by the server when a disconnect request arrived.
   */
  virtual void disconnect( const QObexObject& );

  /**
     Is called by the server when a get request arrived.
   */
  virtual void get( const QObexObject&, QObexObject& );

  /**
     Is called by the server when an empty put request arrived.
   */
  virtual void del( const QObexObject&, QObexObject& );

  /**
     Is called by the server when a put request arrived.
   */
  virtual void put( const QObexObject&, QObexObject& );

  /**
     Is called by the server when a set path request arrived.
     Note that the server application itself does not maintain the current
     working directory. The user itself should keep track of the cwd inside
     this function.
   */
  virtual void setPath( const QObexObject&, QObexObject& );

  /**
     Is called by the server when an abort request arrived.
   */
  virtual void abort( const QObexObject& );

  /**
     Is called by the server when an unexpected request arrives during a
     streaming operation.
   */
  virtual void streamingError();

  /**
     Gets called when a get request needs data. Return an empty array on EOF.
   */
  virtual bool dataReq( QByteArray&, Q_LONG );

  /**
     Gets called when a put request deliverd data.
   */
  virtual bool data( const QValueList<QByteArray>& );

  /**
     Return true if your implementation is able to handle connections
     with the given uuid.
   */
  virtual bool canHandle( const QByteArray& ) = 0;

  /**
     Return a new instance of server ops. This is called when a new connection
     request arrives and canHandle returns true.
   */
  virtual QObexServerOps* clone() = 0;

  /**
     Should return the realm and ooptions to send with an
     authentication challange header.
   */
  virtual QObexAuthDigestChallenge::AuthInfo serverAuthInfo();

  /**
     When authentication is done the serverops should return the secret
     which is shared with the client.
   */
  virtual QByteArray serverSecret( const QString& clientUserId );

  /**
     When authentication is done the serverops should return the secret
     matching realm. This one is used for autentication checking.
   */
  virtual QObexAuthDigestResponse::AuthInfo clientAuthInfo( const QString& realm, bool userRequired );

  /**
     Returns true if the client is authenticated.
   */
  bool isAuthenticated() const {
    return mAuthStatus == Authenticated || mAuthStatus == TempAuthenticated;
  }

  /**
     Returns if the client has set the read only flag in the auth challenge.
   */
  bool clientReadOnly() const { return mClientIsReadOnly; }

  /**
     Returns if we are connetcted.
   */
  bool isConnected() const { return mConnected; }

  /**
     Returns the current working directory.
   */
  QString getCwd() const;

  /**
     Returns if we are in the root directory.
   */
  bool isRootDir() const;

  /**
     Steps one directory component higher.
   */
  bool oneDirectoryUp();

  /**
     Enters into given directory.
   */
  bool enterDirectory( const QString&, bool=false );

  /**
     Enters into the root directory/clears cwd.
   */
  void enterRootDirectory();

  /**
     Composes a pathname.
   */
  QString composePath( const QString&, bool=false );

  /**
     Retunrs true if we have a directed connection.
   */
  bool isDirected() const;

  /**
     Returns the current uuid;
   */
  QByteArray getUuid() const;

  /**
     Returns the current connection id.
   */
  Q_UINT32 getConnectionId() const;

 private:
  /**
     Holds the current working directory.
   */
  QString mCwd;

  /**
     FIXME obsolete since we don't assign connectionid 0
     Is true if we have a connection id.
   */
  bool mUseConnectionId; 

  /**
     Holds the connection id itself.
   */
  Q_UINT32 mConnectionId;
  
  /**
     Holds the uuid.
   */
  QByteArray mUuid;


  bool mConnected;

  enum StreamingStatus {
    NoStreaming,
    StreamingPut,
    PreparingPut,
    StreamingGet,
    PreparingGet
  };

  /**
     Holds the streaming status.
   */
  StreamingStatus mStreamingStatus;

  enum AuthenticationStatus {
    UnAuthenticated,
    Authenticated,
    TempAuthenticated,
    AuthenticationFailed
  };

  /**
     Holds the authentication status.
   */
  AuthenticationStatus mAuthStatus;

  bool mClientIsReadOnly;
  QValueList<QObexAuthDigestChallenge> mPendingAuth;

  bool authResponseOk( const QObexAuthDigestResponse& );

  bool havePendingAuthForNonce( const QByteArray& nonce ) const;

  /**
     Dump incomplete get headers here.
   */
  QValueList<QObexHeader> mHeaders;

  /**
     Friend classes.
   */
  friend class QObexServerConnection;
};

#endif
