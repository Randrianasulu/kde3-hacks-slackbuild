/*
    This file is part of libqobex.

    Copyright (C) 2000 Stefan Westerfeld stefan@space.twc.de
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <qstring.h>
#include <qcstring.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include "qobexlengthvaluebase.h"
#include "qobexarray.h"
#include "qobexauth.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

namespace {
  /*
    Yet an other md5 sum implementation. The first cut was copied from arts
    in the KDE cvs tree. Then modified it to fit here.
    If you find libqobex as part of kde, note that we don't use kde's md5
    routine since libqobex should also work without kde.
  */

  /*
   * I used the central part of Colin Plumb's public domain MD5 implementation
   */
  
  /*
   * This code implements the MD5 message-digest algorithm.
   * The algorithm is due to Ron Rivest.	This code was
   * written by Colin Plumb in 1993, no copyright is claimed.
   * This code is in the public domain; do with it what you wish.
   *
   * Equivalent code is available from RSA Data Security, Inc.
   * This code has been tested against that, and is equivalent,
   * except that you don't need to include two pages of legalese
   * with every copy.
   */
  
  
  /* The four core functions - F1 is optimized somewhat */
  
  /* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
  
  /* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )
  
  /*
   * The core of the MD5 algorithm, this alters an existing MD5 hash to
   * reflect the addition of 16 longwords of new data.  MD5Update blocks
   * the data and converts bytes into longwords for this routine.
   */
  void MD5Transform(Q_UINT32 buf[4], Q_UINT32 in[16])
  {
    myDebug(( "MD5Transform()" ));
    Q_UINT32 a = buf[0];
    Q_UINT32 b = buf[1];
    Q_UINT32 c = buf[2];
    Q_UINT32 d = buf[3];
    
    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
  }
#undef F1
#undef F2
#undef F3
#undef F4
#undef MD5STEP

  QByteArray QObexMD5( const QByteArray& data ) {
    myDebug(( "QObexMD5( ... )" ));
    QByteArray md5sum(16);
    unsigned long finalsize = data.size()+1; /* in bytes */
    unsigned int i = 0, j = 0;
    unsigned char w = '\0';
    Q_UINT32 buffer[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    Q_UINT32 in[16];
    
    while((finalsize & 63) != 56) finalsize++;
    finalsize += 8;
    
    for( i=0, j=0; i<finalsize; i++ )
      {
	if(i < data.size()) {
	  w = data[i];
	} else if(i == data.size()) {
	  w = 0x80;
	} else if((finalsize-i > 8) || (finalsize-i <= 4)) {
	  w = 0;
	} else {
	  /* well, the length thing encoded in here will only
	     work until 2^32 bits (though md5 would support 2^64
	     bits) */
	  w = ((data.size()*8) >> ((i+8-finalsize)*8)) & 0xff;
	}
	switch(i & 3) {
	case 0: in[j]  = w;
	  break;
	case 1: in[j] |= w << 8;
	  break;
	case 2: in[j] |= w << 16;
	  break;
	case 3: in[j] |= w << 24;
	  j++;
	  if(j == 16) {
	    MD5Transform(buffer,in);
	    j = 0;
	  }
	  break;
	}
      }
    
    for( i=0, j=0;j<4;j++)
      {
	md5sum[i++] = buffer[j] & 0xff;
	md5sum[i++] = (buffer[j] >> 8) & 0xff;
	md5sum[i++] = (buffer[j] >> 16) & 0xff;
	md5sum[i++] = (buffer[j] >> 24) & 0xff;
      }
    
    return md5sum;
  }
}

QObexAuthDigestBase::text_encoding QObexAuthDigestBase::encoding_list[] = {
  { "ISO-8859-1", QObexAuthDigestBase::Realm_ISO_8859_1 },
  { "ISO-8859-2", QObexAuthDigestBase::Realm_ISO_8859_2 },
  { "ISO-8859-3", QObexAuthDigestBase::Realm_ISO_8859_3 },
  { "ISO-8859-4", QObexAuthDigestBase::Realm_ISO_8859_4 },
  { "ISO-8859-5", QObexAuthDigestBase::Realm_ISO_8859_5 },
  { "ISO-8859-6", QObexAuthDigestBase::Realm_ISO_8859_6 },
  { "ISO-8859-7", QObexAuthDigestBase::Realm_ISO_8859_7 },
  { "ISO-8859-8", QObexAuthDigestBase::Realm_ISO_8859_8 },
  { "ISO-8859-9", QObexAuthDigestBase::Realm_ISO_8859_9 },
  { "utf-16", QObexAuthDigestBase::Realm_UNICODE },
  // Use latin 1 for ascii. Since latin1 is a superset this should be
  // ok for conversion to QString.
  // For conversion to binary arrays we have to check for real latin-1
  // first and sign that encoding. This way outgoing engodings will
  // never be ascii, but that does't matter ...
  { "ISO-8859-1", QObexAuthDigestBase::Realm_ASCII },
  { 0, 0 }
};

