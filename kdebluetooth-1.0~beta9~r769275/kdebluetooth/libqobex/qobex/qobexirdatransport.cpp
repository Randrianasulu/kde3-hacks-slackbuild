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
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include <linux/types.h>
#include <linux/irda.h>

#include "qobexirdatransport.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexIrDATransport::QObexIrDATransport( QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexIrDATransport::QObexIrDATransport()" ));
  open();
  mSrc = 0;
  mDst = 0;
}

QObexIrDATransport::QObexIrDATransport( int fd, Q_UINT32 src, Q_UINT32 dst, QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexIrDATransport::QObexIrDATransport( %d )", fd ));
  setStatus( StatusConnected );
  mFd = fd;
  mSrc = src;
  mDst = dst;
  setBlocking( true );
}

QObexIrDATransport::~QObexIrDATransport() {
  myDebug(( "QObexIrDATransport::~QObexIrDATransport()" ));
  if ( 0 < mFd )
    ::close( mFd );
}

bool QObexIrDATransport::connect() {
  myDebug(( "QObexIrDATransport::connect()" ));

  // FIXME? Work out how to avoid this.
  // For now it's more reliable ...
  if ( 0 <= mFd ) {
    ::close( mFd );
    mFd = -1;
  }
  if ( !open() ) {
    error( ConnectionRefused );
    return false;
  }

  // If the user has not specified an address
  // discover devices speaking OBEX, take the first best ...
  if ( mDst == 0 ) {
    QMap<Q_UINT32,QString> devs = QObexIrDATransport::discoverDevices();
    if ( devs.isEmpty() ) {
      myDebug(( "Can not discover device" ));
      error( ConnectionRefused );
      return false;
    }

    setDestAddress( devs.keys().first() );
  }

  struct sockaddr_irda addr;
  addr.sir_addr = mDst;
  addr.sir_family = AF_IRDA;
  strncpy( addr.sir_name, "OBEX", sizeof(addr.sir_name) );
  if ( 0 > ::connect( mFd, (struct sockaddr*)&addr, sizeof(addr) ) ) {
    ::close( mFd );
    mFd = -1;
    myDebug(( "Can not connect to device: %s", strerror( errno ) ));
    error( ConnectionRefused );
    return false;
  }
  
  myDebug(( "Successfully opened irda device" ));
  
  connected();
  return true;
}

void QObexIrDATransport::disconnect() {
  myDebug(( "QObexIrDATransport::disconnect()" ));
  if ( 0 <= mFd ) {
    ::close( mFd );
    mFd = -1;
    connectionClosed();
  }
}

bool QObexIrDATransport::listen( int backlog ) {
  myDebug(( "QObexIrDATransport::listen()" ));

  // use any address here
  struct sockaddr_irda self;
  self.sir_addr = 0;
  self.sir_family = AF_IRDA;
  strncpy( self.sir_name, "OBEX", sizeof(self.sir_name) );

  int ret = ::bind( mFd, (struct sockaddr*)&self, sizeof(self) );
  if ( ret < 0 ) {
    setStatus( StatusError );
    error( BindError );
    return false;
  }

  // This brace is taken from openobex. Thanks!!
  /* Ask the IrDA stack to advertise the Obex hint bit - Jean II */
  /* Under Linux, it's a regular socket option */
  unsigned char hints[4];       /* Hint be we advertise */
  
  /* We want to advertise the OBEX hint bit */
  hints[0] = HINT_EXTENSION;
  hints[1] = HINT_OBEX;
  hints[2] = 0; // ???
  hints[3] = 0; // ???
  
  /* Tell the stack about it */
  setsockopt( mFd, SOL_IRLMP, IRLMP_HINTS_SET, hints, sizeof(hints) );
  
  ret = ::listen( mFd, backlog );
  if ( ret < 0 ) {
    setStatus( StatusError );
    error( ListenError );
    return false;
  }

  return true;
}

QObexTransport* QObexIrDATransport::accept() {
  myDebug(( "QObexIrDATransport::accept()" ));

  struct sockaddr_irda addr;
  socklen_t len = sizeof(addr);
  int newfd = ::accept( mFd, (struct sockaddr*)&addr, &len );
  if ( 0 < newfd )
    return new QObexIrDATransport( newfd, mSrc, addr.sir_addr );
  else {
    setStatus( StatusError );
    error( AcceptError );
    return 0;
  }
}

Q_LONG QObexIrDATransport::maximumTransferUnit() const {
  myDebug(( "QObexIrDATransport::maximumTransferUnit()" ));
  /* Check what the IrLAP data size is */
  unsigned int mtu = 0;
  socklen_t slen = sizeof(mtu);
  if ( ::getsockopt( mFd, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, (void*)&mtu, &slen) < 0 )
    return QObexTransport::MinimumMtu;
  else {
    myDebug(( "QObexIrDATransport::maximumTransferUnit() mtu = %d", mtu ));
    return mtu;
  }
}

Q_LONG QObexIrDATransport::bytesPerSecond() const {
  return 115200/10;
}

void QObexIrDATransport::setDestAddress( Q_UINT32 addr )
{
  mDst = htonl( addr );
}

void QObexIrDATransport::setDestAddress( const QString& addr )
{
  myDebug(( "QObexIrDATransport::setDestAddress( %s )", addr.ascii() ));
  setDestAddress( QObexIrDATransport::string2Address( addr ) );
}

Q_UINT32 QObexIrDATransport::destAddress() const
{
  return ntohl( mDst );
}

void QObexIrDATransport::setSrcAddress( Q_UINT32 addr )
{
  mSrc = htonl( addr );
}

