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

#ifndef QOBEXINTRANSPORT_H
#define QOBEXINTRANSPORT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "qobextransport.h"

class QObexInTransport
  : public QObexTransport {
public:
  enum {
    DefaultObexPort = 650,
    ObexUserPort = 29650
  };

  QObexInTransport( QObject* = 0, const char* = 0 );
  QObexInTransport( int, const in_addr_t*, const in_addr_t*, QObject* = 0, const char* = 0 );
  ~QObexInTransport();

  int socket() const;
  bool connect();
  void disconnect();

  bool bind();
  bool listen( int );
  QObexTransport* accept();

  Q_LONG maximumTransferUnit() const;
  Q_LONG bytesPerSecond() const;

  void setHost( const QString& );
  void setSrc( const QString& );
  void setPort( int );

private:

  bool open();

  static int defaultObexPort();

  in_addr_t mSrc;
  in_addr_t mDst;
  int mPort;

  int mFd;
};

#endif
