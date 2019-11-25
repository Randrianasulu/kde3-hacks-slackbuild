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

#include <qstring.h>
#include <qregexp.h>
#include <qcstring.h>
#include <qdir.h>

#include "qobexobject.h"
#include "qobexserverops.h"
#include "qobexauth.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexServerOps::QObexServerOps() {
  myDebug(( "QObexServerOps::QObexServerOps()" ));
  mUseConnectionId = false;
  mStreamingStatus = NoStreaming;
  mConnected = false;
  mAuthStatus = UnAuthenticated;
  mClientIsReadOnly = false;
}

QObexServerOps::~QObexServerOps() {
  myDebug(( "QObexServerOps::~QObexServerOps()" ));
}

void QObexServerOps::connect( const QObexObject&, QObexObject& resp ) {
  myDebug(( "QObexServerOps::connect( ... )" ));
  resp.setCode( QObexObject::NotImplemented );
}

void QObexServerOps::disconnect( const QObexObject& ) {
  myDebug(( "QObexServerOps::disconnect( ... )" ));
}

void QObexServerOps::get( const QObexObject&, QObexObject& resp ) {
  myDebug(( "QObexServerOps::get( ... )" ));
  resp.setCode( QObexObject::NotImplemented );
}

void QObexServerOps::del( const QObexObject&, QObexObject& resp ) {
  myDebug(( "QObexServerOps::del( ... )" ));
  resp.setCode( QObexObject::NotImplemented );
}

void QObexServerOps::put( const QObexObject&, QObexObject& resp ) {
  myDebug(( "QObexServerOps::put( ... )" ));
  resp.setCode( QObexObject::NotImplemented );
}

void QObexServerOps::setPath( const QObexObject&, QObexObject& resp ) {
  myDebug(( "QObexServerOps::setPath( ... )" ));
  resp.setCode( QObexObject::NotImplemented );
}

void QObexServerOps::abort( const QObexObject& ) {
  myDebug(( "QObexServerOps::abort( ... )" ));
}

void QObexServerOps::streamingError() {
  myDebug(( "QObexServerOps::streamingError()" ));
}

bool QObexServerOps::dataReq( QByteArray&, Q_LONG ) {
  myDebug(( "QObexServerOps::dataReq( ... )" ));
  return true;
}

bool QObexServerOps::data( const QValueList<QByteArray>& ) {
  myDebug(( "QObexServerOps::data( ... )" ));
  return true;
}

QObexAuthDigestChallenge::AuthInfo QObexServerOps::serverAuthInfo() {
  myDebug(( "QObexServerOps::serverAuthInfo( ... )" ));
  return QObexAuthDigestChallenge::AuthInfo();
}

QByteArray QObexServerOps::serverSecret( const QString& ) {
  myDebug(( "QObexServerOps::serverSecret( ... )" ));
  return QByteArray();
}

QObexAuthDigestResponse::AuthInfo QObexServerOps::clientAuthInfo( const QString&, bool ) {
  myDebug(( "QObexServerOps::authenticationRequired( ... )" ));
  return QObexAuthDigestResponse::AuthInfo();
}

QString QObexServerOps::getCwd() const {
  myDebug(( "QObexServerOps::getCwd()" ));
  return mCwd;
}

bool QObexServerOps::isRootDir() const {
  myDebug(( "QObexServerOps::isRootDir()" ));
  return mCwd.isEmpty() || mCwd == ".";
}

bool QObexServerOps::oneDirectoryUp() {
  myDebug(( "QObexServerOps::oneDirectoryUp(): enter %s", mCwd.latin1() ));
  if ( isRootDir() )
    return false;

  mCwd = QDir::cleanDirPath ( "./" + mCwd + "/.." );
  myDebug(( "QObexServerOps::oneDirectoryUp(): exit %s", mCwd.latin1()));
  return true;
}

bool QObexServerOps::enterDirectory( const QString& dir, bool backup ) {
  myDebug(( "QObexServerOps::enterDirectory( %s ): enter %s", dir.latin1(), mCwd.latin1() ));
  if ( backup && isRootDir() )
    return false;

  mCwd = QDir::cleanDirPath( "./" + mCwd + ( backup ? "/../" : "/" ) + dir );
  myDebug(( "QObexServerOps::enterDirectory( %s ): exit %s", dir.latin1(), mCwd.latin1() ));
  return true;
}

void QObexServerOps::enterRootDirectory() {
  myDebug(( "QObexServerOps::enterRootDirectory()" ));
  mCwd = QString::null;
}

QString QObexServerOps::composePath( const QString& tail, bool backup ) {
  myDebug(( "QObexServerOps::composePath( ... )" ));
  return QDir::cleanDirPath ( "./" + mCwd + ( backup ? "/../" : "/" ) + tail );
}

bool QObexServerOps::isDirected() const {
  myDebug(( "QObexServerOps::isDirected()" ));
  return mUseConnectionId;
}

QByteArray QObexServerOps::getUuid() const {
  myDebug(( "QObexServerOps::getUuid()" ));
  return mUuid;
}

Q_UINT32 QObexServerOps::getConnectionId() const {
  myDebug(( "QObexServerOps::getConnectionId()" ));
  return mConnectionId;
}

bool QObexServerOps::authResponseOk( const QObexAuthDigestResponse& auth ) {
  myDebug(( "QObexServerOps::authResponseOk( ... )" ));
  if ( auth.hasNonce() ) {
    QByteArray nonce = auth.nonce();
    if ( havePendingAuthForNonce( nonce ) && auth.authenticate( serverSecret( auth.userId() ), nonce ) ) {
      mPendingAuth.clear();
      return true;
    }
  }
  else {
    QValueList<QObexAuthDigestChallenge>::Iterator it;
    for ( it = mPendingAuth.begin(); it != mPendingAuth.end(); ++it ) {
      if ( auth.authenticate( serverSecret( auth.userId() ), (*it).nonce() ) ) {
        mPendingAuth.clear();
        return true;
      }
    }
  }
  return false;
}

bool QObexServerOps::havePendingAuthForNonce( const QByteArray& nonce ) const {
  myDebug(( "QObexServerOps::havePendingAuthForNonce( ... )" ));
  QValueList<QObexAuthDigestChallenge>::ConstIterator it;
  for ( it = mPendingAuth.begin(); it != mPendingAuth.end(); ++it ) {
    if ( (*it).nonce() == nonce )
      return true;
  }
  return false;
}
