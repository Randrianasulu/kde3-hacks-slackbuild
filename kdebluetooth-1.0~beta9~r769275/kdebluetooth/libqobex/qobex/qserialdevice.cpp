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

#include <termios.h>
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
#include <ctype.h>

#include <qstring.h>

#include "qttylock.h"
#include "qserialdevice.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

// #define DUMP_SERIAL
#ifdef DUMP_SERIAL
#include <ctype.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>
namespace {

  QFile serialDump( "/tmp/serial-dump.txt" );
  QTextStream stream( (serialDump.open( IO_WriteOnly ), &serialDump) );

  void
  dumpText( const char* buf, int len ) {
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
  void
  dumpHex( const char* buf, int len ) {
    stream << "<hex>";
    for ( int i=0; i < len; ++i )
      stream << QString().sprintf("<%02X>", (unsigned char)buf[i] );
    stream << "</hex>\n";
  }

  void
  dumpRead( const char* buf, int len ) {
    stream << "Read len = " << len << " bytes ("
	   << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")" << endl;
    if ( 0 < len ) {
      dumpText( buf, len );
      dumpHex( buf, len );
      stream.device()->flush();
    }
  }
  void
  dumpWrite( const char* buf, int len ) {
    stream << "Wrote len = " << len << " bytes ("
	   << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << ")" << endl;
    if ( 0 < len ) {
      dumpText( buf, len );
      dumpHex( buf, len );
      stream.device()->flush();
    }
  }
}
#endif

QSerialDevice::QSerialDevice() {
  mParity = DefaultParity;
  mDataBits = DefaultDataBits;
  mStopBits = DefaultStopBits;
  mSpeed = DefaultSpeed;
  mHWFlowControl = DefaultHWFlowControl;
  mSWFlowControl = DefaultSWFlowControl;

  mDoLocking = true;
  fd = -1;
//   setState( 0 );
}

QSerialDevice::QSerialDevice( const QString& device, bool doLocking ) {
  mLock.setDevice( device );

  mParity = DefaultParity;
  mDataBits = DefaultDataBits;
  mStopBits = DefaultStopBits;
  mSpeed = DefaultSpeed;
  mHWFlowControl = DefaultHWFlowControl;
  mSWFlowControl = DefaultSWFlowControl;

  mDoLocking = doLocking;
  fd = -1;
//   setState( 0 );
}

QSerialDevice::~QSerialDevice() {
  myDebug(("QSerialDevice::~QSerialDevice()"));
  close();
}

bool
QSerialDevice::isOpen() const {
  return fd != -1;
}

bool
QSerialDevice::open( int mode ) {
  myDebug(("QSerialDevice::open()"));
  // we can not handle special modes
  if ( mode & ( IO_Append | IO_Translate | IO_Truncate ) ) {
    setStatus( IO_OpenError );
    setState( 0 );
    return false;
  } else {
    if ( mDoLocking ) {
      if ( !mLock.lock() ) {
	setStatus( IO_OpenError );
	setState( 0 );
	return false;
      }
    }

    int flgs = O_NOCTTY | O_NDELAY;
    if ( (mode & IO_ReadWrite) == IO_ReadOnly )
      flgs |= O_RDONLY;
    if ( (mode & IO_ReadWrite) == IO_WriteOnly )
      flgs |= O_WRONLY;
    if ( (mode & IO_ReadWrite) == IO_ReadWrite )
      flgs |= O_RDWR;
    fd = ::open( mLock.getDevice().latin1(), flgs, 0);
    if ( fd <= 0 ) {
      setStatus( IO_OpenError );
      mLock.unlock();
      fd = -1;
      return false;
    }
    myDebug(( "opened %s: fd = %d", mLock.getDevice().latin1(), fd ));

    // Get the usual terminal settings
    tcgetattr( fd, &oldTermOpts );
    struct termios options = oldTermOpts;

    // make a raw serial port
    cfmakeraw( &options );
    options.c_cflag |= CLOCAL | CREAD;

    setSpeed( options );
    setDataBits( options );
    setStopBits( options );
    setParity( options );
    setHWFlowControl( options );
    setSWFlowControl( options );

    tcsetattr( fd, TCSANOW, &options );

    setBlocking( true );

    setType( IO_Sequential );
    setMode( IO_Async | mode );
    setStatus( IO_Ok );
    setState( IO_Open );

    return true;
  }
}

void
QSerialDevice::close() {
  myDebug(("QSerialDevice::close()"));
  if ( isOpen() ) {
    // clean up
    mReadBuffer.clear();

    // Restore old serial terminal settings ...
    tcflush( fd, TCIOFLUSH );
    tcsetattr( fd, TCSANOW, &oldTermOpts );

    ::close( fd );
    fd = -1;

    mLock.unlock();
    setState( 0 );
  }
}

void
QSerialDevice::flush() {
  myDebug(("QSerialDevice::flush()"));
  // Writes are not buffered
}

Q_ULONG
QSerialDevice::size() const {
  myDebug(("QSerialDevice::size()"));
  return mReadBuffer.size();
}

Q_LONG
QSerialDevice::readBlock( char *data, Q_ULONG maxlen ) {
  myDebug(("QSerialDevice::readBlock()"));
  if ( !isOpen() )
    return -1;

  // can convert it here to unsigned since read buffer always returns >= 0
  Q_ULONG read = mReadBuffer.get( data, maxlen );
  Q_ASSERT( read <= maxlen );
  if ( read < maxlen && 0 < unreadBytes() ) {
    Q_LONG ret = reallyRead( data + read, maxlen - read );
    if ( 0 < ret )
      read += ret;
  }

  return read;
}

Q_LONG
QSerialDevice::writeBlock( const char *data, Q_ULONG len ) {
  myDebug(("QSerialDevice::writeBlock()"));
  if ( !isOpen() )
    return -1;

  if ( len <= 0 )
    return len;

  ssize_t wl;
  do {
    wl = ::write( fd, data, len );
  } while ( wl < 0 && errno == EINTR );
#ifdef DUMP_SERIAL
  dumpWrite( data, wl );
#endif
  if ( wl < 0 && errno != EAGAIN ) {
    setStatus( IO_WriteError );
    return -1;
  } else if ( 0 < wl )
    return wl;
  else
    return 0;
}

int
QSerialDevice::getch() {
  myDebug(("QSerialDevice::getch()"));
  char buf;
  if ( 0 >= readBlock( &buf, 1 ) )
    return -1;
  return (unsigned char)buf;
}

int
QSerialDevice::putch( int ch ) {
  myDebug(("QSerialDevice::putch()"));
  if ( !isOpen() )
    return -1;

  char buf = ch;
  Q_LONG wr = writeBlock( &buf, sizeof( buf ) );
  if ( wr != 1 ) {
    setStatus( IO_WriteError );
    return -1;
  } else
    return ch;
}

int
QSerialDevice::ungetch( int c ) {
  myDebug(("QSerialDevice::ungetch()"));
  return mReadBuffer.ungetch( c );
}

void
QSerialDevice::setDevice( const QString& device ) {
  myDebug(("QSerialDevice::setDevice()"));
  if ( device != mLock.getDevice() ) {
    if ( isOpen() ) {
      close();
      mLock.setDevice( device );
      open( mode() );
    } else
      mLock.setDevice( device );
  }
}

QString
QSerialDevice::device() const {
  myDebug(( "QSerialDevice::device()" ));
  return mLock.getDevice();
}

void
QSerialDevice::setSpeed( int speed ) { 
  myDebug(("QSerialDevice::setSpeed()"));
  if ( mSpeed != speed ) {
    mSpeed = speed;
    if ( isOpen() ) {
      struct termios options;
      tcgetattr(fd, &options);
      setSpeed( options );
      tcsetattr(fd, TCSANOW, &options);
    }
  }
}

int
QSerialDevice::speed() const {
  myDebug(( "QSerialDevice::speed()" ));
  return mSpeed;
}

void
QSerialDevice::setParity( int par ) {
  myDebug(( "QSerialDevice::setParity()" ));
  if ( par == NoParity
       || par == EvenParity
       || par == OddParity
       || par == MarkParity
       || par == SpaceParity ) {
    if ( par != mParity ) {
      mParity = par;
      if ( isOpen() ) {
	struct termios options;
	tcgetattr(fd, &options);
	setParity( options );
	tcsetattr(fd, TCSANOW, &options);
      }
    }
  } else
    qWarning( "Unknown Parity value %c, leaving unchanged", par);
}

int
QSerialDevice::parity() const {
  myDebug(( "QSerialDevice::parity()" ));
  return mParity;
}

void
QSerialDevice::dropDTR() {
  myDebug(("QSerialDevice::dropDTR()"));
  if ( isOpen() ) {
    struct termios options;
    tcgetattr(fd, &options);
    
    // Set speed to 0
    cfsetispeed(&options, B0);
    cfsetospeed(&options, B0);
    tcsetattr(fd, TCSANOW, &options);
  }
}

void
QSerialDevice::raiseDTR() {
  myDebug(("QSerialDevice::raiseDTR()"));
  if ( isOpen() ) {
    struct termios options;
    tcgetattr(fd, &options);
    
    // Set speed to speed
    setSpeed( options );
    tcsetattr(fd, TCSANOW, &options);
  }
}

bool QSerialDevice::blocking () const {
  myDebug(( "QSerialDevice::blocking()" ));
  if ( !isOpen() )
    return true;
  int s = ::fcntl( fd, F_GETFL, 0 );
  return !(s >= 0 && ((s & O_NDELAY) != 0));
}

void QSerialDevice::setBlocking( bool enable ) {
  myDebug(( "QSerialDevice::setBlocking()" ));
  if ( isOpen() ) {
    int tmp = ::fcntl( fd, F_GETFL, 0);
    if ( tmp >= 0 )
      tmp = ::fcntl( fd, F_SETFL, enable ? (tmp&~O_NDELAY) : (tmp|O_NDELAY) );
    if ( tmp < 0 )
      setStatus( IO_ResourceError );
  }
}

bool QSerialDevice::sendModemCommand( const char* cmd ) {
  myDebug(( "QSerialDevice::sendModemCommand()" ));
  Q_LONG len = ::strlen( cmd );
  return len == writeBlock( cmd, len );
}

QString QSerialDevice::readLine() {
  myDebug(( "QSerialDevice::readLine()" ));
  QString line;
  int c;
  while ( 0 <= ( c = getch() ) ) {
    if ( c == '\n' )
      break;
    if ( c == '\r' )
      continue;
    line += QChar( c );
  }
  return line;
}

bool QSerialDevice::lineAvailable() {
  myDebug(( "QSerialDevice::lineAvaliable()" ));
  if ( mReadBuffer.contains( '\n' ) )
    return true;

  while ( 0 < unreadBytes() ) {
    char c;
    Q_LONG ret = reallyRead( &c, sizeof(c) );
    if ( ret <= 0 )
      return false;

    mReadBuffer.put( &c, 1 );
    
    if ( c == '\n' )
      return true;
  }

  return false;
}

int QSerialDevice::waitForMore( int msecs ) const {
  myDebug(( "QSerialDevice::waitForMore( %d )", msecs ));
  if ( !isOpen() )
    return -1;
  
  fd_set fds;
  struct timeval tv;
  
  FD_ZERO( &fds );
  FD_SET( fd, &fds );
  
  tv.tv_sec = msecs / 1000;
  tv.tv_usec = (msecs % 1000) * 1000;
  
  return ::select( fd+1, &fds, 0, 0, msecs < 0 ? 0 : &tv );
}

// Discard any buffered input or output. do the same on the serial line.
void QSerialDevice::clear() {
  myDebug(( "QSerialDevice::clear()" ));
  mReadBuffer.clear();
  tcflush( fd, TCIOFLUSH );
}

Q_LONG QSerialDevice::unreadBytes() const {
  myDebug(( "QSerialDevice::unreadBytes()" ));
  size_t nbytes = 0;
  // gives shorter than true amounts on Unix domain sockets.
  if ( ::ioctl( fd, FIONREAD, (char*)&nbytes) < 0 )
    return -1;

  return (Q_LONG) *((int *) &nbytes);
}

Q_LONG QSerialDevice::reallyRead( char* buf, Q_ULONG maxlen ) {
  Q_LONG ret;
  do {
    ret = ::read( fd, buf, maxlen );
  } while ( ret < 0 && errno == EINTR );
#ifdef DUMP_SERIAL
  dumpRead( buf, ret );
#endif
    
  myDebug(( "QSerialDevice::reallyRead() read %d bytes", (int)ret ));

  if ( ret < 0 && errno != EAGAIN ) {
    myDebug(( "QSerialDevice::reallyRead() Error" ));
    setStatus( IO_ReadError );
    return -1;
  }
  
  // EAGAIN is nonfatal ...
  if ( ret < 0 )
    return 0;

  return ret;
}

void
QSerialDevice::setDataBits( struct termios& options ) {
  options.c_cflag &= ~CSIZE;
  switch ( mDataBits ) {
  case 5:
    options.c_cflag |= CS5;
    break;
  case 6:
    options.c_cflag |= CS6;
    break;
  case 7:
    options.c_cflag |= CS7;
    break;
  default:
    options.c_cflag |= CS8;
    break;
  }
}

void
QSerialDevice::setStopBits( struct termios& options ) {
  if ( mStopBits == 2 )
    options.c_cflag |= CSTOPB;
  else
    options.c_cflag &= ~CSTOPB;
}

void
QSerialDevice::setParity( struct termios& options ) {
  options.c_cflag &= ~(PARENB | PARODD);
  if ( mParity == EvenParity )
    options.c_cflag |= PARENB;
  else if ( mParity == OddParity )
    options.c_cflag |= (PARENB | PARODD);
}

void
QSerialDevice::setHWFlowControl( struct termios& options ) {
#ifdef CRTSCTS
  if ( mHWFlowControl )
    options.c_cflag |= CRTSCTS;
  else
    options.c_cflag &= ~CRTSCTS;
#else
#ifdef CNEW_RTSCTS
  if ( mHWFlowControl )
    options.c_cflag |= CNEW_RTSCTS;
  else
    options.c_cflag &= ~CNEW_RTSCTS;
#endif
#endif
}

void
QSerialDevice::setSWFlowControl( struct termios& options ) {
  if ( mSWFlowControl )
    options.c_iflag |= (IXON | IXOFF | IXANY);
  else
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
}

void
QSerialDevice::setSpeed( struct termios& options ) {
  speed_t tioSpeed;
  if ( mSpeed <= 9600 )
    mSpeed = 9600, tioSpeed = B9600;
  else if ( mSpeed <= 19200 )
    mSpeed = 19200, tioSpeed = B19200;
  else if ( mSpeed <= 38400 )
    mSpeed = 38400, tioSpeed = B38400;
  else if ( mSpeed <= 57600 )
    mSpeed = 57600, tioSpeed = B57600;
  else
    mSpeed = 115200, tioSpeed = B115200;

  cfsetispeed( &options, tioSpeed );
  cfsetospeed( &options, tioSpeed );
}
