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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <qstring.h>
#include <qstringlist.h>
#include "qobexbttransport.h"

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

#ifdef SDP_RETRY_IF_BUSY
#define SDP_STANDARD_CONNECT_FLAGS SDP_RETRY_IF_BUSY
#else
#define SDP_STANDARD_CONNECT_FLAGS 0
#endif

QObexBtTransport::QObexBtTransport( QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexBtTransport::QObexBtTransport( ... )" ));
  bacpy( &mDest, BDADDR_ANY );
  bacpy( &mSrc, BDADDR_ANY );
  mChannel = -1;

  open();

  mRegisterSdp = true;
  mSdpHandle = 0;
}

QObexBtTransport::QObexBtTransport( int fd, const bdaddr_t* src, const bdaddr_t*dst, QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexBtTransport::QObexBtTransport( ... )" ));
  bacpy( &mSrc, src );
  bacpy( &mDest, dst );
  mFd = fd;
  setBlocking( true );  

  setStatus( StatusConnected );
  
  mRegisterSdp = false;
  mSdpHandle = 0;
  bacpy( &mSrc, BDADDR_ANY );
  mChannel = -1;
}

QObexBtTransport::~QObexBtTransport() {
  myDebug(( "QObexBtTransport::~QObexBtTransport()" ));
  if ( 0 <= mFd )
    ::close( mFd );

  if ( mRegisterSdp && mSdpHandle )
    unregisterSdp();
}

bool QObexBtTransport::open() {
  myDebug(( "QObexBtTransport::open()" ));
  int fd = ::socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );
  myDebug(( "QObexBtTransport::open(): fd = %d", fd ));
  if ( fd < 0 ) {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }
  mFd = fd;
  
  setStatus( StatusOpen );
  setBlocking( true );
  myDebug(( "QObexBtTransport::open(): line %d, fd = %d", __LINE__, fd ));

  return true;
}

bool QObexBtTransport::connect() {
  myDebug(( "QObexBtTransport::connect() %d", getpid() ));

  if ( !open() ) {
    error( ConnectionRefused );
    return false;
  }

  myDebug(( "QObexBtTransport::connect(): Opened device" ));

  // If the user has not specified an address
  // discover devices speaking OBEX, take the first best ...
  if ( 0 == bacmp( &mDest, BDADDR_ANY ) ) {
    if ( !discover() ) {
      myDebug(( "QObexBtTransport::connect(): Can not discover device" ));
      error( ConnectionRefused );
      return false;
    }
  } else if ( mChannel <= 0 ) {
    mChannel = getRfCommChannel( mDest, mUuid );
    if ( 0 < mChannel ) {
      myDebug(( "QObexBtTransport::connect(): Got channel %d", mChannel ));
    } else {
      myDebug(( "QObexBtTransport::connect(): Can not get rfcomm proto" ));
      error( ConnectionRefused );
      return false;
    }
  }


  struct sockaddr_rc sa;
  sa.rc_family = AF_BLUETOOTH;
  bacpy( &sa.rc_bdaddr, &mSrc );
  sa.rc_channel = 0;
  if ( ::bind( mFd, (struct sockaddr*) &sa, sizeof(sa) ) < 0 ) {
    ::close( mFd );
    mFd = -1;
    myDebug(( "QObexBtTransport::connect(): Can not bind to device: %s", strerror( errno ) ));
    error( BindError );
    return false;
  }

  sa.rc_family = AF_BLUETOOTH;
  bacpy( &sa.rc_bdaddr, &mDest );
  sa.rc_channel = mChannel;
  if ( 0 > ::connect( socket(), (struct sockaddr*)&sa, sizeof( sa ) ) ) {
    ::close( mFd );
    mFd = -1;
    myDebug(( "QObexBtTransport::connect(): Can not connect to device: %s", strerror( errno ) ));
    error( ConnectionRefused );
    return false;
  }
  
  myDebug(( "QObexBtTransport::connect(): Successfully opened bluetooth device" ));
  
  connected();
  return true;
}

void QObexBtTransport::disconnect() {
  myDebug(( "QObexBtTransport::disconnect()" )); 
  if ( 0 <= mFd ) {
    ::close( mFd );
    mFd = -1;
    connectionClosed();
  }
}

