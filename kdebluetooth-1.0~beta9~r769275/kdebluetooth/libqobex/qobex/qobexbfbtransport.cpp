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
#include <errno.h>

#include "qserialdevice.h"

#include "qobexbfbtransport.h"
#include "qbfbframe.h"
#include "qbfbdata.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

// #define DUMP_BFB_FRAME
// #define DUMP_BFB_DATA

#ifdef DUMP_BFB_FRAME
#include <ctype.h>
#include <qfile.h>
#include <qtextstream.h>
namespace {

  QFile frameDump( "/tmp/bfb-frame-dump.txt" );
  QTextStream frameStream( (frameDump.open( IO_WriteOnly ), &frameDump) );

}
#endif

#ifdef DUMP_BFB_DATA
#include <ctype.h>
#include <qfile.h>
#include <qtextstream.h>
namespace {

  QFile dataDump( "/tmp/bfb-data-dump.txt" );
  QTextStream dataStream( (dataDump.open( IO_WriteOnly ), &dataDump) );

}
#endif


QObexBfbTransport::QObexBfbTransport(  QObject* parent, const char* name )
  : QObexTransport( parent, name ) {
  myDebug(( "QObexBfbTransport::QObexBfbTransport( ... )" ));

  setStatus( StatusOpen );
  mBlocking = true;
  setDevice( "/dev/ttyS0" );
  setSpeed();
  mState = NotConnected;
  mATZCount = 0;
  mPingCount = 0;
  mPort = Modem;
  mSeq = 0;
}

QObexBfbTransport::~QObexBfbTransport() {
  myDebug(( "QObexBfbTransport::~QObexBfbTransport()" ));
  leaveBfb();
  mSerial.close();
  mSerial.setSpeed();
}

void QObexBfbTransport::reset() {
  myDebug(( "QObexBfbTransport::reset()" ));
  mSerial.close();
  mSerial.setSpeed();
  mFrameBuffer.clear();
  mDataBuffer.clear();
  mReadBuffer.clear();
  mPendingBfbWrite = QBfbData();

  setStatus( StatusOpen );
  mState = NotConnected;
  mATZCount = 0;
  mPingCount = 0;
  mPort = Modem;
  mSeq = 0;
}

void QObexBfbTransport::leaveBfb() {
  myDebug(( "QObexBfbTransport::leaveBfb()" ));
  if ( mState == Connected ) {
    setPort( Modem );
    // FIXME read this at the beginning and restore this value.
    // Note that we can't use sendModemCommand here because we are in bfb
    // mode and modem commands are sent via modem frames ...
    writeWithFrames( "AT^SBFB=2\r", 10 );
    // This seems to make sure that the mobile does get the chance to
    // write the response to an online serial line. (?)
    if ( 0 < mSerial.waitForMore( 300 ) ) {
      char buf[128];
      mSerial.readBlock( buf, sizeof(buf) );
    }
    mState = NotConnected;
    setStatus( StatusClosed );
  }
}

void QObexBfbTransport::disconnect() {
  myDebug(( "QObexBfbTransport::disconnect()" ));
  leaveBfb();
  mSerial.close();
  mSerial.setSpeed();
  mATZCount = 0;
  mPingCount = 0;
  mPort = Modem;
  mSeq = 0;
  connectionClosed();
}

bool QObexBfbTransport::connect() {
  myDebug(( "QObexBfbTransport::connect( ... )" ));
  if ( !mSerial.isOpen() ) {
    // Reset to default speed
    mSerial.setSpeed();
    mSerial.open( IO_ReadWrite );
    mSerial.setBlocking( mBlocking );
    mState = NotConnected;
    setStatus( StatusOpen );
  }
  if ( !mSerial.isOpen() ) {
    setStatus( StatusError );
    error( ConnectionRefused );
    return false;
  }

  if ( mState == NotConnected ) {
    // Just to be sure
    mFrameBuffer.clear();
    mDataBuffer.clear();
    mReadBuffer.clear();
    
    // We expect a modem here. If we have found it we switch to a special
    // binary data transfer mode.
    mSerial.sendModemCommand( "ATZ\r" );
    mATZCount = 10;
    mState = ATZSent;
    selectModeChanged( SelectRead, ATZDelay );
  }

  return true;
}

