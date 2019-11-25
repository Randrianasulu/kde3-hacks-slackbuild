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

#ifndef QOBEXBFBTRANSPORT_H
#define QOBEXBFBTRANSPORT_H

#include <qstring.h>
#include <qvaluelist.h>

#include "qobextransport.h"

#include "qserialdevice.h"
#include "qbfbframe.h"
#include "qbfbdata.h"

class QObexBfbTransport
  : public QObexTransport {
  Q_OBJECT
public:
  enum Port {
    InterfaceChange = 0x01,
    Connect = 0x02,
    Keypress = 0x05,
    Modem = 0x06,
    EEPROM = 0x14,
    Data = 0x16
  };

  // We have here a complex state machine.
  enum State {
    // Usually at the beginning and after closing the connection.
    // If we have unrecoverable timeouts we end here also.
    // At this stage we can connect.
    NotConnected,

    // The connection is up and idle. At this stage we can send data.
    Connected,

    // We try to connect, first send a ATZ and check if there is a modem
    // attached to the serial line.
    ATZSent,
    // We know now that we have a modem and check if it is built by Siemens.
    CGMISent,
    // We try to switch to BFB mode.
    SBFBSent,
    // The modem is not responding, so try to initizlise by the hard way.
    // Algorythm is copied from minicoms "Initialize Modem".
    // And it will wake up every modem :-)
    // First stage is that the DTR line is dropped for a second.
    DTRDropped,
    // Second stage is that the DTR line is raised again. Then wait for an
    // additional second.
    DTRRaised,
    // Third stage is to send a \r and wait an additional second.
    // Past this command ew check again with ATZ and try to initialize
    // the modem.
    CRSent,
    // We want to have the faster speed and sent an interface change request.
    InterfaceChangeSent,
    // We sent a bfb ping frame. Waiting for an ack.
    PingSent,
    // We have sent a bfb data packet. Now we wait for the ack from the
    // other side. If we get an timeout in this stage retransmit the packet.
    BfbDataSent
  };

  enum {
    // Retransmit timeout: time to wait for a bfb ack before retransmitting
    // the bfb dta packet.
    BfbRetransmitTimeout = 4000,
    ATCommandTimeout = 500,
    BfbPingTimeout = 70,
    InterfaceChangeTimeout = 500,
    ATZDelay = 300,
    DTRDropDelay = 1000,
    DTRRaiseDelay = 1000,
    CRDelay = 1000
  };

  QObexBfbTransport( QObject* = 0, const char* = 0 );
  ~QObexBfbTransport();

  bool connect();
  void disconnect();

  int socket() const { return mSerial.getFd(); }

  Q_LONG readBlock( char *, Q_ULONG );
  Q_LONG writeBlock( const char *, Q_ULONG );
  
  SelectHint getSelectHint() const { return SelectRead; };
  int getDelayTimeout() const { return 0; };

  Q_LONG maximumTransferUnit() const;
  Q_LONG bytesPerSecond() const;

  // Sets the current channel.
  Port port() const;
  void setPort( Port );

  // Sets the speed
  int speed() const;
  void setSpeed( int = 57600 );

  // Sets the device
  void setDevice( const QString& );
  QString device() const;

  // Sets the transport blocking or nonblocking
  bool blocking () const { return mBlocking; }
  void setBlocking ( bool enable );

  virtual void reset();
  virtual void timeout();

public slots:
  virtual void slotIOReady();

private:
  Port mPort;
  Q_UINT8 mSeq;
  bool mBlocking;
  State mState;
  int mATZCount;
  int mPingCount;
  int mSpeed;

  QSerialDevice mSerial;

  // Here incomplete packets are stored.
  QBfbFrame mFrameBuffer;
  QBfbData mDataBuffer;
  QByteBuffer mReadBuffer;

  QBfbData mPendingBfbWrite;

  int writeWithDataPackets( const char *, Q_ULONG );
  int writeWithFrames( const char *, Q_ULONG );
  int writeFrame( const QBfbFrame& );
  int writeRaw( const char *, Q_ULONG );
  int completeDataPacket();
  int completeFrame();
  void leaveBfb();
};


#endif
