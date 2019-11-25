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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <stdint.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#include <qstring.h>
#include <qcstring.h>
#include <qiodevice.h>
#include <qbuffer.h>

#include "qobexarray.h"

#include "qobextransport.h"

//#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

// #define DUMP_OBEX
// #define DUMP_TRANSPORT

#define DUMP_OBEX_FULL false

#ifdef DUMP_TRANSPORT
#include <ctype.h>
#include <qfile.h>
#include <qtextstream.h>
namespace {

  QFile QObexTransportDump( QString("/tmp/QObexTransport-dump-pid-%1.txt").arg(getpid()) );
  QTextStream stream( (QObexTransportDump.open( IO_WriteOnly ), &QObexTransportDump) );

  void dumpText( const char* buf, int len ) {
    stream << "<ascii>";
    for ( int i=0; i < len; ++i ) {
      Q_UINT8 c = buf[i];
      if ( isprint( c ) )
	stream << QChar( c );
      else if ( c == '\r' )
	stream << "<CR>";
      else if ( c == '\n' )
	stream << "<LF>";
      else
	stream << ".";
    }
    stream << "</ascii>\n";
  }

  void dumpHex( const char* buf, int len ) {
    stream << "<hex>";
    for ( int i=0; i < len; ++i )
      stream << QString().sprintf("<%02X>", (unsigned char)buf[i] );
    stream << "</hex>\n";
  }

  void dumpRead( const char* buf, int len ) {
    stream << "Read len = " << len << " bytes ("
	   << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")" << endl;
    if ( len < 0 ) {
      stream << "  error: " << ::strerror( errno ) << endl;
    } else if ( 0 < len ) {
      dumpText( buf, len );
      dumpHex( buf, len );
      stream.device()->flush();
    }
  }

  void dumpWrite( const char* buf, int len ) {
    stream << "Wrote len = " << len << " bytes ("
	   << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")" << endl;
    if ( len < 0 ) {
      stream << "  error: " << ::strerror( errno ) << endl;
    } else if ( 0 < len ) {
      dumpText( buf, len );
      dumpHex( buf, len );
      stream.device()->flush();
    }
  }
}
#endif

#ifdef DUMP_OBEX
#include <qfile.h>
#include <qtextstream.h>
#include <qdatetime.h>
namespace {

  QFile QObexDump( QString("/tmp/QObex-dump-pid-%1.txt").arg(getpid()) );
  QTextStream stream2( (QObexDump.open( IO_WriteOnly ), &QObexDump ) );

  void dumpRead( const QObexObject* obj ) {
    stream2 << "Read: (" << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")\n";
    stream2 << obj->toString( 0, DUMP_OBEX_FULL );
    stream2 << endl;
    stream2.device()->flush();
  }

  void dumpWrite( const QObexObject* obj ) {
    stream2 << "Write: (" << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")\n";
    stream2 << obj->toString( 0, DUMP_OBEX_FULL );
    stream2 << endl;
    stream2.device()->flush();
  }
}
#endif

QObexTransport::QObexTransport( QObject* parent, const char* name )
  : QObject( parent, name ) {
  myDebug(( "QObexTransport::QObexTransport( ... )" ));
  mReadTimeout = 0;
  mWriteTimeout = 0;
  mSelectHint = SelectRead; // FIXME
  mStatus = StatusOpen;
  slotReset();
}

QObexTransport::~QObexTransport() {
  myDebug(( "QObexTransport::~QObexTransport()" ));
}

