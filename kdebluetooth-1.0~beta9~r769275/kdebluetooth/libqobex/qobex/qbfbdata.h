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

#ifndef QBFBDATA_H
#define QBFBDATA_H

#include <qcstring.h>

// Class to handle bfb data packets.
// knowledge about the structure of these data is taken from
// the obexftp code and the bfb datadumps on the ftp server of
// Christian W. Zuckschwerdt <zany@triq.net>.
// Without his work it would be impossible for me to write all this code!
// Many Thanks to him!
//
// The data packets are laid out as follows
//  uint8_t cmd;
//  uint8_t chk;
//  uint8_t seq;
//  uint16_t len;
//  uint8_t data[...];
//  uint16_t crc;
// 
// the crc is computed from data position 2 (seq) to the last data byte.
//

class QBfbData
  : public QByteArray {
public:
  enum {
    TypePos = 0,
    ChkPos = 1,
    SeqPos = 2,
    SizePos = 3,
    DataPos = 5,
    CRCPos = 5 /* + data size */
  };

  enum Type {
    Invalid = 0,
    DataFirst = 2,
    DataNext = 3
  };

  QBfbData() : QByteArray() {}
  QBfbData( Q_UINT8 seq, Q_UINT16 len, const char* data );

  QString dump() const;

  // returns the remaining size of the data packet.
  // The header has to be complete when calling this function
/*   ssize_t remainingSize() const { return packetSize() - size(); } */
  bool headerComplete() const { return DataPos <= size(); }
  bool packetComplete() const {
    return headerComplete() && size() == packetSize();
  }
  unsigned int packetSize() const { return userDataSize() + 7; }
  Q_UINT8 type() const { return Q_UINT8(at(TypePos)); }
  Q_UINT16 userDataSize() const {
    Q_UINT8 hi = at(SizePos);
    Q_UINT8 lo = at(SizePos+1);
    return ( Q_UINT16(hi) << 8 ) + Q_UINT16(lo);
  }
  const char *userData() const { return data() + DataPos; }
  char *userData() { return data() + DataPos; }
  bool headerValid() const {
    return Q_UINT8(at( ChkPos )) == computeChk(); // && userDataSize() <= MTU;
  }
  Q_UINT8 computeChk() const { return ~Q_UINT8(at(TypePos)); }
 
  Q_UINT16 computeCrc() const {
    Q_UINT16 crc = qChecksum( data() + 2, userDataSize() + 3 );
/*     qDebug( "Crc computed from packet is %04x", Q_UINT32(crc) ); */
    return crc;
  }
  Q_UINT16 readCrc() const {
    Q_UINT16 ds = userDataSize();
    Q_UINT8 hi = at(CRCPos+ds+1);
    Q_UINT8 lo = at(CRCPos+ds);
    Q_UINT16 crc = ( Q_UINT16(hi) << 8 ) + Q_UINT16(lo);
/*     qDebug( "Crc stored in the packet is %04x", Q_UINT32(crc) ); */
    return crc;
  }
  bool crcOk() const {
    return readCrc() == computeCrc();
  }
  bool valid() const {
    return headerValid() && packetComplete() && crcOk();
  }

  void clear() { QByteArray::resize( 0, QGArray::SpeedOptim ); }
  void resize( int sz ) { QByteArray::resize( sz, QGArray::SpeedOptim ); }

};

#endif
