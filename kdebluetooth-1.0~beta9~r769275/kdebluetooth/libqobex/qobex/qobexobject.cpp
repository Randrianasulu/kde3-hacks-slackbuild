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

#include <qstring.h>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qtextstream.h>

#include "qobexheader.h"
#include "qobexobject.h"


#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexObject::QObexObject( Q_UINT8 code, bool final ) {
  mValid = true;
  mCode = final ? ( code | FinalBit ) : ( code & ~FinalBit );
  mPacketType = NormalPacket;
  myDebug(( "QObexObject::QObexObject( %s, %d )", stringCode().ascii(), final ));
}

QObexObject::QObexObject( Q_UINT8 code, Q_UINT8 version, Q_UINT8 flags, Q_UINT16 mtu ) {
  mValid = true;
  mCode = code | FinalBit;
  mVersion = version;
  mFlags = flags;
  mMtu = mtu;
  mPacketType = ConnectPacket;
  myDebug(( "QObexObject::QObexObject( %s, %02x, %02x, %04x )",
	    stringCode().ascii(), version, flags, mtu ));
}

QObexObject::QObexObject( Q_UINT8 code, Q_UINT8 flags, Q_UINT8 constants ) {
  mValid = true;
  mCode = code | FinalBit;
  mFlags = flags;
  mConstants = constants;
  mPacketType = SetPathPacket;
  myDebug(( "QObexObject::QObexObject( %s, %02x, %02x )", stringCode().ascii(),
	    flags, constants ));
}

QObexObject::~QObexObject() {
  myDebug(( "QObexObject::~QObexObject()" ));
}

Q_UINT16 QObexObject::size() const {
  myDebug(( "QObexObject::size()" ));
  return coreHeaderSize() + headerSize();
}

void QObexObject::addHeader( const QObexHeader& h ) {
  myDebug(( "QObexObject::addHeader( ... )" ));

  // Body headers are always at the end.
  if ( mHeaders.isEmpty() )
    mHeaders.append( h );
  else {
//     const QObexHeader& first = mHeaders.first();
    const QObexHeader& last = mHeaders.last();

    // Connection id is always the first header.
    if ( h.headerId() == QObexHeader::ConnectionId
	 || h.headerId() == QObexHeader::AuthChallenge ) 
      mHeaders.prepend( h );
    // Body is always the last one
    else if ( last.headerId() == QObexHeader::Body
	      || last.headerId() == QObexHeader::BodyEnd )
      mHeaders.insert( mHeaders.fromLast(), h );
    else
      mHeaders.append( h );
  }
}

void QObexObject::appendHeader( const QObexHeader& h ) {
  myDebug(( "QObexObject::appendHeader( ... )" ));
  mHeaders.append( h );
}

void QObexObject::appendHeaders( const QValueList<QObexHeader>& headers ) {
  myDebug(( "QObexObject::appendHeader( ... )" ));
  mHeaders += headers;
}

void QObexObject::prependHeaders( const QValueList<QObexHeader>& headers ) {
  myDebug(( "QObexObject::prependHeader( ... )" ));
  mHeaders = headers + mHeaders;
}

void QObexObject::removeHeader( Q_UINT8 hid ) {
  myDebug(( "QObexObject::removeHeader( ... )" ));
  QValueList<QObexHeader>::Iterator it;
  for ( it = mHeaders.begin(); it != mHeaders.end(); ) {
    if ( (*it).headerId() == hid )
      it = mHeaders.remove( it );
    else
      ++it;
  }
}

QValueList<QObexHeader> QObexObject::getHeaders() const {
  myDebug(( "QObexObject::getHeader()" ));
  return mHeaders;
}
  
QValueList<QObexHeader> QObexObject::getHeaders( Q_UINT8 hid ) const {
  myDebug(( "QObexObject::getHeaders( ... )" ));
  QValueList<QObexHeader> ret;
  QValueList<QObexHeader>::ConstIterator cit;
  for ( cit = mHeaders.begin(); cit != mHeaders.end(); ++cit )
    if ( (*cit).headerId() == hid )
      ret.append( *cit );
  return ret;
}

