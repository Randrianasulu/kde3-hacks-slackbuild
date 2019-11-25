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

#ifndef QOBEXTRANSPORT_H
#define QOBEXTRANSPORT_H

#include <qobject.h>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <kdebug.h>

#include "qobexheader.h"
#include "qobexobject.h"

class QObexTransport
  : public QObject {
  Q_OBJECT
public:
  enum {
    MaximumMtu = 0xffff,
    MinimumMtu = 255
  };

  enum Status {
    StatusOpen = 1,
    StatusConnected, // If idle and ready
    StatusClosed,
    StatusWriting, // If we are during a write
    StatusReading, // If we are during a read
    StatusError
  };
  
  enum Error {
    ConnectionRefused = 1,
    WriteError,
    ReadError,
    PacketTooBig,
    ParseError,
    BindError,
    ListenError,
    AcceptError,
    TimeoutError,
    ConnectionReset,
    UnknownError
  };

  QObexTransport( QObject*, const char* );
  virtual ~QObexTransport();
  
  /**
     Access function for the obexserver and client.
     Sends an QObexObject with the transport.
     Returns if it was able to send the object.
     Note, that an object is rejected because is too big
     for the negotiated Mtu.
     It the object is a connect object it is rejected if it
     does not fit the transport's Mtu requirements or if it
     is smaller then 255.
  */
  bool sendObject( const QObexObject* );

  /**
     Used when we want to have asynchronous clients/servers to
     give a hint what type of select we should do.
     SelectRead means that you should do a select for reading
     SelectWrite means that you should do a select for writes
     SelectDisable means that you should not select on this transport.
     Usually this occures when the connection is closed.
   */
  enum SelectHint {
    SelectRead = 1,
    SelectWrite,
    SelectDisable
  };

  /**
     Returns the select hint value for the transport in the current state.
   */
  SelectHint getSelectHint() const { return mSelectHint; }

  /**
     Returns the socket filedeskriptor of the transport.
     Use this filedescriptor for select calls if you want to do
     asynchronous OBEX servers/clients.
   */
  virtual int socket() const = 0;

  /**
     The classical connect call. The address where the transport should
     connect to must be already known.
     Note that this call may be asynchronous and it terminates with either
     signalConnected() or signalError( ... )
   */
  virtual bool connect() = 0;
  
  /**
     Disconnect from transport.
   */
  virtual void disconnect() = 0;

  /**
     The classical listen call.
   */
  virtual bool listen( int = 5 );
  /**
     More or less the classical accept call.
     It returns a new ready to use and connected
     transport or 0 if an error occurs.
   */
  virtual QObexTransport* accept();

  /**
     Should return the maximum transfer unnit of this transport.
     Writes are clustered up to this mtu to avoid too many fragmented
     small packets on the transport. Use a value of 1 for serial transports
     without any encapsulation.
   */
  virtual Q_LONG maximumTransferUnit() const = 0;

  /**
     Should return the speed of the connection in bytes per second.
     This is used to estimate an optimal MTU for OBEX packets. MTU is
     chosen so that an OBEX packet will not take longer than 1/4 second on
     the transport.
  */
  virtual Q_LONG bytesPerSecond() const = 0;

  /**
     Returns if the current transport block reads and writes or not.
   */
  virtual bool blocking() const;

  /**
     Sets makes this transport blocking according to the argument.
   */
  virtual void setBlocking( bool );

  /**
     Returns if the transport is connected.
   */
  bool isConnected() const {
    return mStatus == StatusConnected
      || mStatus == StatusWriting
      || mStatus == StatusReading;
  }
  
  /**
     Returns if the transport is idle, that means if a packet can be sent.
  */
  bool isIdle() const { return mStatus == StatusConnected; }
  
  /**
     Returns if the transport is active
  */
  bool isActive() const {
    return mStatus != StatusError;
  }
  /**
     Returns the current status.
  */
//   Status status() const { return mStatus; }
  /**
     Returns the current error state.
  */
//   Error error() { return mError; }
  
  /**
     Used by the client to tell the transport parser that we expect a connect
     response next.
  */
  void setParseHint( QObexObject::PacketType type ) { mPacketType = type; }


  /**
     Selects according to the current select hint to the next timeout.
     Returns negative on error, 0 on timeout and positive on success.
   */
  int doSelect();

//   virtual int bytesAvailable() const {
//     myDebug(( "QObexTransport::bytesAvailable()" )); 

//     fd_set fds;
//     FD_ZERO( &fds );
//     FD_SET( socket(), &fds );
    
//     return ::select( socket()+1, &fds, 0, 0, 0 );
//   }

protected:
  /**
     The real read function. If not reimplemented reads
     from socket() filedescriptor. The number ob bytes read from the transport
     is returned.
   */
  virtual Q_LONG readBlock( char *, Q_ULONG );

  /**
     The real write function. If not reimplemented writes
     to socket() filedescriptor. The number of bytes accepted for writes
     is returned.
   */
  virtual Q_LONG writeBlock( const char *, Q_ULONG );

  /**
     If you reimplement writeBlock( const char *, Q_ULONG ) call this 
     function if the function has really written out the given number of
     bytes.
   */
  void bytesWritten( Q_LONG bytes );

  /**
     Call to report an error.
   */
  void error( Error );

  /**
     Call when connetced.
   */
  void connected();

  /**
     Call when disconnected or when the connection is closed by
     the peer.
   */
  void connectionClosed();

  /**
     Call when the select mode should change.
   */
  void selectModeChanged( QObexTransport::SelectHint, Q_LONG );

  /**
     Called in the timeout slot. Reimplement if the transport needs to do
     something on timeout. Note that if the timeout here is the read, write
     or connect timeout, reset() is called instead.
   */
  virtual void timeout() {}

  /**
     Called when the transport is reset vial slotReset.
     Reimplement if the transport needs to do something on transport reset.
  */
  virtual void reset() {}

  /**
     Call when the Transport has some bytes to read.
   */
  void readyRead();

  /*
    FIXME
   */
  void readyWrite();

  /*
    FIXME
   */
  void setStatus( Status s ) { mStatus = s; }

  /*
    FIXME
   */
  void setError( Error e ) { mError = e; }

signals:
  /**
     Emitted when a connection is esablished.
   */
  void signalConnected();

  /**
     Emitted when a connection can be accepted.
   */
  void signalConnectionAccept( QObexTransport* );

  /**
     Emitted if the transport is closed.
   */
  void signalConnectionClosed();

  /**
     Emitted when a correct packet is received.
   */
  void signalPacketRecieved( QObexObject& );

  /**
     Emitted when a correct packet is written. Mostly for debugging ...
   */
  void signalWritingPacket( const QObexObject* );

  /**
     Emitted past a packet is written.
   */
  void signalPacketWritten();

  /**
     Emitted when an error occurs.
   */
  void signalError( QObexTransport::Error );

  /**
     Emitted the transport wants to change select mode.
     The second argument is a timeout in milliseconds.
     You should change these timeout values since the serial transports
     use them for delays in nonblocking modem initialization.
   */
  void signalSelectModeChanged( QObexTransport::SelectHint, Q_LONG );

public slots:
  /**
     Read/write multiplexer. Chooses which io to process based on
     the select hint. Must be reimplemented by transports changing the select
     hint.
  */
  virtual void slotIOReady();

  /**
     When select'ing yourself on the transport socket call this one if
     the timeout triggers.
   */
  void slotTimeout();

  /**
     Call if you want to reset the timeout to initial state.
     FIXME, verify ...
   */
  void slotReset();

  /**
     Thought to connect here a socketnotifier for a server transport.
   */
  void slotReadyAccept( int );

  
  /**
     HMM
   */
  void startCommand( Q_LONG msecs ) {
	 kdDebug() << "foobar" << endl;
    if ( msecs <= 0 ) {
      mCommandDeadline = QDateTime();
      return;
    }

    mCommandDeadline = QDateTime::currentDateTime();
    QTime time = mTimeout.time();
    time = time.addMSecs( msecs );
    if ( time < mCommandDeadline.time() )
      mCommandDeadline = mCommandDeadline.addDays( 1 );
    mCommandDeadline.setTime( time );
  }

private:
  /**
     Reads exact bytes from transport. Returns the whole data or nothing.
   */
  QObexArray readBlock( Q_ULONG bytes );

  /**
     Resets the write buffers.
   */
  void resetWriteState();

  /**
     Resets the read buffers.
   */
  void resetReadState();

  /**
     Used for async nonblocking writes.
   */
  QValueList<QByteArray> mWriteList;
  Q_ULONG mWritten;
  Q_LONG mPendingWrites;

  /**
     Used for the async nonblocking packet parsing code.
   */
  QObexObject::PacketType mPacketType;
  QObexArray mPacketHeaderData;
  Q_LONG mPacketCode;
  Q_LONG mRemainingPacketSize;

  QValueList<QObexHeader> mHeaderList;
  Q_LONG mHeaderCode;
  Q_LONG mHeaderSize;

  QByteArray mAtomicReadBuffer;


  SelectHint mSelectHint;
  QDateTime mCommandDeadline;
  QDateTime mTimeout;

  /**
     Read timeout.
   */
  int mReadTimeout;
  /**
     Write timeout.
   */
  int mWriteTimeout;

  /**
     Holds the device Status.
   */
  Status mStatus;
  /**
     Holds the device Error value.
   */
  Error mError;

};

#endif