bool QObexTransport::sendObject( const QObexObject* obj ) {
  myDebug(( "QObexTransport::sendObject( const QObexObject& )" ));
  if ( !isIdle() || !obj->isValid() )
    return false;

  myDebug(( "QObexTransport::sendObject() at %d, size = %u",
	    __LINE__, obj->size() ));
  
  mPendingWrites = obj->size();


  if ( obj->getPacketType() == QObexObject::ConnectPacket ) {
    QObexArray ch( 7 );
    ch.uint8( 0, obj->getRawCode() );
    ch.uint16( 1, obj->size() );
    ch.uint8( 3, obj->getVersion() );
    ch.uint8( 4, obj->getFlags() );
    ch.uint16( 5, obj->getMtu() );
    mWriteList.append( ch );
    
  } else if ( obj->getPacketType() == QObexObject::SetPathPacket ) {
    QObexArray ch( 5 );
    ch.uint8( 0, obj->getRawCode() );
    ch.uint16( 1, obj->size() );
    ch.uint8( 3, obj->getFlags() );
    ch.uint8( 4, obj->getConstants() );
    mWriteList.append( ch );
    
  } else {
    // conventional 3 byte header.
    QObexArray ch( 3 );
    ch.uint8( 0, obj->getRawCode() );
    ch.uint16( 1, obj->size() );
    mWriteList.append( ch );

  }
  
  QValueList<QObexHeader> mHeaders = obj->getHeaders();
  QValueList<QObexHeader>::Iterator it;
  for ( it = mHeaders.begin(); it != mHeaders.end(); ++it ) {
    mWriteList.append( (*it).coreHeader() );
    if ( (*it).hasArrayData() )
      mWriteList.append( (*it).arrayData() );
  }

  startCommand( mWriteTimeout );

  mStatus = StatusWriting;
  mWritten = 0;
  bool blk = blocking();
  do {
    readyWrite();
  } while ( blk && !mWriteList.isEmpty() );

#ifdef DUMP_OBEX
  dumpWrite( obj );
#endif
  
  // hook to dump packets ...
  emit signalWritingPacket( obj );
  
  return isConnected();
}

bool QObexTransport::listen( int ) {
  myDebug(( "QObexTransport::listen()" ));
  setStatus( StatusError );
  error( ListenError );
  return false;
}

QObexTransport* QObexTransport::accept() {
  myDebug(( "QObexTransport::accept()" ));
  setStatus( StatusError );
  error( AcceptError );
  return 0;
}

bool QObexTransport::blocking() const {
  myDebug(( "QObexTransport::blocking()" ));
  if ( socket() < 0 )
    return true;

  int flags = ::fcntl( socket(), F_GETFL, 0 );
  if ( flags < 0 )
    return true;

  return 0 == (flags & O_NDELAY);
}

void QObexTransport::setBlocking( bool enable ) {
  myDebug(( "QObexTransport::setBlocking( %d )", enable ));
  if ( socket() < 0 ) {
    myDebug(( "QObexTransport::setBlocking(): No socket"));
    setStatus( StatusError );
    error( UnknownError );
    return;
  }

  int flags = ::fcntl( socket(), F_GETFL, 0);
  if ( flags < 0 ) {
    myDebug(( "QObexTransport::setBlocking(): Error"));
    setStatus( StatusError );
    error( UnknownError );
    return;
  }

  if ( enable )
    flags &= ~O_NDELAY;
  else
    flags |= O_NDELAY;
    
  flags = ::fcntl( socket(), F_SETFL, flags );
  if ( flags < 0 ) {
    myDebug(( "QObexTransport::setBlocking(): Error" ));
    setStatus( StatusError );
    error( UnknownError );
  }
}

Q_LONG QObexTransport::readBlock( char *buf, Q_ULONG len ) {
  myDebug(( "QObexTransport::readBlock( buf, %ld )", len ));
  if ( !isConnected() )
    return -1;

  if ( len <= 0 )
    return 0;

  ssize_t rl;
  do {
    rl = ::read( socket(), buf, len );
    myDebug(( "QObexTransport::readBlock(): read( %d, ., %lu ) returns %d",
	      socket(), len, rl ));
  } while ( rl < 0 && errno == EINTR );
#ifdef DUMP_TRANSPORT
  dumpRead( buf, rl );
#endif
  if ( rl < 0 && errno != EAGAIN && errno != EWOULDBLOCK ) {
    myDebug(( "QObexTransport::readBlock( ... ): error: %s",
	      ::strerror( errno ) ));
    setStatus( StatusError );
    error( ReadError );
    return -1;
  } else if ( 0 == rl ) {
    myDebug(( "QObexTransport::readBlock( ... ): connection closed" ));
    connectionClosed();
    return 0;
  } else if ( 0 < rl )
    return rl;
  else
    return 0;
}

