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

#include <qobject.h>
#include <qcstring.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <termios.h>

#include "qserialdevice.h"

#include "qobexserialtransport.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexSerialTransport::QObexSerialTransport( QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexSerialTransport::QObexSerialTransport( ... )" ));
  
  setStatus( StatusOpen );
  setBlocking( true );
  
  setSpeed();
  setDevice( "/dev/ttyS0" );
}

QObexSerialTransport::~QObexSerialTransport() {
  myDebug(( "QObexSerialTransport::~QObexSerialTransport()" ));
  if ( mSerial.isOpen() )
    mSerial.close();
}

void QObexSerialTransport::disconnect() {
  myDebug(( "QObexSerialTransport::disconnect()" ));

  mSerial.close();
  // Reset to default speed
  mSerial.setSpeed();
  connectionClosed();
}

bool QObexSerialTransport::connect() {
  myDebug(( "QObexSerialTransport::connect( ... )" ));
  if ( !mSerial.isOpen() ) {
    // Reset to default speed
    mSerial.setSpeed();
    mSerial.open( IO_ReadWrite );
  }
  if ( mSerial.isOpen() ) {
    setStatus( StatusConnected );
    connected();
    return true;
  } else {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }
}

bool QObexSerialTransport::isConnected() const {
  myDebug(( "QObexSerialTransport::isConnected()" ));
  return mSerial.isOpen();
}

Q_LONG QObexSerialTransport::readBlock( char *buf, Q_ULONG len ) {
  myDebug(( "QObexSerialTransport::readBlock( ... )" ));
  if ( isConnected() ) {
    return mSerial.readBlock( buf, len );
  } else
    return -1;
}

Q_LONG QObexSerialTransport::writeBlock( const char *buf, Q_ULONG len ) {
  myDebug(( "QObexSerialTransport::writeBlock( ... )" ));
  if ( isConnected() ) {
    return mSerial.writeBlock( buf, len );
  } else
    return -1;
}

Q_LONG QObexSerialTransport::maximumTransferUnit() const {
  myDebug(( "QObexSerialTransport::maximumTransferUnit()" ));
  return 1;
}

Q_LONG QObexSerialTransport::bytesPerSecond() const {
  return speed()/10;
}

// Sets the speed
void QObexSerialTransport::setSpeed( int speed ) {
  myDebug(( "QObexSerialTransport::setSpeed()" ));
  mSerial.setSpeed( speed );
}

int QObexSerialTransport::speed() const {
  myDebug(( "QObexSerialTransport::speed()" ));
  return mSerial.speed();
}

// Sets the device
void QObexSerialTransport::setDevice( const QString& dev ) {
  myDebug(( "QObexSerialTransport::setDevice()" ));
  mSerial.setDevice( dev );
}

QString QObexSerialTransport::device() const {
  myDebug(( "QObexSerialTransport::device()" ));
  return mSerial.device();
}

bool QObexSerialTransport::blocking() const {
  myDebug(( "QObexSerialTransport::blocking()" ));
  return mSerial.blocking();
}

void QObexSerialTransport::setBlocking( bool enable ) {
  myDebug(( "QObexSerialTransport::setDevice()" ));
  mSerial.setBlocking( enable );
}
