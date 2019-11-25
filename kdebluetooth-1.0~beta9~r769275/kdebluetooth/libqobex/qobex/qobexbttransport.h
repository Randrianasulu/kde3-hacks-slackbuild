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

#ifndef QOBEXBTTRANSPORT_H
#define QOBEXBTTRANSPORT_H

#include <qstring.h>
#include <qmap.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>

#include "qobextransport.h"

class QObexBtTransport
  : public QObexTransport {
public:
  enum {
    IrMCServ = IRMC_SYNC_SVCLASS_ID,
    IrMCCmdServ = IRMC_SYNC_CMD_SVCLASS_ID,
    ObexPushServ = OBEX_OBJPUSH_SVCLASS_ID,
    ObexFileServ = OBEX_FILETRANS_SVCLASS_ID
  } Service;

  /**
     Standard constructor.
   */
  QObexBtTransport( QObject* = 0, const char* = 0 );
  /**
     Constructor for use with an already accepted connection.
   */
  QObexBtTransport( int, const bdaddr_t*, const bdaddr_t*, QObject* = 0, const char* = 0 );

  ~QObexBtTransport();

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
  void setDestAddress( bdaddr_t );

  /**
     Sets the peer address to connect to.
   */
  void setDestAddress( const QString& );

  /**
     Returns the peer address to connect to.
   */
  bdaddr_t destAddress() const;

  /**
     Sets the source address where the connection is bound to.
     Defaults to 00:00:00:00:00:00.
   */
  void setSrcAddress( bdaddr_t );

  /**
     Sets the source address where the connection is bound to.
     Defaults to 00:00:00:00:00:00.
   */
  void setSrcAddress( const QString& );

  /**
     Returns the source address where the connection is bound to.
   */
  bdaddr_t srcAddress() const;

  /**
     Sets the rfcomm channel to use. If not set the sdp server is queried
     for the profile uuid set by setUuid( Q_UINT16 ).
   */
  void setRfCommChannel( int channel ) { mChannel = channel; }
  
  /**
     Returns the rfcomm channel. If unset 0 is returned.
     FIXME is 0 a valid rfcomm channel??
   */
  int rfCommChannel() const { return mChannel; }

  /**
     Sets the service profile uuid.
   */
  void setUuid( Q_UINT16 uuid ) { mUuid = uuid; }

  /**
     Returns the service profile uuid.
   */
  Q_UINT16 uuid() const { return mUuid; }

  /**
     Utility function to convert a string to a binary hardware address.
   */
  static bdaddr_t string2Address( const QString& );

  /**
     Utility function to convert a binary hardware address to a string.
   */
  static QString address2String( bdaddr_t );

  /**
     Utility function. Returns a list of devices which can be connected.
   */
  static QValueList<bdaddr_t> inquireDevices();
  
  /**
     Utility function. Returns the device name for the given hw address.
   */
  static QString getRemoteName( bdaddr_t );

  /**
     Utility function. Returns the device name for the given hw address.
   */
  static QString getRemoteName( const QString& );

  /**
     Utility function. Returns the rfcomm channel for the profile with the
     given uuid at the bluetooth host with the given hw address.
   */
  static int getRfCommChannel( bdaddr_t, Q_UINT16 );

  /**
     Utility function. Returns the rfcomm channel for the profile with the
     given uuid at the bluetooth host with the given hw address.
   */
  static int getRfCommChannel( const QString&, Q_UINT16 );

  /**
     Registers the profile given by uuid() at the local sdp server.
     This is done automatically on listen( int ).
   */
  bool registerSdp();

  /**
     Unregisters the service uuid given by uuid() at the local sdp server.
     Is done on transport deletion.
   */
  bool unregisterSdp();

private:
  /**
     Internal helpers.
   */
  bool discover();
  bool open();

  int mFd;
  int mChannel;
  bool mRegisterSdp;
  bdaddr_t mDest;
  bdaddr_t mSrc;
  Q_UINT16 mUuid;
  Q_UINT32 mSdpHandle;
};

#endif
