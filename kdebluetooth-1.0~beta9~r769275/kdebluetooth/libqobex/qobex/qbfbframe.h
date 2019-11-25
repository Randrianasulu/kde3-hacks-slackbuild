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

#ifndef QBFBFRAME_H
#define QBFBFRAME_H

#include <qstring.h>
#include <qcstring.h>

class QBfbFrame
  : public QByteArray {
public:
  enum {
    TypePos = 0,
    SizePos = 1,
    ChkPos = 2,
    DataPos = 3,

    HeaderSize = 3,

    MTU = 32
  };

  // There may be others but who knows ...
  enum Type {
    InterfaceChange = 0x01,
    Connect = 0x02,
    Keypress = 0x05,
    Modem = 0x06,
    EEPROM = 0x14,
    Data = 0x16
  };

  // Creates a new bfb frame by reading up to MTU bytes from data.
  QBfbFrame() : QByteArray() {}
  QBfbFrame( Type t, const char *, int len );
  QBfbFrame( Type t, QByteArray& d );
  QBfbFrame( Type t, Q_UINT8 d );
  QBfbFrame( Type t, Q_UINT8 d0, Q_UINT8 d1 );

  static QString typeToQString( Type t );
  QString dump() const;

  int remainingSize() {
    // first we have to read the header.
    if ( size() < 3 )
      return 3;
    else
      return frameSize() - size();
  }
  bool headerComplete() const { return 3 <= size(); }
  bool headerValid() const {
    return at(ChkPos) == computeChk() && userDataSize() <= MTU;
  }
  Q_UINT8 computeChk() const { return at(TypePos) ^ at(SizePos); }
  Q_UINT8 readChk() const { return at(ChkPos); }
  bool frameComplete() const { return headerComplete() && size() == frameSize(); }
  Q_UINT8 frameSize() const { return at(SizePos) + 3; }
  Q_UINT8 type() const { return at(TypePos); }

  Q_UINT8 userDataSize() const { return at(SizePos); }
  const char *userData() const { return data() + DataPos; }
  char *userData() { return data() + DataPos; }

  void clear() { QByteArray::resize( 0, QGArray::SpeedOptim ); }
  void resize( int sz ) { QByteArray::resize( sz, QGArray::SpeedOptim ); }

  static const QBfbFrame hello;
  static const QBfbFrame helloAck;
  static const QBfbFrame dataAck;
  static QBfbFrame ifChange( int );
  static QBfbFrame ifChangeAck( int );
};

#endif