bool QObexBtTransport::listen( int backlog ) {
  myDebug(( "QObexBtTransport::listen()" ));

  if ( mRegisterSdp )
    registerSdp();

  struct sockaddr_rc sa;
  sa.rc_family = AF_BLUETOOTH;
  bacpy( &sa.rc_bdaddr, &mSrc );
  sa.rc_channel = mChannel;
  if ( ::bind( mFd, (struct sockaddr*)&sa, sizeof(sa) ) < 0 ) {
    setStatus( StatusError );
    error( BindError );
    return false;
  }

  if ( ::listen( socket(), backlog ) < 0 ) {
    setStatus( StatusError );
    error( ListenError );
    return false;
  }
  
  return true;
}

QObexTransport* QObexBtTransport::accept() {
  myDebug(( "QObexBtTransport::accept()" ));
  
  struct sockaddr_rc sa;
  socklen_t len = sizeof ( sa );
  int newfd = ::accept( socket(), (struct sockaddr*)&sa, &len );
  if ( newfd < 0 ) {
    setStatus( StatusError );
    error( AcceptError );
    return 0;
  }
  
  return new QObexBtTransport( newfd, &mSrc, &sa.rc_bdaddr );
}

Q_LONG QObexBtTransport::maximumTransferUnit() const {
  return RFCOMM_DEFAULT_MTU;
}

Q_LONG QObexBtTransport::bytesPerSecond() const {
  return 128*1024/10;
}

void QObexBtTransport::setDestAddress( bdaddr_t addr ) {
  bacpy( &mDest, &addr );
}

void QObexBtTransport::setDestAddress( const QString& addr )
{
  myDebug(( "QObexBtTransport::setAddress( %s )", addr.ascii() ));
  setDestAddress( QObexBtTransport::string2Address( addr ) );
}

bdaddr_t QObexBtTransport::destAddress() const {
  return mDest;
}

void QObexBtTransport::setSrcAddress( bdaddr_t addr ) {
  bacpy( &mSrc, &addr );
}

void QObexBtTransport::setSrcAddress( const QString& addr )
{
  myDebug(( "QObexBtTransport::setAddress( %s )", addr.ascii() ));
  setSrcAddress( QObexBtTransport::string2Address( addr ) );
}

bdaddr_t QObexBtTransport::srcAddress() const {
  return mSrc;
}

bdaddr_t QObexBtTransport::string2Address( const QString& saddr )
{
  Q_UINT8 a[6] = { 0, 0, 0, 0, 0, 0 };
  QStringList parts = QStringList::split( QChar( ':' ), saddr );

  a[0] = parts[0].toUInt( 0, 16 );
  a[1] = parts[1].toUInt( 0, 16 );
  a[2] = parts[2].toUInt( 0, 16 );
  a[3] = parts[3].toUInt( 0, 16 );
  a[4] = parts[4].toUInt( 0, 16 );
  a[5] = parts[5].toUInt( 0, 16 );

  bdaddr_t addr;
  baswap( &addr, (bdaddr_t *)a );

  return addr;
}

QString QObexBtTransport::address2String( bdaddr_t addr )
{
  Q_UINT8 b[6];
  baswap( (bdaddr_t *)b, &addr );
  QString str;
  return str.sprintf( "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
		      b[0], b[1], b[2], b[3], b[4], b[5] );
}

QValueList<bdaddr_t> QObexBtTransport::inquireDevices()
{
  myDebug(( "QObexBtTransport::inquireDevices()" ));

  // Get all avlaiable bluetooth devices. May take long time ...
  inquiry_info* iiPtr = 0;
  int numfound = hci_inquiry( -1, 10, 0, NULL, &iiPtr, 0 );

  QValueList<bdaddr_t> ret;
  for ( int i = 0; i < numfound; ++i )
    ret.append( iiPtr[i].bdaddr );

  if ( iiPtr )
    ::free( iiPtr );

  return ret;
}