Q_LONG QObexTransport::writeBlock( const char *buf, Q_ULONG len ) {
  myDebug(( "QObexTransport::writeBlock( buf, %ld )", len ));
  if ( !isConnected() )
    return -1;

  if ( len <= 0 )
    return len;
  
  ssize_t wl;
  do {
    wl = ::write( socket(), buf, len );
    myDebug(( "QObexTransport::writeBlock(): write( %d, ., %lu ) returns %d",
	      socket(), len, wl ));
  } while ( wl < 0 && errno == EINTR );
#ifdef DUMP_TRANSPORT
  dumpWrite( buf, wl );
#endif

  if ( wl < 0 && ( errno == EAGAIN || errno == EWOULDBLOCK ) ) {
    // If we have not written the whole packet, select for writes.
    selectModeChanged( SelectWrite, mWriteTimeout );
  }

  if ( wl < 0 && errno != EAGAIN && errno != EWOULDBLOCK ) {
    myDebug(( "QObexTransport::writeBlock( ... ): error: %s",
	      ::strerror( errno ) ));
    setStatus( StatusError );
    error( WriteError );
    return -1;
  } else if ( 0 == wl ) {
    myDebug(( "QObexTransport::writeBlock( ... ): connection closed" ));
    connectionClosed();
    return 0;
  } else if ( 0 < wl ) {
    mPendingWrites -= wl;
    return wl;
  } else
    return 0;
}
  
int QObexTransport::doSelect() {
  myDebug(( "QObexTransport::doSelect()" ));
  if ( !isActive() ) {
    setStatus( StatusError );
    myDebug(( "QObexTransport::doSelect(): case !isActive()" )); 
    return -1;
  }
  
  if ( mSelectHint == SelectDisable ) {
    setStatus( StatusError );
    myDebug(( "QObexTransport::doSelect(): Select diasabled ..." )); 
    return -1;
  }
  
  myDebug(( "QObexTransport::doSelect(): try to select" ));
  // handle infinite timeouts means !mTime.isValid()
  struct timeval tv;
  struct timeval *tvp = 0;
  if ( mTimeout.isValid() ) {
    QDateTime now = QDateTime::currentDateTime();
    if ( mTimeout < now )
      return 0;
    
    tv.tv_sec = now.secsTo( mTimeout );
    int msecs = now.time().msecsTo( mTimeout.time() );
    if ( msecs < 0 )
      msecs += 1000*60*60*24;
    tv.tv_usec = 1000 * ( msecs % 1000 );
    
    myDebug(( "QObexTransport::doSelect(): waiting for at most %d.%03d",
	      int(tv.tv_sec), int(tv.tv_usec/1000) ));
    
    tvp = &tv;
  } else
    myDebug(( "QObexTransport::doSelect(): infinite select" ));
  
  fd_set fds;
  FD_ZERO( &fds );
  FD_SET( socket(), &fds );
  
  int ret;
  if ( mSelectHint == SelectRead )
    ret = ::select( socket()+1, &fds, 0, 0, tvp );
  else
    ret = ::select( socket()+1, 0, &fds, 0, tvp );
  myDebug(( "QObexTransport::doSelect(): select returns %d", ret )); 
  
  return ret;
}

void QObexTransport::bytesWritten( Q_LONG bytes ) {
  myDebug(( "QObexTransport::bytesWritten( %d )", (int)bytes )); 
  
  mPendingWrites -= bytes;
  if ( mPendingWrites <= 0 ) {
    mStatus = StatusConnected;
    emit signalPacketWritten();
    // Default is to read from the transport.
    selectModeChanged( SelectRead, mReadTimeout );
  }
}

void QObexTransport::error( Error error ) {
  myDebug(( "QObexTransport::error( ... )" ));
  mError = error;
  switch ( mError ) {
  case ConnectionRefused:
  case WriteError:
  case ReadError:
  case BindError:
  case ListenError:
  case AcceptError:
    mStatus = StatusError;
    break;
  default:
    break;
  }
  emit signalError( error );
}

void QObexTransport::connected() {
  myDebug(( "QObexTransport::connected( ... )" ));
#ifdef DUMP_OBEX
  stream2 << "Connected ("
	  << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")\n";
#endif
  resetReadState();
  resetWriteState();
  mStatus = StatusConnected;
  emit signalConnected();
}

void QObexTransport::connectionClosed() {
  myDebug(( "QObexTransport::connectionClosed()" ));
#ifdef DUMP_OBEX
  stream2 << "Connection closed ("
	  << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")\n" << endl;
#endif
  mStatus = StatusClosed;
  resetReadState();
  resetWriteState();
  // clear selection on socket ...
  selectModeChanged( SelectDisable, 0 );
  emit signalConnectionClosed();
}