Q_LONG QObexBfbTransport::maximumTransferUnit() const {
  myDebug(( "QObexBfbTransport::maximumTransferUnit()" ));
  // REALLY???
  return 65535;
}

Q_LONG QObexBfbTransport::bytesPerSecond() const {
  return speed()/10;
}

// Sets the current channel.
void QObexBfbTransport::setPort( Port c ) {
  myDebug(( "QObexBfbTransport::setPort()" ));
  mReadBuffer.clear();
  mFrameBuffer.clear();
  mDataBuffer.clear();
  mPort = c;
}

QObexBfbTransport::Port QObexBfbTransport::port() const {
  return mPort;
}

// Sets the speed
void QObexBfbTransport::setSpeed( int speed ) {
  myDebug(( "QObexBfbTransport::setSpeed()" ));
  if ( speed == 115200 )
    mSpeed = 115200;
  else
    mSpeed = 57600;
}

int QObexBfbTransport::speed() const {
//   myDebug(( "QObexBfbTransport::speed()" ));
  return mSpeed;
}

// Sets the speed
void QObexBfbTransport::setDevice( const QString& dev ) {
  myDebug(( "QObexBfbTransport::setDevice()" ));
  mSerial.setDevice( dev );
}

QString QObexBfbTransport::device() const {
//   myDebug(( "QObexBfbTransport::device()" ));
  return mSerial.device();
}

void QObexBfbTransport::setBlocking ( bool enable ) { 
  myDebug(( "QObexBfbTransport::setBlocking( %d )", enable ));
  if ( mSerial.isOpen() )
    mSerial.setBlocking( enable );
  mBlocking = enable;
}

Q_LONG QObexBfbTransport::readBlock( char *buf, Q_ULONG len ) {
  myDebug(( "QObexBfbTransport::readBlock( ... )" ));
  if ( len <= mReadBuffer.size() )
    return mReadBuffer.get( buf, len );
  if ( mPort != Data )
    return -1;

  Q_LONG read = completeDataPacket();
  if ( mReadBuffer.size() == 0 && read < 0 )
    return -1;

  return mReadBuffer.get( buf, len );
}

Q_LONG QObexBfbTransport::writeBlock( const char *buf, Q_ULONG len ) {
  myDebug(( "QObexBfbTransport::writeBlock( ... )" ));
  if ( mPort != Data )
    return -1;

  // special handling for data frames
  return writeWithDataPackets( buf, len );
}

int QObexBfbTransport::writeWithDataPackets( const char *data, Q_ULONG len ) {
  myDebug(( "QObexBfbTransport::writeWithDataPackets()" ));

  QBfbData packet( mSeq++, len, data );
  myDebug(( "QObexBfbTransport::writeWithDataPackets(): "
	    "Writing bfb data packet of len %d", (int) len ));
  
#ifdef DUMP_BFB_DATA
  dataStream << "Write:" << endl;
  dataStream << packet.dump() << endl;
  dataStream.device()->flush();
#endif
  writeWithFrames( packet.data(), packet.size() );

  mPendingBfbWrite = packet;
  mState = BfbDataSent;

  selectModeChanged( SelectRead, BfbRetransmitTimeout );

  return len;
}

int QObexBfbTransport::writeWithFrames( const char *data, Q_ULONG len ) {
  myDebug(( "QObexBfbTransport::writeWithFrames()" ));
  Q_ULONG written = 0;
  while ( written < len ) {
    int curLen = len-written;
    if ( QBfbFrame::MTU < curLen )
      curLen = QBfbFrame::MTU;

    myDebug(( "QObexBfbTransport::writeWithFrames(), curLen = %d", curLen ));

    QBfbFrame frame( (QBfbFrame::Type)mPort, data+written, curLen );
    if ( writeFrame( frame ) < 0 )
      return -1;

    written += curLen;
  }
  return written;
}

int QObexBfbTransport::writeFrame( const QBfbFrame& frame ) {
  myDebug(( "QObexBfbTransport::writeFrame( ... )" ));
#ifdef DUMP_BFB_FRAME
  frameStream << "Write:" << endl;
  frameStream << frame.dump() << endl;
  frameStream.device()->flush();
#endif

// #define RETRANSMIT_TESTING
#ifdef RETRANSMIT_TESTING
/////////////////////////////////////////////////////////////////////////
  if ( rand()%2 && rand()%2 && mPort == Data
       && frame != QBfbFrame::dataAck && 10 < frame.size() )
    frame.at(10) = 0;
#endif

  return writeRaw( frame.data(), frame.size() );
}

