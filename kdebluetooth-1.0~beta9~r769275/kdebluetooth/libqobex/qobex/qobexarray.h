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

#ifndef QOBEXARRAY_H
#define QOBEXARRAY_H

#include <qstring.h>
#include <qcstring.h>

/**
   QObexArray is basically a bytearray with additional access functions used
   for bigger chunks of data and for specific data. Access functions here are
   byteorder and alignment aware.
 */
class QObexArray
  : public QByteArray {
public:
  inline QObexArray() {}
  inline QObexArray( int size ) : QByteArray( size ) {}
  inline QObexArray( const QByteArray& a ) : QByteArray( a ) {}

  inline Q_UINT8 uint8( Q_ULONG byteoffset ) const {
    return *(Q_UINT8*)(data()+byteoffset);
  }
  inline void uint8( Q_ULONG byteoffset, Q_UINT8 v ) {
    *(Q_UINT8*)(data()+byteoffset) = v;
  }

  inline Q_UINT16 uint16( Q_ULONG byteoffset ) const {
    return ( Q_UINT8( uint8( byteoffset ) ) << 8 ) + uint8( byteoffset+1 );
  }
  inline void uint16( Q_ULONG byteoffset, Q_UINT16 v ) {
    uint8( byteoffset, Q_UINT8( v >> 8 ) );
    uint8( byteoffset+1, Q_UINT8( v ) );
  }

  inline QChar qchar( Q_ULONG byteoffset ) const {
    return QChar( uint8( byteoffset+1 ), uint8( byteoffset ) );
  }
  inline void qchar( Q_ULONG byteoffset, QChar v ) {
    uint8( byteoffset, v.row() );
    uint8( byteoffset+1, v.cell() );
  }

  inline Q_UINT32 uint32( Q_ULONG byteoffset ) const {
    return ( Q_UINT32( uint16( byteoffset ) ) << 16 ) + uint16( byteoffset+2 );
  }
  inline void uint32( Q_ULONG byteoffset, Q_UINT32 v ) {
    uint16( byteoffset, Q_UINT16( v >> 16 ) );
    uint16( byteoffset+2, Q_UINT16( v ) );
  }
};

#endif