void QObexTransport::selectModeChanged( QObexTransport::SelectHint hint, Q_LONG timeout ) {
  myDebug(( "QObexTransport::selectModeChanged( %d, %d )", int(hint), int(timeout) ));
  mSelectHint = hint;
  if ( timeout <= 0 ) {
    mTimeout = QDateTime();
  } else {
    // Calculate the time when the timeout triggers.
    mTimeout = QDateTime::currentDateTime();
    QTime time = mTimeout.time();
    time = time.addMSecs( timeout );
    if ( time < mTimeout.time() )
      mTimeout = mTimeout.addDays( 1 );
    mTimeout.setTime( time );
  }
  if ( mCommandDeadline.isValid()
       && ( mCommandDeadline < mTimeout || !mTimeout.isValid() ) ) {
    timeout = 1000*QDateTime::currentDateTime().secsTo( mCommandDeadline );
    timeout += QTime::currentTime().msecsTo( mCommandDeadline.time() )%1000;
    mTimeout == mCommandDeadline;
  }
  emit signalSelectModeChanged( hint, timeout );
}

void QObexTransport::slotIOReady()
{
  myDebug(( "QObexTransport::slotIOReady()" ));
  if ( mSelectHint == SelectRead )
    readyRead();
  else if ( mSelectHint == SelectWrite )
    readyWrite();
  else
    qWarning( "QObexTransport::slotIOReady() is called with SelectDisable" );
}

void QObexTransport::slotTimeout() {
  myDebug(( "QObexTransport::slotTimeou()" ));
  if ( mTimeout < mCommandDeadline || !mCommandDeadline.isValid() ) {
    timeout();
  } else {
    slotReset();
  }
}