QString QObexBtTransport::getRemoteName( bdaddr_t addr )
{
  myDebug(( "QObexBtTransport::getRemoteName( %s )", address2String( addr ).ascii() ));
  int devid = hci_get_route( &addr );
  if ( devid < 0 )
    return QString::null;

  myDebug(( "QObexBtTransport::getRemoteName(): devid = %d", devid ));
  int dev = hci_open_dev( devid );
  char name[249] = { 0, };
  if ( 0 != hci_read_remote_name( dev, &addr, sizeof(name), name, 5000 ) )
    hci_read_remote_name( dev, &addr, sizeof(name), name, 15000 );

  hci_close_dev( dev );
  myDebug(( "QObexBtTransport::getRemoteName(): name = %s", name ));
  return QString::fromUtf8( name );
}

QString QObexBtTransport::getRemoteName( const QString& addr )
{
  return getRemoteName( QObexBtTransport::string2Address( addr ) );
}

int QObexBtTransport::getRfCommChannel( const bdaddr_t addr, Q_UINT16 uuid )
{
  myDebug(( "QObexBtTransport::getRfCommChannel( %s )", address2String( addr ).ascii() ));

  // If this profile is not found return an error.
  int channel = -1;

  sdp_session_t *sdp = sdp_connect( BDADDR_ANY, &addr, SDP_STANDARD_CONNECT_FLAGS );
  if ( sdp ) {
    myDebug(( "QObexBtTransport::getRfCommChannel(): sdp session opened" ));
    
    uuid_t svcUuid;
    sdp_uuid16_create( &svcUuid, uuid );
    sdp_list_t *services = sdp_list_append( 0, &svcUuid );
      
    unsigned int range = 0x0000ffff;
    sdp_list_t *attributes = sdp_list_append( 0, &range );
    
    sdp_list_t *rsp_list = 0;
    sdp_service_search_attr_req( sdp, services, SDP_ATTR_REQ_RANGE,
				 attributes, &rsp_list );
    sdp_list_free( services, 0 );
    sdp_list_free( attributes, 0 );

    if ( rsp_list ) {
      sdp_list_t *access = 0;
      sdp_get_access_protos( (sdp_record_t*) rsp_list->data, &access );

      if ( access )
	channel = sdp_get_proto_port( access, RFCOMM_UUID );

      sdp_list_free( rsp_list, free );
    }

    // Close the sdp connection for this address.
    sdp_close( sdp );
  }

  return channel;
}

int QObexBtTransport::getRfCommChannel( const QString& addr, Q_UINT16 uuid )
{
  return getRfCommChannel( QObexBtTransport::string2Address( addr ), uuid );
}