int QObexBfbTransport::writeRaw( const char *data, Q_ULONG len ) {
  myDebug(( "QObexBfbTransport::writeRaw( ... )" ));
  Q_ULONG written = 0;
  // switch to blocking mode to be sure all data gets written.
  bool blk = mSerial.blocking();
  if ( !blk )
    mSerial.setBlocking( true );
  do {
    Q_LONG writeRet = mSerial.writeBlock( data+written, len-written );
    if ( writeRet < 0 ) {
      if ( !blk )
	mSerial.setBlocking( blk );
      return -1;
    } else
      written += writeRet;
  } while ( written < len );
  if ( !blk )
    mSerial.setBlocking( blk );
  return written;
}

int QObexBfbTransport::completeDataPacket() {
  myDebug(( "QObexBfbTransport::completeDataPacket()" ));

  // As long as we get bfb frames
  do {
    // here we know that we really got data. Collect it and return positive
    // if the packet is ok so far.
    if ( mFrameBuffer.frameComplete() ) {
      Q_LONG oldSize = mDataBuffer.size();
      Q_UINT8 fbs = mFrameBuffer.userDataSize();
      mDataBuffer.resize( oldSize + fbs );
      ::memcpy( mDataBuffer.data()+oldSize, mFrameBuffer.userData(), fbs );
      mFrameBuffer.clear();
    }
    
    if ( mDataBuffer.packetComplete() ) {
      myDebug(( "QObexBfbTransport::completeDataPacket()"
		"BFB data packet is complete!" ));
#ifdef DUMP_BFB_DATA
      dataStream << "Read:" << endl;
      dataStream << mDataBuffer.dump() << endl;
      dataStream.device()->flush();
#endif
      
      // && mDataBuffer.seq() FIXME ???
      if ( mDataBuffer.valid() ) {
	myDebug(( "QObexBfbTransport::completeDataPacket()"
		  "Successfully completed BFB data packet. Sending Ack!" ));
	mReadBuffer.put( mDataBuffer.userData(), mDataBuffer.userDataSize() );
	mDataBuffer = QBfbData();
	// respond with an ack ...
	writeFrame( QBfbFrame::dataAck );
	return mReadBuffer.size();
      } else {
	myDebug(( "QObexBfbTransport::completeDataPacket()"
		  "Got an invalid BFB data packet. Ignoring it ... " ));
	// flush all buffers ant thow them away. The device will retransmit.
	mSerial.clear();
	mFrameBuffer.clear();
	mDataBuffer.clear();
      }
    }
    
    // try to read the next packet ...
  } while ( 0 < completeFrame() );
  
  return 0;
}

