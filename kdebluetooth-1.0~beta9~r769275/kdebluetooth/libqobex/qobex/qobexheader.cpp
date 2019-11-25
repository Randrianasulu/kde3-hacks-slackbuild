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
#include <qdatetime.h>
#include <qtextstream.h>

#include "qobexheader.h"
#include "qobexauth.h"
#include "qobexapparam.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexHeader::QObexHeader( const Q_UINT8 id ) : mId( id ) {
  myDebug(( "QObexHeader::QObexHeader( %s )", stringHeaderId().ascii() ));
}

QObexHeader::QObexHeader( const Q_UINT8 id, const QByteArray& data )
  : mId( id ) {
  myDebug(( "QObexHeader::QObexHeader( %s, const QByteArray& )", stringHeaderId().ascii() ));
  if ( dataType() == QObexHeader::Byte ) {
    Q_ASSERT( data.size() == 1 );
  } else if ( dataType() == QObexHeader::DWord ) {
    Q_ASSERT( data.size() == 4 );
  }
  mData = data;
}

QObexHeader::QObexHeader(const Q_UINT8 id, const QString& data)
  : mId(id) {
  myDebug(( "QObexHeader::QObexHeader( %s, const QString& )", stringHeaderId().ascii() ));
  if ( dataType() == QObexHeader::Unicode ) {
    // Zero terminated unicode string in obex(network) byteorder
    if ( !data.isNull() ) {
      const Q_ULONG l = data.length();
      QObexArray tmp( 2 + 2*l );
      for ( Q_ULONG i = 0; i < l; ++i )
	tmp.qchar( 2*i, data[i] );
      tmp.qchar( 2*l, 0 );
      mData = tmp;
    }
  } else if ( dataType() == QObexHeader::ByteSequence ) {
  // The Type header should be null terminated. 
    if (id == QObexHeader::Type) {
      mData.duplicate( data.latin1(), data.length()+1 );
    } else {
      mData.duplicate( data.latin1(), data.length());
    }
  } else if ( dataType() == QObexHeader::Byte ) {
    Q_ASSERT( data.length() != 1 );
    mData.resize( 1 );
    mData[0] = data[0].latin1();
  } else
    Q_ASSERT( false );
}

QObexHeader::QObexHeader(const Q_UINT8 id, const Q_UINT32 data) : mId(id) {
  myDebug(( "QObexHeader::QObexHeader( %s, %ud )", stringHeaderId().ascii(), data ));
  if ( dataType() == QObexHeader::Byte ) {
    mData.resize( 1 );
    mData.uint8( 0, data );
  } else if ( dataType() == QObexHeader::DWord ) {
    mData.resize( 4 );
    mData.uint32( 0, data );
  } else
    Q_ASSERT( false );
}

QObexHeader::QObexHeader(const Q_UINT8 id, const Q_UINT8 data) : mId(id) {
  myDebug(( "QObexHeader::QObexHeader( %s, %ud )", stringHeaderId().ascii(), data ));
  if ( dataType() == QObexHeader::Byte ) {
    mData.resize( 1 );
    mData.uint8( 0, data );
  } else if ( dataType() == QObexHeader::DWord ) {
    mData.resize( 4 );
    mData.uint32( 0, data );
  } else
    Q_ASSERT( false );
}

QObexHeader::QObexHeader(const Q_UINT8 id, const QDateTime& date) : mId(id) {
  myDebug(( "QObexHeader::QObexHeader( %s, const QDateTime& )", stringHeaderId().ascii() ));
  if ( headerId() == QObexHeader::Time ) {
    QString str = date.toString( Qt::ISODate );
    str.replace( "-", "" );
    str.replace( ":", "" );
    mData.duplicate( str.latin1(), str.length() );
  } else if ( headerId() == QObexHeader::TimeCompat ) {
    Q_UINT32 tt = date.toTime_t();
    mData.resize( 4 );
    mData.uint32( 0, tt );
  } else
    Q_ASSERT( false );
}

Q_UINT16
QObexHeader::length() const {
  myDebug(( "QObexHeader::length() const" ));
  if ( dataType() == QObexHeader::Byte )
    return 2;
  else if ( dataType() == QObexHeader::DWord )
    return 5;
  else
    return 3 + mData.size();
}

QString QObexHeader::stringHeaderId() const {
  switch ( mId ) {
  case Count:
    return "Count";
  case Name:
    return "Name";
  case Type:
    return "Type";
  case Length:
    return "Length";
  case Time:
    return "Time";
  case TimeCompat:
    return "TimeCompat";
  case Description:
    return "Description";
  case Target:
    return "Target";
  case HTTP:
    return "HTTP";
  case Body:
    return "Body";
  case BodyEnd:
    return "BodyEnd";
  case Who:
    return "Who";
  case ConnectionId:
    return "ConnectionId";
  case AppParameters:
    return "AppParameters";
  case AuthChallenge:
    return "AuthChallenge";
  case AuthResponse:
    return "AuthResponse";
  case CreatorId:
    return "CreatorId";
  case WanUUID:
    return "WanUUID";
  case ObjectClass:
    return "ObjectClass";
  case SessParameters:
    return "SessParameters";
  case SessSequence:
    return "SessSequence";
  default:
    return "Invalid";
  }
}