void QObexAuthDigestBase::appendStringValue( Q_UINT8 tag, const QString& s ) {
  QTextCodec* codec = 0;
  int idx;
  Q_UINT8 realm = 0;
  for ( idx = 0; encoding_list[idx].name; ++idx ) {
    codec = QTextCodec::codecForName( encoding_list[idx].name );
    if ( codec && codec->canEncode( s ) ) {
      realm = encoding_list[idx].tag;
      break;
    }
    codec = 0;
  }

  if ( !codec ) {
    // if not found take unicode. This one will fit best ...
    codec = QTextCodec::codecForName ( "utf-16" );
    realm = Realm_UNICODE;
  }

  if ( !codec )
    return;

  QCString encoded = codec->fromUnicode( s );
  encoded = " " + encoded;
  encoded.at( 0 ) = realm;
  appendTag( tag, encoded );
}

QString QObexAuthDigestBase::getStringTag( Q_UINT8 tag ) const {
  QByteArray data = getTag( tag );
  if ( 1 < data.size() ) {
    Q_UINT8 realm = data.at( 0 );
    QTextCodec* codec = 0;
    int idx;
    for ( idx = 0; encoding_list[idx].name; ++idx ) {
      if ( encoding_list[idx].tag == realm ) {
	codec = QTextCodec::codecForName ( encoding_list[idx].name );
	break;
      }
    }

    if ( !codec )
      codec = QTextCodec::codecForContent ( data.data() + 1, data.size() - 1);

    if ( codec )
      return codec->toUnicode( data.data() + 1, data.size() - 1 );
  }
  return QString::null;
}

QByteArray QObexAuthDigestBase::computeDigest( const QByteArray& d1, const QByteArray& d2 ) {
  QByteArray message( 1 + d1.size() + d2.size() );
  ::memcpy( message.data(), d1.data(), d1.size() );
  message[ d1.size() ] = ':';
  ::memcpy( message.data()+d1.size()+1, d2.data(), d2.size() );
  return QObexMD5( message );
}

QByteArray QObexAuthDigestBase::randomNonce() {
  myDebug(( "QObexAuthDigestChallenge::randomNonce()" ));
  QObexArray rnd( 16 );
  rnd.uint32( 0, ::rand() );
  rnd.uint32( 4, ::rand() );
  rnd.uint32( 8, ::rand() );
  rnd.uint32( 12, ::rand() );
  return QObexMD5( rnd );
}

QString QObexAuthDigestBase::toString( const QByteArray& data ) {
  QString str;
  QTextStream stream( &str, IO_WriteOnly );

  for ( Q_ULONG i = 0; i < data.size(); ++i )
    stream << QString().sprintf("%02X", (unsigned char)data[i] );

  return str;
}

// Constructor to create data for a new AuthChallenge header.
QObexAuthDigestChallenge::QObexAuthDigestChallenge( const AuthInfo& ai ) {
  myDebug(( "QObexAuthDigestChallenge::QObexAuthDigestChallenge( ... )" ));

  appendTag( NonceTag, randomNonce() );

  if ( !ai.realm.isNull() )
    appendStringValue( RealmTag, ai.realm );

  if ( 0 < ai.options ) {
    QByteArray data( 1 );
    data.at( 0 ) = ai.options;
    appendTag( OptionsTag, data );
  }
}

// Constructor used to parse the data given here.
QObexAuthDigestChallenge::QObexAuthDigestChallenge( const QByteArray& data )
  : QObexAuthDigestBase( data ) {
  myDebug(( "QObexAuthDigestChallenge::QObexAuthDigestChallenge( ... )" ));
}

// Needed for list handling
QObexAuthDigestChallenge::QObexAuthDigestChallenge() {
  myDebug(( "QObexAuthDigestChallenge::QObexAuthDigestChallenge()" ));
}

// Read access to the fields of the header data.
QByteArray QObexAuthDigestChallenge::nonce() const {
  myDebug(( "QObexAuthDigestChallenge::nonce()" ));
  return getTag( NonceTag );
}

QString QObexAuthDigestChallenge::realm() const {
  myDebug(( "QObexAuthDigestChallenge::realm()" ));
  return getStringTag( RealmTag );
}

bool QObexAuthDigestChallenge::hasRealm() const {
  myDebug(( "QObexAuthDigestChallenge::hasRealm()" ));
  return hasTag( RealmTag );
}

