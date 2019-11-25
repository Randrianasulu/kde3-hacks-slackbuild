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

#ifndef QOBEXIRDATRANSPORT_H
#define QOBEXIRDATRANSPORT_H

#include <qstring.h>
#include <qmap.h>

#include "qobextransport.h"

class QObexIrDATransport
  : public QObexTransport {
public:
  /**
     Standard constructor.
   */
  QObexIrDATransport( QObject* = 0, const char* = 0 );
  /**
     Constructor for use with an already accepted connection.
   */
  QObexIrDATransport( int, Q_UINT32, Q_UINT32, QObject* = 0, const char* = 0 );

  ~QObexIrDATransport();

  /**
     Returns the actual socket.
     Required by an OBEX transport.
   */
  int socket() const { return mFd; }

  /**
     Connect to the device specified by set address before. If no device
     is set or the device is set to 00:00:00:00 devices are discovered and
     the first discovered device is connected.
     Required by an OBEX transport.
   */
  bool connect();

  /**
     Disconnect from the device.
     Required by an OBEX transport.
   */
  void disconnect();

  /**
     Start listening with a given backlog.
     Required for OBEX server operations.
   */
  bool listen( int );

  /**
     Call this if you have a connection ready to accept.
     Required for OBEX server operations.
   */
  QObexTransport* accept();

  /**
     Returns the maximum transfer unit.
     Required by an OBEX transport.
   */
  Q_LONG maximumTransferUnit() const;

  /**
     Returns the maximum transfer unit.
     Required by an OBEX transport.
   */
  Q_LONG bytesPerSecond() const;

  /**
     Sets the peer address to connect to.
   */
  void setDestAddress( Q_UINT32 addr );

  /**
     Sets the peer address to connect to.
   */
  void setDestAddress( const QString& );

  /**
     Returns the peer address to connect to.
   */
  Q_UINT32 destAddress() const;

  /**
     Sets the source address where the connection is bound to.
     Defaults to 00:00:00:00.
   */
  void setSrcAddress( Q_UINT32 addr );

  /**
     Sets the source address where the connection is bound to.
     Defaults to 00:00:00:00.
   */
  void setSrcAddress( const QString& addr );

  /**
     Returns the source address where the connection is bound to.
   */
  Q_UINT32 srcAddress() const;

  /**
     Utility function to convert a string to a binary hardware address.
   */
  static Q_UINT32 string2Address( const QString& );

  /**
     Utility function to convert a binary hardware address to a string.
   */
  static QString address2String( Q_UINT32 );

  /**
     Utility function. Returns a list of devices which can be connected.
   */
  static QMap<Q_UINT32,QString> discoverDevices();
  
  /**
     Return a correctly decoded string for the given IrDA charset.
   */
  static QString decodeString( int, char*, int );

private:
  /**
     Internal helpers.
   */
  bool open();

  Q_UINT32 mSrc;
  Q_UINT32 mDst;
  int mFd;
};

#endif
