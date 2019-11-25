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

#ifndef QOBEXSERIALTRANSPORT_H
#define QOBEXSERIALTRANSPORT_H

#include <qstring.h>

#include "qobextransport.h"

#include "qserialdevice.h"

class QObexSerialTransport
  : public QObexTransport {
public:
  QObexSerialTransport( QObject* = 0, const char* = 0 );
  ~QObexSerialTransport();

  bool isConnected() const;

  bool connect();
  void disconnect();

  int socket() const { return mSerial.getFd(); }

  Q_LONG readBlock( char *, Q_ULONG );
  Q_LONG writeBlock( const char *, Q_ULONG );
  
  Q_LONG maximumTransferUnit() const;
  Q_LONG bytesPerSecond() const;

  // Sets the speed
  int speed() const;
  void setSpeed( int = 57600 );

  // Sets the device
  void setDevice( const QString& );
  QString device() const;

  // Sets the transport blocking or nonblocking
  bool blocking () const;
  void setBlocking ( bool );

private:
  mutable QSerialDevice mSerial;
};

#endif