void QObexTransport::readyRead()
{
  myDebug(( "QObexTransport::readyRead()" ));

  // We are really reading OBEX packets.
  if ( mPacketCode < 0 ) {
    myDebug(( "QObexTransport::readyRead(): try to read core header." ));

//     // This is implicitly used by readBlock( int ) to check 
//     mRemainingPacketSize = 3;
    QObexArray block = readBlock( 3 );
    if ( block.size() != 3 )
      return;
  
    mPacketCode = block.uint8( 0 );
    mRemainingPacketSize = block.uint16( 1 ) - 3;

    // Update the packet type if we don't already know that we will read a
    // special packet.
    if ( mPacketType == QObexObject::NormalPacket ) {
      if ( mPacketCode == ( QObexObject::Connect | QObexObject::FinalBit ) )
	mPacketType = QObexObject::ConnectPacket;
      if ( mPacketCode == ( QObexObject::SetPath | QObexObject::FinalBit ) )
	mPacketType = QObexObject::SetPathPacket;
    }
  }

  myDebug(( "QObexTransport::readyRead(): Header seems to be complete" ));
  if ( 0 < mRemainingPacketSize ) {
    myDebug(( "QObexTransport::readyRead(): "
	      "try to read additional core header." ));
    switch ( mPacketType ) {
    case QObexObject::ConnectPacket:
      myDebug(( "QObexTransport::readyRead(): ConnectPacket" ));
      if ( mPacketHeaderData.size() != 4 ) {
	if ( mRemainingPacketSize < 4 ) {
	  mRemainingPacketSize = -1;
	  break;
	}
	mPacketHeaderData = readBlock( 4 );
	if ( mPacketHeaderData.size() != 4 )
	  return;
	mRemainingPacketSize -= 4;
      }
      break;
    case QObexObject::SetPathPacket:
      myDebug(( "QObexTransport::readyRead(): SetPathPacket" ));
      if ( mPacketHeaderData.size() != 2 ) {
	if ( mRemainingPacketSize < 2 ) {
	  mRemainingPacketSize = -1;
	  break;
	}
	mPacketHeaderData = readBlock( 2 );
	if ( mPacketHeaderData.size() != 2 )
	  return;
	mRemainingPacketSize -= 2;
      }
      break;
    default:
      myDebug(( "QObexTransport::readyRead(): NormalPacket" ));
      break;
    }
  }

  // Read headers.
  while ( 0 < mRemainingPacketSize ) {
    // if we have not already read the header code read it now.
    if ( mHeaderCode < 0 ) {
      myDebug(( "QObexTransport::readyRead(): Try to read header id" ));
      QObexArray buf = readBlock( 1 );
      if ( buf.size() != 1 )
	return;
      mRemainingPacketSize -= 1;
      mHeaderCode = buf.uint8( 0 );
    }
    // Ok, we have now then header code.
    Q_LONG type = QObexHeader::dataType( mHeaderCode );
    if ( type == QObexHeader::Byte ) {
      myDebug(( "QObexTransport::readyRead(): Try to read byte header value" ));
      // In case of a byte header
      if ( mRemainingPacketSize < 1 ) {
	mRemainingPacketSize = -1;
	break;
      }
      QObexArray buf = readBlock( 1 );
      if ( buf.size() != 1 )
	return;
      mRemainingPacketSize -= 1;
      // Ok got it, append header ...
      mHeaderList.append( QObexHeader( mHeaderCode, buf ) );
      // ... and reset header parsing state
      mHeaderCode = -1;
    } else if ( type == QObexHeader::DWord ) {
      myDebug(( "QObexTransport::readyRead(): Try to read dword header value" ));
      // In case of a 4byte header
      if ( mRemainingPacketSize < 4 ) {
	mRemainingPacketSize = -1;
	break;
      }
      QObexArray buf = readBlock( 4 );
      if ( buf.size() != 4 )
	return;
      mRemainingPacketSize -= 4;
      // Ok got it, append header ...
      mHeaderList.append( QObexHeader( mHeaderCode, buf ) );
      // ... and reset header parsing state
      mHeaderCode = -1;
    } else {
      // In case of a length encoded header
      if ( mHeaderSize < 0 ) {
	myDebug(( "QObexTransport::readyRead(): Try to read header length" ));
	
	// read length if not already read.
	if ( mRemainingPacketSize < 2 ) {
	  mRemainingPacketSize = -1;
	  break;
	}
	QObexArray buf = readBlock( 2 );
	if ( buf.size() != 2 )
	  return;
	mRemainingPacketSize -= 2;
	mHeaderSize = buf.uint16( 0 ) - 3;
      }
      myDebug(( "QObexTransport::readyRead(): Try to read header value" ));
      if ( 0 == mHeaderSize ) {
	mHeaderList.append( QObexHeader( mHeaderCode ) );
      } else {
	// Read header data if not already read.
	if ( mRemainingPacketSize < mHeaderSize ) {
	  mRemainingPacketSize = -1;
	  break;
	}
	QObexArray buf = readBlock( mHeaderSize );
	if ( Q_LONG( buf.size() ) != mHeaderSize )
	  return;
	mRemainingPacketSize -= mHeaderSize;
	// Ok got it, append header ...
	mHeaderList.append( QObexHeader( mHeaderCode, buf ) );
      }
      // ... and reset header parsing state
      mHeaderCode = -1;
      mHeaderSize = -1;
    }
  }

  if ( mRemainingPacketSize < 0 ) {
    myDebug(( "QObexTransport::readyRead(): parse error" ));

    // FIXME this is a parse error.
    slotReset();
    error( ParseError );
    setStatus( StatusConnected );
    return;
  }
  if ( mRemainingPacketSize == 0 ) {
    // We have read a complete packet ...
    myDebug(( "QObexTransport::readyRead(): packet ready" ));

    QObexObject obj;
    /// FIXME Use this for parsing ...
    obj.setValid( true );
    obj.setPacketType( mPacketType );
    obj.setRawCode( mPacketCode );
    switch ( mPacketType ) {
    case QObexObject::ConnectPacket:
      obj.setVersion( mPacketHeaderData.uint8( 0 ) );
      obj.setFlags( mPacketHeaderData.uint8( 1 ) );
      obj.setMtu( mPacketHeaderData.uint16( 2 ) );
      break;
    case QObexObject::SetPathPacket:
      obj.setFlags( mPacketHeaderData.uint8( 0 ) );
      obj.setConstants( mPacketHeaderData.uint8( 1 ) );
      break;
    default:
      break;
    }

    QValueList<QObexHeader>::Iterator it;
    for ( it = mHeaderList.begin(); it != mHeaderList.end(); ++it )
      obj.appendHeader( *it );

    resetReadState();
    setStatus( StatusConnected );
#ifdef DUMP_OBEX
    dumpRead( &obj );
#endif
    emit signalPacketRecieved( obj );
  }
}