QObexHeader QObexObject::getHeader( Q_UINT8 hid ) const {
  myDebug(( "QObexObject::getHeader( ... )" ));
  QValueList<QObexHeader>::ConstIterator it;
  myDebug(( " Searching for header with id %02x", hid ));
  for ( it = mHeaders.begin(); it != mHeaders.end(); ++it ) {
    myDebug(( " Trying header with id %02x", (*it).headerId() ));
    if ( (*it).headerId() == hid )
      return (*it);
  }
  return QObexHeader();
}

QValueList<QByteArray> QObexObject::getBodyData() const {
  QValueList<QByteArray> ret;
  QValueList<QObexHeader>::ConstIterator cit;
  for ( cit = mHeaders.begin(); cit != mHeaders.end(); ++cit )
    if ( (*cit).headerId() == QObexHeader::Body || (*cit).headerId() == QObexHeader::BodyEnd )
      ret.append( (*cit).arrayData() );
  return ret;
}

bool QObexObject::hasHeader( Q_UINT8 hid ) const {
  myDebug(( "QObexObject::hasHeader( ... )" ));
  QValueList<QObexHeader>::ConstIterator it;
  for ( it = mHeaders.begin(); it != mHeaders.end(); ++it ) {
    if ( (*it).headerId() == hid )
      return true;
  }
  return false;
}

QString QObexObject::stringCode() const {
  QString ret;
  ret = stringCode( code() );

  if ( finalBit() )
    ret += ", final bit set";

  return ret;

}

QString QObexObject::stringCode(Q_UINT8 code) {
  QString ret;

  switch ( code ) {
  case Connect:
    ret = "Connect";
    break;
  case Disconnect:
    ret = "Disconnect";
    break;
  case Put:
    ret = "Put";
    break;
  case Get:
    ret = "Get";
    break;
  case SetPath:
    ret = "SetPath";
    break;
  case Session:
    ret = "Session";
    break;
  case Abort:
    ret = "Abort";
    break;
  case Continue:
    ret = "Continue";
    break;
  case Success:
    ret = "Success";
    break;
  case Created:
    ret = "Created";
    break;
  case Accepted:
    ret = "Accepted";
    break;
  case NonAuthoritativeInfo:
    ret = "Non-Authoritative Information";
    break;
  case NoContent:
    ret = "No Content";
    break;
  case ResetContent:
    ret = "Reset Content";
    break;
  case PartialContent:
    ret = "Partial Content";
    break;
  case MultipleChoices:
    ret = "Multiple Choices";
    break;
  case MovedPermanently:
    ret = "Moved Permanently";
    break;
  case MovedTemporarily:
    ret = "Moved Temporarily";
    break;
  case SeeOther:
    ret = "See Other";
    break;
  case NotModified:
    ret = "Not Modified";
    break;
  case UseProxy:
    ret = "Use Proxy";
    break;
  case BadRequest:
    ret = "Bad Request";
    break;
  case Unauthorized:
    ret = "Unauthorized";
    break;
  case PaymentRequired:
    ret = "Payment Required";
    break;
  case Forbidden:
    ret = "Forbidden - operation is understood but refused";
    break;
  case NotFound:
    ret = "Not Found";
    break;
  case MethodNotAllowed:
    ret = "Method Not Allowed";
    break;
  case NotAcceptable:
    ret = "Not Acceptable";
    break;
  case ProxyAuthRequired:
    ret = "Proxy Authentication Required";
    break;
  case RequestTimeOut:
    ret = "Request Time Out";
    break;
  case Conflict:
    ret = "Conflict";
    break;
  case Gone:
    ret = "Gone";
    break;
  case LengthRequired:
    ret = "Length Required";
    break;
  case PreconditionFailed:
    ret = "Precondition Failed";
    break;
  case RequestEntityTooLarge:
    ret = "Request Entity Too Large";
    break;
  case RequestURLTooLarge:
    ret = "Request URL Too Large";
    break;
  case UnsupportedMediaType:
    ret = "Unsupported Media Type";
    break;
  case InternalServerError:
    ret = "Internal Server Error";
    break;
  case NotImplemented:
    ret = "Not Implemented";
    break;
  case BadGateway:
    ret = "Bad Gateway";
    break;
  case ServiceUnavaliable:
    ret = "Service Unavaliable";
    break;
  case GatewayTimeout:
    ret = "Gateway Timeout";
    break;
  case HTTPVersionNotSupported:
    ret = "HTTP Version Not Supported";
    break;
  case DatabaseFull:
    ret = "Database Full";
    break;
  case DatabaseLocked:
    ret = "Database Locked";
    break;
  default:
    ret = "Unknown response code";
    break;
  }

  return ret;
}

