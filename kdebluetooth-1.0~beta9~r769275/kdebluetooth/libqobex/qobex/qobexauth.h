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

#ifndef QOBEXAUTH_H
#define QOBEXAUTH_H

#include <qcstring.h>

#include "qobexlengthvaluebase.h"

class QObexAuthDigestBase
  : public QObexLengthValueBase {
protected:
  enum {
    Realm_ASCII = 0,
    Realm_ISO_8859_1 = 1,
    Realm_ISO_8859_2 = 2,
    Realm_ISO_8859_3 = 3,
    Realm_ISO_8859_4 = 4,
    Realm_ISO_8859_5 = 5,
    Realm_ISO_8859_6 = 6,
    Realm_ISO_8859_7 = 7,
    Realm_ISO_8859_8 = 8,
    Realm_ISO_8859_9 = 9,
    Realm_UNICODE = 255
  };

  QObexAuthDigestBase( const QByteArray& d ) : QObexLengthValueBase( d ) {}
  QObexAuthDigestBase() {}

  void appendStringValue( Q_UINT8 tag, const QString& s );
  QString getStringTag( Q_UINT8 tag ) const;

  static QByteArray computeDigest( const QByteArray&, const QByteArray& );
  // returns a random bytearray
  static QByteArray randomNonce();
  static QString toString( const QByteArray& );

  struct text_encoding {
    const char* name;
    Q_UINT8 tag;
  };
  static text_encoding encoding_list[];
};

class QObexAuthDigestChallenge
  : public QObexAuthDigestBase {
public:
  enum {
    NonceTag = 0,
    OptionsTag = 1,
    RealmTag = 2,

    SendUserId = 0x1,
    AccessReadOnly = 0x2
  };

  struct AuthInfo {
    QString realm;
    Q_UINT8 options;
    AuthInfo() : options( 0 ) {}
    AuthInfo( const QString& r ) : realm( r ), options( 0 ) {}
    AuthInfo( Q_UINT8 o ) : options( o ) {}
    AuthInfo( const QString& r, Q_UINT8 o ) : realm( r ), options( o ) {}
  };

  // Constructor to create data for a new AuthChallenge header.
  QObexAuthDigestChallenge( const AuthInfo& );
  // Constructor used to parse the data given here.
  QObexAuthDigestChallenge( const QByteArray& );
  // Needed for list handling.
  QObexAuthDigestChallenge();

  // Read access to the fields of the header data.
  QByteArray nonce() const;
  QString realm() const;
  bool hasRealm() const;
  int options() const;
  bool readOnly() const;
  bool userIdRequired() const;
  bool hasOptions() const;

  /**
   */
  QString toString( int indent ) const;
};

class QObexAuthDigestResponse
  : public QObexAuthDigestBase {
public:
  enum {
    RequestDigestTag = 0,
    UserIdTag = 1,
    NonceTag = 2,

    SendUserId = 0x1,
    AccessReadOnly = 0x2
  };

  struct AuthInfo {
    QByteArray secret;
    QString userId;
    AuthInfo() {}
    AuthInfo( const QByteArray& s ) : secret( s ) {}
    AuthInfo( const QString& u ) : userId( u ) {}
    AuthInfo( const QByteArray& s, const QString& u ) : secret( s ), userId( u ) {}
  };

  // Constructor to create data for a new AuthResponse header.
  QObexAuthDigestResponse( const QByteArray&, const AuthInfo& );
  // Constructor used to parse the data given here.
  QObexAuthDigestResponse( const QByteArray& );
  // Needed for list handling.
  QObexAuthDigestResponse();
  
  // Read access to the fields of the header data.
  QByteArray requestDigest() const;
  QByteArray nonce() const;
  bool hasNonce() const;
  QString userId() const;
  bool hasUserId() const;

  bool authenticate( const QByteArray& secret, const QByteArray& nonce ) const;

  /**
   */
  QString toString( int indent ) const;
};

#endif
