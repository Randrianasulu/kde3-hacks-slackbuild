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

#include "qobexericssontransport.h"

#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexEricssonTransport::QObexEricssonTransport( QObject* parent, const char *name )
: QObexTransport( parent, name ) {
  myDebug(( "QObexEricssonTransport::QObexEricssonTransport( ... )" ));

  setStatus( StatusOpen );
  mBlocking = true;
  setDevice( "/dev/ttyS0" );
  setSpeed();
  mState = NotConnected;
  mATZCount = 0;
}

QObexEricssonTransport::~QObexEricssonTransport() {
  myDebug(( "QObexEricssonTransport::~QObexEricssonTransport()" ));
  mSerial.close();
}

void QObexEricssonTransport::reset() {
  myDebug(( "QObexEricssonTransport::reset()" ));
  mSerial.close();

  setStatus( StatusOpen );
  mState = NotConnected;
  mATZCount = 0;
}

void QObexEricssonTransport::disconnect() {
  myDebug(( "QObexEricssonTransport::disconnect()" ));

  mSerial.close();
  connectionClosed();
}

bool QObexEricssonTransport::connect() {
  myDebug(( "QObexEricssonTransport::connect( ... )" ));
  if ( !mSerial.isOpen() ) {
    // Reset to default speed
    mSerial.setSpeed();
    mSerial.open( IO_ReadWrite );
    mSerial.setBlocking( mBlocking );
  }
  if ( !mSerial.isOpen() ) {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }

  if ( mState == NotConnected ) {
    // We expect a modem here. If we have found it we switch to a special
    // binary data transfer mode.
    mSerial.sendModemCommand( "ATZ\r" );
    mATZCount = 10;
    mState = ATZSent;
    selectModeChanged( SelectRead, 200 );
  }
  return true;
}

Q_LONG QObexEricssonTransport::maximumTransferUnit() const {
  myDebug(( "QObexEricssonTransport::maximumTransferUnit()" ));
  return 1;
}

Q_LONG QObexEricssonTransport::bytesPerSecond() const {
  return speed()/10;
}

// Sets the speed
void QObexEricssonTransport::setSpeed( int speed ) {
  myDebug(( "QObexEricssonTransport::setSpeed()" ));
  mSerial.setSpeed( speed );
}

int QObexEricssonTransport::speed() const {
//   myDebug(( "QObexEricssonTransport::speed()" ));
  return mSerial.speed();
}

// Sets the speed
void QObexEricssonTransport::setDevice( const QString& dev ) {
  myDebug(( "QObexEricssonTransport::setDevice()" ));
  mSerial.setDevice( dev );
}

QString QObexEricssonTransport::device() const {
//   myDebug(( "QObexEricssonTransport::device()" ));
  return mSerial.device();
}

void QObexEricssonTransport::setBlocking ( bool enable ) { 
  myDebug(( "QObexEricssonTransport::setBlocking( %d )", enable ));
  if ( mSerial.isOpen() )
    mSerial.setBlocking( enable );
  mBlocking = enable;
}

Q_LONG QObexEricssonTransport::readBlock( char *buf, Q_ULONG len ) {
  myDebug(( "QObexEricssonTransport::readBlock( ... )" ));
  if ( !isConnected() )
    return -1;

  return mSerial.readBlock( buf, len );
}

Q_LONG QObexEricssonTransport::writeBlock( const char *buf, Q_ULONG len ) {
  myDebug(( "QObexEricssonTransport::writeBlock( ... )" ));
  if ( !isConnected() )
    return -1;

  return mSerial.writeBlock( buf, len );
}

void QObexEricssonTransport::slotIOReady() {
  myDebug(( "QObexEricssonTransport::slotIOReady()" ));
  switch ( mState ) {
  case NotConnected:
    // Usually at the beginning and after closing the connection.
    // If we have unrecoverable timeouts we end here also.
    // At this stage we can connect.
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "ups, we get data in state NotConnected!" ));
    mSerial.clear();
    break;

  case Connected:
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "handle data while connected ..." ));

    // The connection is up. At this stage we are a normal socket oriented 
    // transport.
    QObexTransport::slotIOReady();
