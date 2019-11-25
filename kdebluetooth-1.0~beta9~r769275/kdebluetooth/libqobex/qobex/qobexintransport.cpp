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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <qobject.h>

#include "qobexintransport.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexInTransport::QObexInTransport( QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexInTransport::QObexInTransport( ... )" ));
//   ::memcpy( &mSrc, &INADDR_ANY, sizeof(in_addr_t) );
//   ::memcpy( &mDst, &INADDR_ANY, sizeof(in_addr_t) );
  mSrc = 0;
  mDst = 0;
  open();
  mPort = 0;
}

QObexInTransport::QObexInTransport( int fd, const in_addr_t* src, const in_addr_t* dst, QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexInTransport::QObexInTransport( ... )" ));
  ::memcpy( &mSrc, src, sizeof(in_addr_t) );
  ::memcpy( &mDst, dst, sizeof(in_addr_t) );
  setStatus( StatusConnected );
  mFd = fd;
  setBlocking( true );
}

QObexInTransport::~QObexInTransport() {
  myDebug(( "QObexInTransport::~QObexInTransport()" ));
  if ( 0 <= mFd )
    ::close( mFd );
}

int QObexInTransport::socket() const {
  myDebug(( "QObexInTransport::socket()" ));
  return mFd;
}

bool QObexInTransport::connect() {
  myDebug(( "QObexInTransport::connect()" ));
  if ( mFd < 0 )
    if ( !open() ) {
      error( ConnectionRefused );
      return false;
    }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  ::memcpy( &addr.sin_addr.s_addr, &mDst, sizeof( addr.sin_addr.s_addr ));
  if ( mPort == 0 ) {
    addr.sin_port = defaultObexPort();
    if ( 0 > ::connect( mFd, (struct sockaddr*)&addr, sizeof( addr ) ) ) {
      // Try again on the fallback port for non root servers
      addr.sin_port = htons( ObexUserPort );
      if ( 0 > ::connect( mFd, (struct sockaddr*)&addr, sizeof( addr ) ) ) {
	error( ConnectionRefused );
	return false;
      }
    }
  } else {
    addr.sin_port = htons( mPort );
    if ( 0 > ::connect( mFd, (struct sockaddr*)&addr, sizeof( addr ) ) ) {
      error( ConnectionRefused );
      return false;
    }
  }

  myDebug(( "Successfully opened in device" ));

  connected();
  return true;
}

void QObexInTransport::disconnect() {
  myDebug(( "QObexInTransport::disconnect()" ));
  if ( 0 <= mFd ) {
    ::close( mFd );
    mFd = -1;
    connectionClosed();
  }
}

void QObexInTransport::setHost( const QString& host ) {
  myDebug(( "QObexInTransport::setHost( %s )", host.ascii() ));
  if ( host.length() <= 0 )
    return;
  // It's save to use ascii() here since DNS hostnames are
  // required to be plain ascii.
  struct hostent *lh = ::gethostbyname( host.ascii() );
  if ( lh && lh->h_length == sizeof(mDst) )
    ::memcpy( &mDst, lh->h_addr, lh->h_length );
}

void QObexInTransport::setSrc( const QString& src ) {
  myDebug(( "QObexInTransport::setSrc( %s )", src.ascii() ));
  if ( src.length() <= 0 )
    return;
  // It's save to use ascii() here since DNS hostnames are
  // required to be plain ascii.
  struct hostent *lh = ::gethostbyname( src.ascii() );
  if ( lh && lh->h_length == sizeof(mSrc) )
    ::memcpy( &mSrc, lh->h_addr, lh->h_length );
}

void QObexInTransport::setPort( int port ) {
  myDebug(( "QObexInTransport::setPort( %d )", port ));
  mPort = port;
}

bool QObexInTransport::listen( int backlog ) {
  myDebug(( "QObexInTransport::listen()" ));

  // listen on any addr on the default port
  struct sockaddr_in self;
  self.sin_family = AF_INET;
  ::memcpy( &self.sin_addr.s_addr, &mSrc, sizeof( self.sin_addr.s_addr ));
  if ( mPort == 0 )
    self.sin_port = QObexInTransport::defaultObexPort();
  else
    self.sin_port = htons( mPort );
  int ret = ::bind( mFd, (struct sockaddr*)&self, sizeof( self ) );
  if ( ret < 0 ) {
    // try again with an non priveleged port
    myDebug(( "QObexInTransport::listen(): bind: try again with user port" ));
    self.sin_port = htons( ObexUserPort );
    ret = ::bind( mFd, (struct sockaddr*)&self, sizeof( self ) );
  }
  if ( ret < 0 ) {
    setStatus( StatusError );
    error( BindError );
    return false;
  }

  ret = ::listen( socket(), backlog );
  if ( ret < 0 ) {
    setStatus( StatusError );
    error( ListenError );
    return false;
  }

  return true;
}

QObexTransport* QObexInTransport::accept() {
  myDebug(( "QObexInTransport::accept()" ));
  struct sockaddr_in dst;
  socklen_t len = sizeof(dst);
  int newfd = ::accept( mFd, (struct sockaddr*)&dst, &len );
  if ( 0 < newfd )
    return new QObexInTransport( newfd, &mSrc, &dst.sin_addr.s_addr );
  else {
    setStatus( StatusError );
    error( AcceptError );
    return 0;
  }
}

Q_LONG QObexInTransport::maximumTransferUnit() const {
  return 1024;
}

Q_LONG QObexInTransport::bytesPerSecond() const {
  return 10*1024*1024/10;
}

bool QObexInTransport::open() {
  myDebug(( "QObexInTransport::open()" ));
  int fd = ::socket( AF_INET, SOCK_STREAM, 0);
  if ( fd < 0 ) {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }
  mFd = fd;
  setStatus( StatusOpen );
  setBlocking( true );  

  return true;
}

int QObexInTransport::defaultObexPort() {
  myDebug(( "QObexInTransport::defaultObexPort()" ));
  // FIXME is it called obex???
  struct servent *obexServ = ::getservbyname( "obex", "tcp" );
  if ( obexServ ) {
    myDebug(( "QObexInTransport::defaultObexPort() port is %d", ntohs( obexServ->s_port ) ));
    return obexServ->s_port;
  } else {
    myDebug(( "QObexInTransport::defaultObexPort() using builtin default port %d", DefaultObexPort ));
    return htons( DefaultObexPort );
  }
}
