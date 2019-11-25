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

#include <qcstring.h>
#include <qmap.h>
#include <qdatastream.h>

#include "qobexlengthvaluebase.h"

QObexLengthValueBase::QObexLengthValueBase( const QByteArray& data ) {
  QDataStream stream( data, IO_ReadOnly );
  stream.setByteOrder( QDataStream::BigEndian );
  stream.setVersion( 5 );

  while ( !stream.atEnd() ) {
    Q_UINT8 tag;
    stream >> tag;
    QByteArray value;
    Q_UINT8 length;
    stream >> length;
    value.resize( length );
    stream.readRawBytes( value.data(), value.size() );
    mParams[ tag ] = value;
  }
}

QObexLengthValueBase::operator QByteArray () const {
  QByteArray ret;
  QDataStream stream( ret, IO_WriteOnly );
  stream.setByteOrder( QDataStream::BigEndian );
  stream.setVersion( 5 );

  QMap<Q_UINT8,QByteArray>::ConstIterator ci;
  for ( ci = mParams.begin(); ci != mParams.end(); ++ci ) {
    stream << ci.key();
    stream << Q_UINT8( ci.data().size() );
    stream.writeRawBytes( ci.data().data(), ci.data().size() );
  }
  return ret;
}