void QObexIrDATransport::setSrcAddress( const QString& addr )
{
  myDebug(( "QObexIrDATransport::setSrcAddress( %s )", addr.ascii() ));
  setSrcAddress( QObexIrDATransport::string2Address( addr ) );
}

Q_UINT32 QObexIrDATransport::srcAddress() const
{
  return ntohl( mSrc );
}

bool QObexIrDATransport::open() {
  myDebug(( "QObexIrDATransport::open()" ));

  int fd = ::socket( AF_IRDA, SOCK_STREAM, 0);
  if ( fd < 0 ) {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }
  
  setStatus( StatusOpen );
  mFd = fd;
  setBlocking( true );  

  return true;
}

Q_UINT32 QObexIrDATransport::string2Address( const QString& saddr )
{
  QStringList parts = QStringList::split( QChar( ':' ), saddr );
  Q_UINT32 a0 = parts[0].toUInt( 0, 16 );
  Q_UINT32 a1 = parts[1].toUInt( 0, 16 );
  Q_UINT32 a2 = parts[2].toUInt( 0, 16 );
  Q_UINT32 a3 = parts[3].toUInt( 0, 16 );

  return htonl( a0<<24 | a1<<16 | a2<<8 | a3 );
}

QString QObexIrDATransport::address2String( Q_UINT32 addr )
{
  Q_UINT32 haddr = ntohl( addr );
  QString str;
  return str.sprintf( "%2.2X:%2.2X:%2.2X:%2.2X", (haddr&0xff000000)>>24,
		      (haddr&0x00ff0000)>>16, (haddr&0x0000ff00)>>8,
		      (haddr&0x000000ff) );
}

QMap<Q_UINT32,QString> QObexIrDATransport::discoverDevices() {
  myDebug(( "QObexIrDATransport::discoverDevices()" ));
  QMap<Q_UINT32,QString> res;

  int fd = ::socket( AF_IRDA, SOCK_STREAM, 0);
  if ( fd < 0 )
    return res;
  
  // Code taken from openobex. Thanks!!!
  unsigned char hints[4];       /* Hint be we filter on */
  struct {
    __u32 len;
    struct irda_device_info dev[ 16 ]; // 16 must be enough
  } devs;
  socklen_t slen = sizeof(devs);

  /* We want only devices that advertise OBEX hint */
  hints[0] = HINT_EXTENSION;
  hints[1] = HINT_OBEX;
  /* Set the filter used for performing discovery */
  if ( ::setsockopt( fd, SOL_IRLMP, IRLMP_HINT_MASK_SET,
		     hints, sizeof( hints ) ) )
    goto error_return;

  /* Perform a discovery and get device list */
  if ( ::getsockopt( fd, SOL_IRLMP, IRLMP_ENUMDEVICES, (void*)&devs, &slen ) )
    goto error_return;

  /* Did we get any ? (in some rare cases, this test is true) */
  if ( devs.len <= 0 )
    goto error_return;
  
  /* List all Obex devices : Linux case */
  myDebug(( "Discovered %d IrDA devices", devs.len ));
  for( unsigned int i = 0; i < devs.len; ++i ) {
    struct irda_ias_set ias_query;
    /* Ask if the requested service exist on this device */
    slen = sizeof(ias_query);
    ias_query.daddr = devs.dev[i].daddr;
    ::strcpy( ias_query.irda_class_name, "OBEX" );
    ::strcpy( ias_query.irda_attrib_name, "IrDA:TinyTP:LsapSel" );
    /* Check if we failed */
    if( 0 == ::getsockopt( fd, SOL_IRLMP, IRLMP_IAS_QUERY,
			   &ias_query, &slen) ) {
      QString name = QObexIrDATransport::decodeString( devs.dev[i].charset, devs.dev[i].info,
				       sizeof(devs.dev[i].info) );
      /* Pick this device */
      res[ ntohl( devs.dev[i].daddr ) ] = name;
      myDebug(( " name = %s, addr = 0x%08x", name.latin1(),
		devs.dev[i].daddr ));
    }
  }

 error_return:
  ::close( fd );
  return res;
}

QString QObexIrDATransport::decodeString( int cs, char* str, int maxlen ) {
  // make sure it is 0 terminated.
  str[maxlen-1] = 0;

  QTextCodec* codec = 0;
  switch ( cs ) {
  case CS_ASCII:
  case CS_ISO_8859_1:
    codec = QTextCodec::codecForName( "ISO8859-1" );
    break;
  case CS_ISO_8859_2:
    codec = QTextCodec::codecForName( "ISO8859-2" );
    break;
  case CS_ISO_8859_3:
    codec = QTextCodec::codecForName( "ISO8859-3" );
    break;
  case CS_ISO_8859_4:
    codec = QTextCodec::codecForName( "ISO8859-4" );
    break;
  case CS_ISO_8859_5:
    codec = QTextCodec::codecForName( "ISO8859-5" );
    break;
  case CS_ISO_8859_6:
    codec = QTextCodec::codecForName( "ISO8859-6" );
    break;
  case CS_ISO_8859_7:
    codec = QTextCodec::codecForName( "ISO8859-7" );
    break;
  case CS_ISO_8859_8:
    codec = QTextCodec::codecForName( "ISO8859-8" );
    break;
  case CS_ISO_8859_9:
    codec = QTextCodec::codecForName( "ISO8859-9" );
    break;
  case CS_UNICODE:
    codec = QTextCodec::codecForName( "utf-16" );
    str[maxlen-2] = 0;
    break;
  }

  if ( !codec )
    codec = QTextCodec::codecForContent ( str, maxlen );

  if ( codec )
    return codec->toUnicode( str );
  
  return QString::null;
}