void QObexTransport::readyWrite()
{
  myDebug(( "QObexTransport::readyWrite()" ));

  Q_ULONG mtu = maximumTransferUnit();

  // cycle here as long as we can write something.
  QValueList<QByteArray>::Iterator it = mWriteList.begin();
  while ( it != mWriteList.end() ) {
    // Can happen with empty body headers etc ...
    if ( (*it).size() == 0 ) {
      it = mWriteList.remove( it );
      continue;
    }
    Q_ASSERT( mWritten < (*it).size() );

    if ( (*it).size() < mtu ) {
      QBuffer tmp( *it );
      tmp.open( IO_WriteOnly );
      tmp.at( tmp.size() );
      QValueList<QByteArray>::Iterator it2 = it;
      ++it2;
      while ( it2 != mWriteList.end() ) {
	if ( tmp.size() + (*it2).size() <= mtu + mWritten ) {
	  tmp.writeBlock( *it2 );
	  it2 = mWriteList.remove( it2 );
	} else
	  break;
      }
      tmp.close();

      Q_LONG written;
      written = writeBlock( (*it).data() + mWritten, (*it).size() - mWritten );
      myDebug(( "QObexTransport::readyWrite(): written = %ld", written ));

      // We can not write anymore or an error occurred;
      if ( written <= 0 )
	break;
      
      mWritten += written;
      // The whole block was written.
      if ( mWritten == (*it).size() ) {
	it = mWriteList.remove( it );
	mWritten = 0;
      }
    } else {
      Q_LONG written;
      written = writeBlock( (*it).data() + mWritten, (*it).size() - mWritten );
      myDebug(( "QObexTransport::readyWrite(): %ld", written ));

      // We can not write anymore or an error occurred;
      if ( written <= 0 )
	break;
      
      mWritten += written;
      // The whole block was written.
      if ( mWritten == (*it).size() ) {
	it = mWriteList.remove( it );
	mWritten = 0;
      }
    }
  }
  
  if ( mPendingWrites <= 0 ) {
    mStatus = StatusConnected;
    emit signalPacketWritten();
    // Default is to read from the transport.
    selectModeChanged( SelectRead, mReadTimeout );
  }
}

void QObexTransport::slotReset()
{
  myDebug(( "QObexTransport::slotReset()" ));
  
  resetWriteState();
  resetReadState();

  // call back into the implementation.
  reset();

  switch ( mStatus ) {
  case StatusReading:
  case StatusWriting:
    mStatus = StatusConnected;
    break;
  default:
    mStatus = StatusOpen;
    break;
  }
  emit signalError( ConnectionReset );
}

void QObexTransport::slotReadyAccept( int fd ) {
  myDebug(( "QObexTransport::slotReadyAccept( %d )", fd ));
  if ( fd == socket() ) {
    // We have a listening transport which got an established connection.
    emit signalConnectionAccept( this );
  }
}

QObexArray QObexTransport::readBlock( Q_ULONG bytes ) {
  myDebug(( "QObexTransport::readBlock( %lu )", bytes ));

  if ( bytes <= 0 )
    return QObexArray();

  Q_ULONG size = mAtomicReadBuffer.size();
  mAtomicReadBuffer.resize( bytes, QGArray::SpeedOptim );
  bool blk = blocking();
  do {
    Q_LONG read = readBlock( mAtomicReadBuffer.data()+size, bytes-size );
    if ( 0 <= read ) {
      size += read;
    } else {
      size = 0;
      mStatus = StatusClosed;
      connectionClosed();
      break;
    }
  } while ( blk && size < bytes && isConnected() );
  mAtomicReadBuffer.resize( size, QGArray::SpeedOptim );

  if ( size == bytes ) {
    QByteArray ret = mAtomicReadBuffer;
    mAtomicReadBuffer.duplicate( 0, 0 );
//     mRemainingPacketSize -= bytes;
    return ret;
  }

  if ( bytes < mAtomicReadBuffer.size() ) {
    // This case should not happen. But to be complete ...
    qWarning( "########################This codepath should not trigger ..." );
    QByteArray ret = mAtomicReadBuffer;
    mAtomicReadBuffer = QByteArray( ret.size()-bytes );
    ::memcpy( mAtomicReadBuffer.data(), ret.data()+bytes, ret.size()-bytes );
    ret.resize( bytes, QGArray::SpeedOptim );
    return ret;
  }
  return QByteArray();
}

void QObexTransport::resetWriteState() {
  mWriteList.clear();
  mWritten = 0;
}

void QObexTransport::resetReadState() {
  mAtomicReadBuffer.resize( 0, QGArray::SpeedOptim ); 
  mPacketHeaderData.resize( 0, QGArray::SpeedOptim );
  mPacketCode = -1;
  mRemainingPacketSize = -1;
  mPacketType = QObexObject::NormalPacket;
  
  mHeaderList.clear();
  mHeaderCode = -1;
  mHeaderSize = -1;
}

#include "qobextransport.moc"