//     readyRead();
    break;

  case ATZSent:
    // We try to connect, first send a ATZ and check if there is a modem
    // attached to the serial line.
    myDebug(( "QObexEricssonTransport::slotIOReady(): ATZSent" ));
    while ( mSerial.lineAvailable() ) {
      QString answer = mSerial.readLine();
      myDebug(( "QObexEricssonTransport::slotIOReady(): Answer is %s",
		answer.ascii() ));
      if ( answer == "ATZ" || answer.isEmpty() ) {
	// Ok, wait for the next line
      } else if ( answer == "OK" ) {
	myDebug(( "QObexEricssonTransport::slotIOReady(): Sending AT+CGMI" ));
	mSerial.sendModemCommand( "AT+CGMI\r" );
	mState = CGMISent;
	selectModeChanged( SelectRead, 300 );
      } else {
	mState = NotConnected;
	setStatus( StatusError );
	error( ConnectionRefused );
	break;
      }
    }
    break;

  case CGMISent:
    // We know now that we have a modem and check if it is built by Siemens.
    while ( mSerial.lineAvailable() ) {
      QString answer = mSerial.readLine();
      myDebug(( "QObexEricssonTransport::slotIOReady(): Answer is %s",
		answer.ascii() ));
      if ( answer == "ERICSSON" || answer == "AT+CGMI" || answer.isEmpty() ) {
	// Ok, wait for the next line
      } else if ( answer == "OK" ) {
	myDebug(( "QObexEricssonTransport::slotIOReady(): Sending AT*EOBEX" ));
	mSerial.sendModemCommand( "AT*EOBEX\r" );
	mState = ATEOBEXSent;
	selectModeChanged( SelectRead, 300 );
      } else {
	mState = NotConnected;
	setStatus( StatusError );
	error( ConnectionRefused );
	break;
      }
    }
    break;

  case ATEOBEXSent:
    // We try to switch to ERICSSON mode.
    while ( mSerial.lineAvailable() ) {
      QString answer = mSerial.readLine();
      myDebug(( "QObexEricssonTransport::slotIOReady(): Answer is %s",
		answer.ascii() ));
      if ( answer == "CONNECT" ) {
	myDebug(( "QObexEricssonTransport::slotIOReady(): Ok we are in OBEX mode" ));
	setStatus( StatusConnected );
	mState = Connected;
	connected();
      } else if ( answer == "ERROR" ) {
	mState = NotConnected;
	setStatus( StatusError );
	error( ConnectionRefused );
	break;
      }
    }
    break;

  case DTRDropped:
    // The modem is not responding, so try to initizlise by the hard way.
    // Algorythm is copied from minicoms "Initialize Modem".
    // And it will wake up every modem :-)
    // First stage is that the DTR line is dropped for a second.
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "ups, we get data in state DTRDropped!" ));
    mSerial.clear();
    break;

  case DTRRaised:
    // Second stage is that the DTR line is raised again. Then wait for an
    // additional second.
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "ups, we get data in state DTRRaised!" ));
    mSerial.clear();
    break;

  case CRSent:
    // Third stage is to send a \r and wait an additional second.
    // Past this command ew check again with ATZ and try to initialize
    // the modem.
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "ups, we get data in state CRSent!" ));
    mSerial.clear();
    break;

  default:
    myDebug(( "QObexEricssonTransport::slotIOReady(): "
	      "Huch, unknown state??? !!!!" ));
    break;
  }
}

void QObexEricssonTransport::timeout() {
  myDebug(( "QObexEricssonTransport::timeout()" ));
  switch ( mState ) {
  case NotConnected:
    // Usually at the beginning and after closing the connection.
    // If we have unrecoverable timeouts we end here also.
    // At this stage we can connect.
    myDebug(( "QObexEricssonTransport::timeout(): "
	      "Huch, state NotConnected!" ));
    break;

  case Connected:
    // The connection is up and idle. At this stage we can send data.
    myDebug(( "QObexEricssonTransport::timeout(): "
	      "Huch, state Connected!" ));
    break;

  case ATZSent:
    // We try to connect, first send a ATZ and check if there is a modem
    // attached to the serial line.
    if ( 0 < --mATZCount ) {
      myDebug(( "QObexEricssonTransport::timeout(): Sending ATZ" ));
      mSerial.sendModemCommand( "ATZ\r" );
      selectModeChanged( SelectRead, 200 );
    } else {
      myDebug(( "QObexEricssonTransport::timeout(): Dropping DTR" ));
      mState = DTRDropped;
      mSerial.dropDTR();
      selectModeChanged( SelectRead, 1000 );
    }
    break;

  case CGMISent:
    // We know now that we have a modem and check if it is built by Siemens.
    mState = NotConnected;
    myDebug(( "QObexEricssonTransport::timeout(): Set to not connected" ));
    setStatus( StatusError );
    error( TimeoutError );
    break;

  case ATEOBEXSent:
    // We try to switch to ERICSSON mode.
    mState = NotConnected;
    myDebug(( "QObexEricssonTransport::timeout(): Set to not connected" ));
    setStatus( StatusError );
    error( TimeoutError );
    break;

  case DTRDropped:
    // The modem is not responding, so try to initizlise by the hard way.
    // Algorythm is copied from minicoms "Initialize Modem".
    // And it will wake up every modem :-)
    // First stage is that the DTR line is dropped for a second.
    myDebug(( "QObexEricssonTransport::timeout(): Raising DTR" ));
    mState = DTRRaised;
    mSerial.raiseDTR();
    selectModeChanged( SelectRead, 1000 );
    break;

  case DTRRaised:
    // Second stage is that the DTR line is raised again. Then wait for an
    // additional second.
    myDebug(( "QObexEricssonTransport::timeout(): Sending \\r" ));
    mState = CRSent;
    mSerial.sendModemCommand( "\r" );
    selectModeChanged( SelectRead, 1000 );
    break;

  case CRSent:
    // Third stage is to send a \r and wait an additional second.
    // Past this command ew check again with ATZ and try to initialize
    // the modem.
    myDebug(( "QObexEricssonTransport::timeout(): Sending ATZ" ));
    mState = ATZSent;
    mSerial.sendModemCommand( "ATZ\r" );
    mATZCount = 15;
    selectModeChanged( SelectRead, 200 );
    break;

  default:
    myDebug(( "QObexEricssonTransport::timeout(): "
	      "Huch, unknown state??? !!!!" ));
    break;
  }
}

#include "qobexericssontransport.moc"