void QObexObject::setFlags( Q_UINT8 flags ) {
  Q_ASSERT( mPacketType != NormalPacket );
  mFlags = flags;
}

Q_UINT8 QObexObject::getFlags() const {
  Q_ASSERT( mPacketType != NormalPacket );
  return mFlags;
}

void QObexObject::setVersion( Q_UINT8 version ) {
  Q_ASSERT( mPacketType == ConnectPacket );
  mVersion = version;
}

Q_UINT8 QObexObject::getVersion() const {
  Q_ASSERT( mPacketType == ConnectPacket );
  return mVersion;
}

void QObexObject::setMtu( Q_UINT16 mtu ) {
  Q_ASSERT( mPacketType == ConnectPacket );
  mMtu = mtu;
}

Q_UINT16 QObexObject::getMtu() const {
  Q_ASSERT( mPacketType == ConnectPacket );
  return mMtu;
}

void QObexObject::setConstants( Q_UINT8 constants ) {
  Q_ASSERT( mPacketType == SetPathPacket );
  mConstants = constants;
}

Q_UINT8 QObexObject::getConstants() const {
  Q_ASSERT( mPacketType == SetPathPacket );
  return mConstants;
}

Q_UINT16 QObexObject::headerSize() const {
  Q_UINT16 l = 0;
  QValueList<QObexHeader>::ConstIterator it;
  for ( it = mHeaders.begin(); it != mHeaders.end(); ++it )
    l += (*it).length();
  return l;
}

QString QObexObject::toString( unsigned indent, bool fullContent ) const {
  QString indentStr;
  indentStr.fill( QChar( ' ' ), indent );
  QString str;
  QTextStream stream( &str, IO_WriteOnly );
  
  stream << indentStr << "OBEX Packet: " << stringCode() << endl;
  stream << indentStr << "   code = "
	 << QString().sprintf("0x%02X", (unsigned char)getRawCode() )
	 << ", len = " << size() << endl;
  switch ( getPacketType() ) {
  case QObexObject::ConnectPacket:
    stream << indentStr << "   version = "
	   << QString().sprintf("0x%02X", (unsigned char)getVersion())
	   << ", flags = "
	   << QString().sprintf("0x%02X", (unsigned char)getFlags())
	   << ", mtu = " << getMtu() << endl;
    break;
  case QObexObject::SetPathPacket:
    stream << indentStr << "   flags = "
	   << QString().sprintf("0x%02X", (unsigned char)getFlags())
	   << ", constants = " << getConstants() << endl;
    break;
  default:
    break;
  }

  QValueList<QObexHeader>::ConstIterator it;
  for ( it = mHeaders.begin(); it != mHeaders.end(); ++it )
    stream << (*it).toString( indent + 3, fullContent );

  return str;
}

Q_UINT16 QObexObject::coreHeaderSize() const {
  myDebug(( "QObexObject::coreHeaderSize()" ));
  if ( code() == Connect || mPacketType == ConnectPacket )
    return 7;
  // Fixme || is an error here ...
  else if ( code() == SetPath || mPacketType == SetPathPacket )
    return 5;
  else
    return 3;
}