int QObexAuthDigestChallenge::options() const {
  myDebug(( "QObexAuthDigestChallenge::options()" ));
  if ( hasTag( OptionsTag ) ) {
    QObexArray data = getTag( OptionsTag );
    if ( data.size() == 1 )
      return data.uint8( 0 );
    else
      return 0;
  } else
    return 0;
}

bool QObexAuthDigestChallenge::readOnly() const {
  myDebug(( "QObexAuthDigestChallenge::readOnly()" ));
  return options() & AccessReadOnly;
}

bool QObexAuthDigestChallenge::userIdRequired() const {
  myDebug(( "QObexAuthDigestChallenge::userIdRequired()" ));
  return options() & SendUserId;
}

bool QObexAuthDigestChallenge::hasOptions() const {
  myDebug(( "QObexAuthDigestChallenge::hasOptions()" ));
  return hasTag( OptionsTag );
}

QString QObexAuthDigestChallenge::toString( int indent ) const {
  QString indentStr;
  indentStr.fill( QChar( ' ' ), indent );
  QString str;
  QTextStream stream( &str, IO_WriteOnly );

  stream << indentStr << "Nonce:          "
	 << QObexAuthDigestBase::toString( nonce() ) << "\n";
  if ( hasRealm() )
    stream << indentStr << "Realm:          "
	   << realm() << "\n";
  if ( hasOptions() )
    stream << indentStr << "Options:        "
	   << QString().sprintf("0x%02X", (unsigned char)options() ) << " "
	   << ((options() & SendUserId) ? "(Send Userid) " : "" )
	   << ((options() & AccessReadOnly) ? "(ReadOnly) " : "" )
	   << "\n";
  else
    stream << indentStr << "Options:        Not set, defaults to 0x00\n";

  return str;
}

// Constructor to create data for a new AuthResponse header.
QObexAuthDigestResponse::QObexAuthDigestResponse( const QByteArray& nonce, const AuthInfo& ai ) {
  myDebug(( "QObexAuthDigestResponse::QObexAuthDigestResponse()" ));

  Q_ASSERT( !nonce.isEmpty() );
  appendTag( RequestDigestTag, computeDigest( nonce, ai.secret ) );
  appendTag( NonceTag, nonce );

  if ( !ai.userId.isNull() )
    appendStringValue( UserIdTag, ai.userId );
}

// Constructor used to parse the data given here.
QObexAuthDigestResponse::QObexAuthDigestResponse( const QByteArray& data )
  : QObexAuthDigestBase( data ) {
  myDebug(( "QObexAuthDigestResponse::QObexAuthDigestResponse()" ));
}
  
// Needed for list handling
QObexAuthDigestResponse::QObexAuthDigestResponse() {
  myDebug(( "QObexAuthDigestResponse::QObexAuthDigestResponse()" ));
}

// Read access to the fields of the header data.
QByteArray QObexAuthDigestResponse::requestDigest() const {
  myDebug(( "QObexAuthDigestResponse::requestDigest()" ));
  return getTag( RequestDigestTag );
}

QByteArray QObexAuthDigestResponse::nonce() const {
  myDebug(( "QObexAuthDigestResponse::nonce()" ));
  return getTag( NonceTag );
}

bool QObexAuthDigestResponse::hasNonce() const {
  myDebug(( "QObexAuthDigestResponse::hasNonce()" ));
  return hasTag( NonceTag );
}

QString QObexAuthDigestResponse::userId() const {
  myDebug(( "QObexAuthDigestResponse::userId()" ));
  return getStringTag( UserIdTag );
}

bool QObexAuthDigestResponse::hasUserId() const {
  myDebug(( "QObexAuthDigestResponse::hasUserId()" ));
  return hasTag( UserIdTag );
}

bool QObexAuthDigestResponse::authenticate( const QByteArray& secret, const QByteArray& nonce ) const {
  myDebug(( "QObexAuthDigestResponse::authenticate( ... )" ));
  return getTag( RequestDigestTag ) == computeDigest( nonce, secret );
}

QString QObexAuthDigestResponse::toString( int indent ) const {
  QString indentStr;
  indentStr.fill( QChar( ' ' ), indent );
  QString str;
  QTextStream stream( &str, IO_WriteOnly );

  stream << indentStr << "Request Digest: "
	 << QObexAuthDigestBase::toString( requestDigest() ) << "\n";
  if ( hasNonce() )
    stream << indentStr << "Nonce:          "
	   << QObexAuthDigestBase::toString( nonce() ) << "\n";
  if ( hasUserId() )
    stream << indentStr << "UserId:         " << userId() << "\n";

  return str;
}