QString
QObexHeader::stringData() const {
  myDebug(( "QObexHeader::stringData() const: id is %s", stringHeaderId().ascii() ));
  if ( headerId() == QObexHeader::Invalid )
    return QString::null;
  else {
    if ( dataType() == QObexHeader::Unicode ) {
      if ( 0 < mData.size() ) {
	QString ret = "";
	Q_ULONG l = mData.size()/2 - 1;
	for ( Q_ULONG i = 0; i < l; ++i )
	  ret[i] = mData.qchar( 2*i );
	return ret;
      } else
	return QString::null;
    } else if ( dataType() == QObexHeader::ByteSequence ) {
      if ( 0 < mData.size() )
	return QString::fromLatin1( mData.data(), mData.size() );
      else
	return QString::null;
    } else if ( dataType() == QObexHeader::DWord ) {
      return QString::number( mData.uint32( 0 ) );
    } else {
      return QString::number( mData.uint8( 0 ) );
    }
  }
}

Q_UINT32
QObexHeader::uint32Data() const {
  myDebug(( "QObexHeader::uint32Data() const: id is %s", stringHeaderId().ascii() ));
  if ( headerId() == QObexHeader::Invalid )
    return 0;
  else {
    if ( dataType() == QObexHeader::DWord )
      return mData.uint32( 0 );
    else if ( dataType() == QObexHeader::Byte )
      return mData.uint8( 0 );
    else {
      Q_ASSERT( true );
      return 0;
    }
  }
}

Q_UINT8
QObexHeader::uint8Data() const {
  myDebug(( "QObexHeader::uint8Data() const: id is %s", stringHeaderId().ascii() ));
  if ( headerId() == QObexHeader::Invalid )
    return 0;
  else {
    if ( dataType() == QObexHeader::DWord )
      return mData.uint32( 0 );
    else if ( dataType() == QObexHeader::Byte )
      return mData.uint8( 0 );
    else {
      Q_ASSERT( false );
      return 0;
    }
  }
}

QDateTime QObexHeader::timeData() const {
  myDebug(( "QObexHeader::timeData() const: id is %s", stringHeaderId().ascii() ));
  QDateTime ret;
  if ( headerId() == QObexHeader::Time ) {
    QString str = stringData();
    str.insert(13, ':');
    str.insert(11, ':');
    str.insert(6, '-');
    str.insert(4, '-');
    ret.fromString( str, Qt::ISODate );
  } else if ( headerId() == QObexHeader::TimeCompat )
    ret.setTime_t( uint32Data() );

  return ret;
}

QByteArray QObexHeader::coreHeader() const {
  myDebug(( "QObexHeader::coreHeader() const: id is %s", stringHeaderId().ascii() ));
  if ( dataType() == QObexHeader::Byte ) {
    QObexArray ret( 2 );
    ret.uint8( 0, mId );
    ret.uint8( 1, mData.at( 0 ) );
    return ret;
  } else if ( dataType() == QObexHeader::DWord ) {
    QObexArray ret( 5 );
    ret.uint8( 0, mId );
    ::memcpy( ret.data()+1, mData.data(), 4 );
    return ret;
  } else {
    QObexArray ret( 3 );
    ret.uint8( 0, mId );
    ret.uint16( 1, 3 + mData.size() );
    return ret;
  }
}

QString QObexHeader::toString( unsigned indent, bool fullContent ) const {
  QString indentStr;
  indentStr.fill( QChar( ' ' ), indent );
  QString str;
  QTextStream stream( &str, IO_WriteOnly );
  
  stream << indentStr << stringHeaderId();
  if ( dataType() == QObexHeader::ByteSequence ) {
    stream << " (Byte Sequence)\n";
    if ( headerId() == AppParameters ) {
      QObexApparam ap( mData );
      stream << ap.toString( indent + 3 );
    } else if ( headerId() == AuthChallenge ) {
      QObexAuthDigestChallenge ch( mData );
      stream << ch.toString( indent + 3 );
    } else if ( headerId() == AuthResponse ) {
      QObexAuthDigestResponse ar( mData );
      stream << ar.toString( indent + 3 );
    } else {
      indentStr += "   ";
      Q_ULONG len = mData.size();
      if ( ( headerId() == Body || headerId() == BodyEnd ) && !fullContent ) {
	// If we have a body header and we should not print the full content
	// clip the display size to a few chars.
	if ( 80 - (indent + 3 + 7 + 8) < len )
	  len =  80 - (indent + 3 + 7 + 8);
      }

      // finally dump ...
      stream << indentStr << "<ascii>";
      for ( Q_ULONG i = 0; i < len; ++i ) {
	Q_UINT8 c = mData[i];
	if ( isprint( c ) )
	  stream << QChar( c );
	else if ( c == '\r' )
	  stream << "<CR>";
	else if ( c == '\n' )
	  stream << "<LF>";
	else
	  stream << ".";
      }
      if ( len == mData.size() )
	stream << "</ascii>\n";
      else
	stream << " ...\n";

      if ( ( headerId() == Body || headerId() == BodyEnd ) && !fullContent ) {
	// If we have a body header and we should not print the full content
	// clip the display size to a few chars.
	if ( 80 - (indent + 3 + 5 + 6) < 4*len )
	  len = ( 80 - (indent + 3 + 5 + 6) )/4;
      }
      stream << indentStr << "<hex>";
      for ( Q_ULONG i = 0; i < len; ++i )
	stream << QString().sprintf("<%02X>", (unsigned char)mData[i] );
      if ( len == mData.size() )
	stream << "</hex>\n";
      else
	stream << " ...\n";
    }
  } else if ( dataType() == QObexHeader::Unicode ) {
    if ( dataType() == QObexHeader::Unicode )
      stream << " (Unicode)\n";
    stream << indentStr << "   \"" << stringData() << "\"\n";
  } else {
    stream << (dataType() == QObexHeader::Byte ? " (UINT8)\n" : " (UINT32)\n");
    // stringData does "the right thing" also for uint values
    stream << indentStr << "   " << stringData() << "\n";
  }

  return str;
}
