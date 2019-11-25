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

#include <ctype.h>

#include <qstring.h>

#include "qbfbdata.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QBfbData::QBfbData( Q_UINT8 seq, Q_UINT16 len, const char* data )
  : QByteArray( 7 + len ) {
  at(TypePos) = ( seq == 0 ? DataFirst : DataNext );
  at(SeqPos) = seq;
  at(ChkPos) = computeChk();
  at(SizePos) = (0xff00&len) >> 8;
  at(SizePos+1) = 0xff&len;
  ::memcpy( &at(DataPos), data, len );
  Q_UINT16 crc = computeCrc();
#ifdef DEBUG
  myDebug(( "QBfbData::QBfbData: Crc computed from packet is %04x", Q_UINT32(crc) ));
#endif
  // note that the crc byteorder is LE where the size seems BE !!
  at( CRCPos + len ) = 0xff&crc;
  at( CRCPos + len + 1 ) = (0xff00&crc) >> 8;
#ifdef DEBUG
  myDebug(( "QBfbData::QBfbData: Crc stored in packet is %02x %02x",
	    Q_UINT32(at( CRCPos + len )), Q_UINT32(at( CRCPos + len +1)) ));
#endif
}

QString
QBfbData::dump() const {
  QString ret = "BFBData: ";
  ret += ( at(TypePos) == (unsigned char)DataFirst ) ? "Data First" : "Data Next ";
  ret += QString().sprintf(" chk: 0x%02X", (unsigned char)at(ChkPos) );
  ret += QString().sprintf(" seq: 0x%02X", (unsigned char)at(SeqPos) );
  ret += QString().sprintf(" size: 0x%04X (%d)", (unsigned int)userDataSize(), (unsigned int)userDataSize() );
  ret += crcOk() ? " CRC ok\n<ascii>" : " CRC error\n<ascii>";
  
  for ( unsigned int i=5; i < size()-2; ++i) {
    Q_UINT8 c = at(i);
    if ( isprint( c ) )
      ret += QChar( c );
    else if ( c == '\r' )
      ret += "<CR>";
    else if ( c == '\n' )
      ret += "<LF>";
    else
      ret += ".";
  }
  ret += "</ascii>\n<hex>";
  for ( unsigned int i=5; i < size()-2; ++i)
    ret += QString().sprintf("<%02X>", (unsigned char)at(i) );
  ret += "</hex>";
  return ret;
}