bool QObexBtTransport::registerSdp()
{
  myDebug(( "QObexBtTransport::registerSdp()" ));

  sdp_session_t *sdp = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_STANDARD_CONNECT_FLAGS );
  if ( !sdp )
    return false;

  myDebug(( "QObexBtTransport::registerSdp(): sdp session opened" ));
  
  sdp_record_t record;
  uuid_t root_uuid;
  memset( (void *)&record, 0, sizeof(sdp_record_t) );
  record.handle = 0xffffffff;
  sdp_uuid16_create( &root_uuid, PUBLIC_BROWSE_GROUP );
  sdp_list_t *root = sdp_list_append( 0, &root_uuid );
  sdp_set_browse_groups( &record, root );
  
  sdp_profile_desc_t profile[1];
  sdp_list_t *proto[3];
  
  uuid_t svc_uuid;
  switch ( mUuid ) {
  case ObexFileServ:
    sdp_uuid16_create( &svc_uuid, OBEX_FILETRANS_SVCLASS_ID );
    break;
  case IrMCServ:
    sdp_uuid16_create( &svc_uuid, IRMC_SYNC_SVCLASS_ID );
    break;
  case IrMCCmdServ:
    sdp_uuid16_create( &svc_uuid, IRMC_SYNC_CMD_SVCLASS_ID );
    break;
  default:
    sdp_uuid16_create( &svc_uuid, OBEX_OBJPUSH_SVCLASS_ID );
    break;
  }
  sdp_list_t *svclass_id = sdp_list_append( 0, &svc_uuid );
  sdp_set_service_classes( &record, svclass_id );
  
  switch ( mUuid ) {
  case ObexFileServ:
    sdp_uuid16_create( &profile[0].uuid, OBEX_FILETRANS_PROFILE_ID );
    break;
  case IrMCServ:
    sdp_uuid16_create( &profile[0].uuid, IRMC_SYNC_PROFILE_ID );
    break;
  case IrMCCmdServ:
    sdp_uuid16_create( &profile[0].uuid, IRMC_SYNC_CMD_PROFILE_ID );
    break;
  default:
    sdp_uuid16_create( &profile[0].uuid, OBEX_OBJPUSH_PROFILE_ID );
    break;
  }
  profile[0].version = 0x0100;
  sdp_list_t *pfseq = sdp_list_append( 0, profile );
  sdp_set_profile_descs( &record, pfseq );
  
  uuid_t l2cap_uuid;
  sdp_uuid16_create( &l2cap_uuid, L2CAP_UUID );
  proto[0] = sdp_list_append( 0, &l2cap_uuid );
  sdp_list_t *apseq = sdp_list_append( 0, proto[0] );
  
  uuid_t rfcomm_uuid;
  sdp_uuid16_create( &rfcomm_uuid, RFCOMM_UUID );
  proto[1] = sdp_list_append( 0, &rfcomm_uuid );
  uint8_t chan = htobs( mChannel );
  sdp_data_t *channel = sdp_data_alloc( SDP_UINT8, &chan );
  proto[1] = sdp_list_append( proto[1], channel );
  apseq = sdp_list_append( apseq, proto[1] );
  
  uuid_t obex_uuid;
  sdp_uuid16_create( &obex_uuid, OBEX_UUID );
  proto[2] = sdp_list_append( 0, &obex_uuid );
  apseq = sdp_list_append( apseq, proto[2] );
  
  sdp_list_t *aproto = sdp_list_append( 0, apseq );
  sdp_set_access_protos( &record, aproto );
  sdp_data_free( channel );
  sdp_list_free( proto[0], 0 );
  sdp_list_free( proto[1], 0 );
  sdp_list_free( proto[2], 0 );
  sdp_list_free( apseq, 0 );
  sdp_list_free( aproto, 0 );
  
  uint8_t formats[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  uint8_t dtd = SDP_UINT8;
  void *dtds[sizeof(formats)], *values[sizeof(formats)];
  for ( unsigned int i = 0; i < sizeof( formats ); ++i ) {
    dtds[i] = &dtd;
    values[i] = &formats[i];
  }
  sdp_data_t *sflist;
  sflist = sdp_seq_alloc( dtds, values, sizeof(formats) );
  sdp_attr_add( &record, SDP_ATTR_SUPPORTED_FORMATS_LIST, sflist );
  
  char serv_name[128];
  sdp_svclass_uuid2strn( &svc_uuid, serv_name, sizeof(serv_name) );
  sdp_set_info_attr( &record, serv_name, 0, 0 );
  
  if ( sdp_record_register( sdp, &record, SDP_RECORD_PERSIST ) < 0 ) {
    sdp_close(sdp);
    return false;
  }
  
  mSdpHandle = record.handle;
  
  sdp_close( sdp );
  return true;
}

bool QObexBtTransport::unregisterSdp()
{
  myDebug(( "QObexBtTransport::unregisterSdp()" ));

  sdp_session_t *sdp = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_STANDARD_CONNECT_FLAGS );
  if ( !sdp )
    return false;

  uint32_t range = 0x0000ffff;
  sdp_list_t *attr = sdp_list_append( 0, &range );
  sdp_record_t *rec = sdp_service_attr_req( sdp, mSdpHandle, SDP_ATTR_REQ_RANGE, attr );
  sdp_list_free( attr, 0 );

  bool success = false;
  if ( rec )
    if ( 0 == sdp_record_unregister( sdp, rec ) )
      success = true;

  sdp_close(sdp);
  return success;
}

bool QObexBtTransport::discover()
{
  myDebug(( "QObexBtTransport::discover()" ));
  QValueList<bdaddr_t> devs = inquireDevices();

  for ( unsigned int idx = 0; idx < devs.size(); ++idx ) {
    int channel = getRfCommChannel( devs[idx], mUuid );
    if ( 0 < channel ) {
      mDest = devs[idx];
      mChannel = channel;
      return true;
    }
  }
  
  return false;
}
