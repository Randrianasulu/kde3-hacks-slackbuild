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

#include <qcstring.h>

#include "qobexlengthvaluebase.h"
#include "qobexapparam.h"

QObexApparam::QObexApparam() {
}
 
QObexApparam::QObexApparam( const QByteArray& data )
  : QObexLengthValueBase( data ) {
}

void QObexApparam::addParam( Q_UINT8 tag, const QByteArray& value ) {
  appendTag( tag, value );
}

bool QObexApparam::hasParam( Q_UINT8 tag ) const {
  return hasTag( tag );
}

const QByteArray QObexApparam::getParam( Q_UINT8 tag ) const {
  return getTag( tag );
}

QString QObexApparam::toString( int indent ) const {
  QString indentStr;
  indentStr.fill( QChar( ' ' ), indent );
  QString str;
  QTextStream stream( &str, IO_WriteOnly );

  for ( Q_ULONG i = 0; i < 256; ++i ) {
    if ( hasParam( i ) ) {
      stream << indentStr
	     << QString().sprintf("Apparam Tag %02X\n", (unsigned char)i );

      const QByteArray data = getParam( i );
      indentStr += "   ";
      stream << indentStr << "<ascii>";
      for ( Q_ULONG i = 0; i < data.size(); ++i ) {
	Q_UINT8 c = data[i];
	if ( isprint( c ) )
	  stream << QChar( c );
	else if ( c == '\r' )
	  stream << "<CR>";
	else if ( c == '\n' )
	  stream << "<LF>";
	else
	  stream << ".";
      }
      stream << "</ascii>\n";
      stream << indentStr << "<hex>";
      for ( Q_ULONG i = 0; i < data.size(); ++i )
	stream << QString().sprintf("<%02X>", (unsigned char)data[i] );
      stream << "</hex>\n";
      indentStr.truncate( indent );
    }
  }

  return str;
}