// Tries to complete reading of the currrent frame.
// Return the size of user data of the completed frame.
// If it is not complete return 0 and <0 if there was a parsing error.
int QObexBfbTransport::completeFrame() {
   myDebug(( "QObexBfbTransport::completeFrame()" ));

  // first try to complete the header
  if ( !mFrameBuffer.headerComplete() ) {
    Q_LONG oldSize = mFrameBuffer.size();
    Q_LONG remain = QBfbFrame::HeaderSize - oldSize;
    mFrameBuffer.resize( QBfbFrame::HeaderSize );
    Q_LONG readRet = mSerial.readBlock( mFrameBuffer.data()+oldSize, remain );
    if ( 0 <= readRet ) {
      mFrameBuffer.resize( readRet + oldSize );
    } else {
      myDebug(( "QObexBfbTransport::completeFrame() Got an error reading "
		"data. Truncating frame buffer %s", strerror(errno ) ));
      mFrameBuffer.clear();
      return -1;
    }
  }
  // check if we have now the complete header received
  if ( mFrameBuffer.headerComplete() ) {
    if ( !mFrameBuffer.headerValid() ) {
      // there is an error in the current frame data
      myDebug(( "QObexBfbTransport::completeFrame() Got an invalid BFB "
		"header. Truncating frame buffer" ));
      mSerial.clear();
      mFrameBuffer.clear();
      mDataBuffer.clear();
    } else {
      Q_LONG remain = mFrameBuffer.remainingSize();
      Q_LONG oldSize = mFrameBuffer.size();
      mFrameBuffer.resize( oldSize + remain );
      Q_LONG readRet = mSerial.readBlock( mFrameBuffer.data()+oldSize, remain );
      if ( 0 <= readRet ) {
	mFrameBuffer.resize( readRet + oldSize );
	if ( mFrameBuffer.frameComplete() ) {
	  myDebug(( "QObexBfbTransport::completeFrame(): "
		    "successfully read frame" ));
#ifdef DUMP_BFB_FRAME
	  frameStream << "Read:" << endl;
	  frameStream << mFrameBuffer.dump() << endl;
	  frameStream.device()->flush();
#endif
	  if ( mFrameBuffer.type() == mPort ) {
	    return mFrameBuffer.userDataSize();
	  } else {
	    // ignore unexpected frames
	    myDebug(( "QObexBfbTransport::completeFrame(): Got data at "
		      "unexpected port. Truncating frame buffer" ));
	    myDebug(( "%s", mFrameBuffer.dump().ascii() ));
	    mFrameBuffer.clear();
	    return 0;
	    // 	  return -1;
	  }
	} else
	  return 0;
      } else {
	myDebug(( "QObexBfbTransport::completeFrame(): Got an error reading "
		  "data. Truncating frame buffer" ));
	myDebug(( "%s", mFrameBuffer.dump().ascii() ));
	mFrameBuffer.clear();
	return -1;
      }
    }
  }

  // one will never get here, but gcc does not see that ...
  return -1;
}

