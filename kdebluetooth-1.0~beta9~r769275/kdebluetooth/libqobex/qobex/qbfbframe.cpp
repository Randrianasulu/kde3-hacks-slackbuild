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
#include <qcstring.h>

#include "qbfbframe.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

// Creates a new bfb frame by reading up to MTU bytes from data.
QBfbFrame::QBfbFrame( Type t, const char *d, int len ) {
  if ( MTU < len )
    len = MTU;
  
  resize( 3+len );
  at(TypePos) = t;
  at(SizePos) = len;
  at(ChkPos) = computeChk();

  ::memcpy( userData(), d, len );
}

// Creates a new bfb frame by reading up to MTU bytes from data.
QBfbFrame::QBfbFrame( Type t, QByteArray& d ) {
  size_t len = d.size();
  if ( MTU < len )
    len = MTU;
  
  resize( 3+len );
  at(TypePos) = t;
  at(SizePos) = len;
  at(ChkPos) = computeChk();

  ::memcpy( userData(), d.data(), len );
}

QBfbFrame::QBfbFrame( Type t, Q_UINT8 d ) {
  resize( 4 );
  at(TypePos) = t;
  at(SizePos) = 1;
  at(ChkPos) = computeChk();
  at(DataPos) = d;
}

QBfbFrame::QBfbFrame( Type t, Q_UINT8 d0, Q_UINT8 d1 ) {
  resize( 5 );
  at(TypePos) = t;
  at(SizePos) = 2;
  at(ChkPos) = computeChk();
  at(DataPos) = d0;
  at(DataPos+1) = d1;
}

QString
QBfbFrame::typeToQString( Type t ) {
  switch ( t ) {
  case InterfaceChange:      return "Interface Change";
  case Connect:              return "Connect         ";
  case Keypress:             return "Key Press       ";
  case Modem:                return "Modem Command   ";
  case EEPROM:               return "EEPROM          ";
  case Data:                 return "Data            ";
  default: return QString().sprintf("Unknown (0x%02X)  ", (unsigned char)t );
  }
}

QString
QBfbFrame::dump() const {
  QString ret = "BFBFrame: ";
  ret += QBfbFrame::typeToQString( Type(at(TypePos)) );
  ret += QString().sprintf(" size: 0x%02X", (unsigned char)at(SizePos) );
  ret += size() == frameSize() ? "(ok)  " : " (err) ";
  ret += QString().sprintf(" chk: 0x%02X", (unsigned char)at(ChkPos) );
  ret += headerValid() ? " (ok)\n<ascii>" : " (err)\n<ascii>";
  for ( unsigned int i=3; i < size(); ++i) {
    int c = at(i);
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
  for ( unsigned int i=3; i < size(); ++i)
    ret += QString().sprintf("<%02X>", (unsigned char)at(i) );
  ret += "</hex>";
  return ret;
}

// often used frames
const QBfbFrame QBfbFrame::hello(QBfbFrame::Connect, 0x14);
const QBfbFrame QBfbFrame::helloAck(QBfbFrame::Connect, 0x14, 0xAA);
const QBfbFrame QBfbFrame::dataAck(QBfbFrame::Data, 0x1, 0xFE);

QBfbFrame QBfbFrame::ifChange( int speed ) {
  QCString dta;
  dta.sprintf( "\xc0%d\x0d\xd2\x2b", speed );
  return QBfbFrame( QBfbFrame::InterfaceChange, dta.data(), dta.size()-1 );
}

QBfbFrame QBfbFrame::ifChangeAck( int speed ) {
  QCString dta;
  dta.sprintf( "\xcc%d\x0d\xd2\x2b", speed );
//   Q_UINT16 crc = qChecksum( dta.data()+1, 7 );
//   myDebug(( "CRC is %x", crc ));
//   myDebug(( "%s", QBfbFrame( QBfbFrame::InterfaceChange, dta.data(), dta.size()-1 ).dump().ascii() ));
  return QBfbFrame( QBfbFrame::InterfaceChange, dta.data(), dta.size()-1 );
}