void QObexBfbTransport::slotIOReady() {
  myDebug(( "QObexBfbTransport::slotIOReady()" ));
  switch ( mState ) {
  case NotConnected:
    // Usually at the beginning and after closing the connection.
    // If we have unrecoverable timeouts we end here also.
    // At this stage we can connect.
    myDebug(( "QObexBfbTransport::slotIOReady(): "
	      "ups, we get data in state NotConnected!" ));
    mSerial.clear();
    mFrameBuffer.clear();
    mDataBuffer.clear();
    mReadBuffer.clear();
    break;

  case Connected:
    myDebug(( "QObexBfbTransport::slotIOReady(): "
	      "handle data while connected ..." ));

    // The connection is up and idle. At this stage we can send data.
    if ( port() == Data ) {
      completeDataPacket();
      while ( 0 < mReadBuffer.size() ) {
	readyRead();
      }
    } else {
      completeFrame();
      while ( 0 < mReadBuffer.size() ) {
	readyRead();
      }
    }
    break;

  case ATZSent:
    // We try to connect, first send a ATZ and check if there is a modem
    // attached to the serial line.
    myDebug(( "QObexBfbTransport::slotIOReady(): ATZSent" ));
    while ( mSerial.lineAvailable() ) {
      QString answer = mSerial.readLine();
      myDebug(( "QObexBfbTransport::slotIOReady(): Answer is %s",
		answer.latin1() ));
      if ( answer == "OK" ) {
	myDebug(( "QObexBfbTransport::slotIOReady(): Sending AT+CGMI" ));
	mSerial.sendModemCommand( "AT+CGMI\r" );
	mState = CGMISent;
	selectModeChanged( SelectRead, ATCommandTimeout );
      } else if ( answer == "ERROR" ) {
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
      myDebug(( "QObexBfbTransport::slotIOReady(): Answer is %s",
		answer.latin1() ));
      if ( answer == "SIEMENS" || answer == "AT+CGMI" || answer.isEmpty() ) {
	// Ok, wait for the next line
      } else if ( answer == "OK" ) {
	myDebug(( "QObexBfbTransport::slotIOReady(): Sending AT^SBFB=1" ));
	mSerial.sendModemCommand( "AT^SBFB=1\r" );
	mState = SBFBSent;
	selectModeChanged( SelectRead, ATCommandTimeout );
      } else {
	mState = NotConnected;
	setStatus( StatusError );
	error( ConnectionRefused );
	break;
      }
    }
    break;

  case SBFBSent:
    // We try to switch to BFB mode.
    while ( mSerial.lineAvailable() ) {
      QString answer = mSerial.readLine();
      myDebug(( "QObexBfbTransport::slotIOReady(): Answer is %s",
		answer.latin1() ));
      if ( answer == "OK" ) {
	myDebug(( "QObexBfbTransport::slotIOReady(): Sending Ping" ));
	setPort( Connect );
	writeFrame( QBfbFrame::hello );
	mState = PingSent;
	mPingCount = 4;
	selectModeChanged( SelectRead, BfbPingTimeout );
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
    myDebug(( "QObexBfbTransport::slotIOReady(): "
	      "ups, we get data in state DTRDropped!" ));
    mSerial.clear();
    mFrameBuffer.clear();
    mDataBuffer.clear();
    mReadBuffer.clear();
    break;

  case DTRRaised:
    // Second stage is that the DTR line is raised again. Then wait for an
    // additional second.
    myDebug(( "QObexBfbTransport::slotIOReady(): "
	      "ups, we get data in state DTRRaised!" ));
    mSerial.clear();
    mFrameBuffer.clear();
    mDataBuffer.clear();
    mReadBuffer.clear();
    break;

  case CRSent:
    // Third stage is to send a \r and wait an additional second.
    // Past this command ew check again with ATZ and try to initialize
    // the modem.
    myDebug(( "QObexBfbTransport::slotIOReady(): "
	      "ups, we get data in state CRSent!" ));
    mSerial.clear();
    mFrameBuffer.clear();
    mDataBuffer.clear();
    mReadBuffer.clear();
    break;

  case InterfaceChangeSent:
    // We want to have the faster speed and sent an interface change request.
    if ( 0 < completeFrame() ) {
      // We are connected here anyway.
      // But switch speed if we can.
      // FIXME issue a warning that the requested speed is not set.
      if ( mFrameBuffer == QBfbFrame::ifChangeAck( mSpeed ) )
	mSerial.setSpeed( mSpeed );

      myDebug(( "QObexBfbTransport::slotIOReady(): We are connected" ));
      setPort( Data );
      mState = Connected;
      setStatus( StatusConnected );
      connected();

      mFrameBuffer.clear();
      mDataBuffer.clear();
      mReadBuffer.clear();
    }
    break;

  case PingSent:
    // We sent a bfb ping frame. Waiting for an ack.
    if ( 0 < completeFrame() ) {
      if ( mFrameBuffer == QBfbFrame::helloAck ) {
	if ( mSpeed == 115200 ) {
	  setPort( InterfaceChange );
	  writeFrame( QBfbFrame::ifChange( mSpeed ) );
	  mState = InterfaceChangeSent;
	  selectModeChanged( SelectRead, InterfaceChangeTimeout );
	} else {
	  myDebug(( "QObexBfbTransport::slotIOReady(): We are connected" ));
	  setPort( Data );
	  setStatus( StatusConnected );
	  mState = Connected;
	  connected();
	}
      } else {
	mState = NotConnected;
	setStatus( StatusError );
	error( ConnectionRefused );
      }
      mFrameBuffer.clear();
      mDataBuffer.clear();
      mReadBuffer.clear();
    }
    break;

  case BfbDataSent:
    // We have sent a bfb data packet. Now we wait for the ack from the
    // other side. If we get an timeout in this stage retransmit the packet.
    myDebug(( "QObexBfbTransport::slotIOReady(): Data BfbDataSent" ));
    if ( 0 < completeFrame() ) {
      if ( mFrameBuffer == QBfbFrame::dataAck ) {
	myDebug(( "QObexBfbTransport::slotIOReady(): "
		  "Data BfbDataSent, got Ack" ));
	mFrameBuffer.clear();
	bytesWritten( mPendingBfbWrite.userDataSize() );
	mPendingBfbWrite = QBfbData();
	mState = Connected;
      } else {
	myDebug(( "QObexBfbTransport::slotIOReady(): Data BfbDataSent, "
		  "got non Data frame, consider this an error" ));
	mFrameBuffer.clear();
	mState = NotConnected;
	setStatus( StatusError );
	error( WriteError );
      }
    }
    break;

  default:
    myDebug(( "QObexBfbTransport::slotIOReady(): Huch, unknown state??? !!!!" ));
    break;
  }
}

void QObexBfbTransport::timeout() {
  myDebug(( "QObexBfbTransport::timeout()" ));
  switch ( mState ) {
  case NotConnected:
    // Usually at the beginning and after closing the connection.
    // If we have unrecoverable timeouts we end here also.
    // At this stage we can connect.
    myDebug(( "QObexBfbTransport::timeout(): "
	      "Huch, state NotConnected!" ));
    break;

  case Connected:
    // The connection is up and idle. At this stage we can send data.
    myDebug(( "QObexBfbTransport::timeout(): "
	      "Huch, state Connected!" ));
    break;

  case ATZSent:
    // We try to connect, first send a ATZ and check if there is a modem
    // attached to the serial line.
    if ( 0 < --mATZCount ) {
      myDebug(( "QObexBfbTransport::timeout(): Sending ATZ" ));
      mSerial.sendModemCommand( "ATZ\r" );
      selectModeChanged( SelectRead, ATZDelay );
    } else {
      myDebug(( "QObexBfbTransport::timeout(): Dropping DTR" ));
      mState = DTRDropped;
      mSerial.dropDTR();
      selectModeChanged( SelectRead, DTRDropDelay );
    }
    break;

  case CGMISent:
    // We know now that we have a modem and check if it is built by Siemens.
    mState = NotConnected;
    myDebug(( "QObexBfbTransport::timeout(): Set to not connected" ));
    setStatus( StatusError );
    error( TimeoutError );
    break;

  case SBFBSent:
    // We try to switch to BFB mode.
    mState = NotConnected;
    myDebug(( "QObexBfbTransport::timeout(): Set to not connected" ));
    setStatus( StatusError );
    error( TimeoutError );
    break;

  case DTRDropped:
    // The modem is not responding, so try to initizlise by the hard way.
    // Algorythm is copied from minicoms "Initialize Modem".
    // And it will wake up every modem :-)
    // First stage is that the DTR line is dropped for a second.
    myDebug(( "QObexBfbTransport::timeout(): Raising DTR" ));
    mState = DTRRaised;
    mSerial.raiseDTR();
    selectModeChanged( SelectRead, DTRRaiseDelay );
    break;

  case DTRRaised:
    // Second stage is that the DTR line is raised again. Then wait for an
    // additional second.
    myDebug(( "QObexBfbTransport::timeout(): Sending \\r" ));
    mState = CRSent;
    mSerial.sendModemCommand( "\r" );
    selectModeChanged( SelectRead, CRDelay );
    break;

  case CRSent:
    // Third stage is to send a \r and wait an additional second.
    // Past this command ew check again with ATZ and try to initialize
    // the modem.
    myDebug(( "QObexBfbTransport::timeout(): Sending ATZ" ));
    mState = ATZSent;
    mSerial.sendModemCommand( "ATZ\r" );
    mATZCount = 15;
    selectModeChanged( SelectRead, ATZDelay );
    break;

  case InterfaceChangeSent:
    // We want to have the faster speed and sent an interface change request.
    mState = NotConnected;
    myDebug(( "QObexBfbTransport::timeout(): Set to not connected" ));
    setStatus( StatusError );
    error( TimeoutError );
    break;

  case PingSent:
    // We sent a bfb ping frame. Waiting for an ack.
    if ( 0 < --mPingCount ) {
      myDebug(( "QObexBfbTransport::timeout(): Sending Ping" ));
      setPort( Connect );
      writeFrame( QBfbFrame::hello );
      mState = PingSent;
      selectModeChanged( SelectRead, BfbPingTimeout );
    } else {
      myDebug(( "QObexBfbTransport::timeout(): Drop DTR" ));
      mState = DTRDropped;
      mSerial.dropDTR();
      selectModeChanged( SelectRead, DTRDropDelay );
    }

    break;

  case BfbDataSent:
    // We have sent a bfb data packet. Now we wait for the ack from the
    // other side. If we get an timeout in this stage retransmit the packet.
    myDebug(( "QObexBfbTransport::timeout(): Resending bfbdata packet" ));
    writeWithFrames( mPendingBfbWrite.data(), mPendingBfbWrite.size() );
    selectModeChanged( SelectRead, BfbRetransmitTimeout );
    break;

  default:
    myDebug(( "QObexBfbTransport::timeout(): Huch, unknown state??? !!!!" ));
    break;
  }
}

#include "qobexbfbtransport.moc"
