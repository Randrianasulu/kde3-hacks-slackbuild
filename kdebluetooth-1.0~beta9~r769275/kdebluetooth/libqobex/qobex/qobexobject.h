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

#ifndef QOBEX_OBJECT_H
#define QOBEX_OBJECT_H

#include <qcstring.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "qobexheader.h"

class QObexObject {
public:
  enum {
    ProtocolVersion1 = 0x10,
    TransportMultiplexing = 0x1,
    Backup = 0x1,
    NoCreate = 0x2
  };

  enum {
    // The famous final bit ...
    FinalBit                = 0x80,

    // Valid opcodes, some of them have the final bit always set ...
    Connect                 = 0x00 /* | FinalBit */,
    Disconnect              = 0x01 /* | FinalBit */,
    Put                     = 0x02,
    Get                     = 0x03,
    SetPath                 = 0x05 /* | FinalBit */,
    Session                 = 0x07 /* | FinalBit */,
    Abort                   = 0x7f /* | FinalBit */,

    // Valid response codes
    Continue                = 0x10,
    Success                 = 0x20,
    Created                 = 0x21,
    Accepted                = 0x22,
    NonAuthoritativeInfo    = 0x23,
    NoContent               = 0x24,
    ResetContent            = 0x25,
    PartialContent          = 0x26,

    MultipleChoices         = 0x30,
    MovedPermanently        = 0x31,
    MovedTemporarily        = 0x32,
    SeeOther                = 0x33,
    NotModified             = 0x34,
    UseProxy                = 0x35,

    BadRequest              = 0x40,
    Unauthorized            = 0x41,
    PaymentRequired         = 0x42,
    Forbidden               = 0x43,
    NotFound                = 0x44,
    MethodNotAllowed        = 0x45,
    NotAcceptable           = 0x46,
    ProxyAuthRequired       = 0x47,
    RequestTimeOut          = 0x48,
    Conflict                = 0x49,
    Gone                    = 0x4a,
    LengthRequired          = 0x4b,
    PreconditionFailed      = 0x4c,
    RequestEntityTooLarge   = 0x4d,
    RequestURLTooLarge      = 0x4e,
    UnsupportedMediaType    = 0x4f,

    InternalServerError     = 0x50,
    NotImplemented          = 0x51,
    BadGateway              = 0x52,
    ServiceUnavaliable      = 0x53,
    GatewayTimeout          = 0x54,
    HTTPVersionNotSupported = 0x55,

    DatabaseFull            = 0x60,
    DatabaseLocked          = 0x61
  };

  enum PacketType {
    NormalPacket,
    ConnectPacket,
    SetPathPacket
  };

  // FIXME make packet type the first argument
  QObexObject() { mValid = false; mPacketType = NormalPacket; }
  QObexObject( Q_UINT8 code, bool final );
  QObexObject( Q_UINT8 code, Q_UINT8 version, Q_UINT8 flags, Q_UINT16 mtu );
  QObexObject( Q_UINT8 code, Q_UINT8 flags, Q_UINT8 constants );
  // ... needed
  virtual ~QObexObject();

  // Returns the size of the whole packet
  Q_UINT16 size() const;

  // Returns if the packet is valid. When we read a packet from a stream
  // it can happen, that it is invalid in some way ...
  // Cases where specific objects always should have the final bit set
  // and it is not set are not covered.
  bool isValid() const { return mValid; }
  void setValid( bool valid ) { mValid = valid; }

  Q_UINT8 code() const { return mCode & ~FinalBit; }
  void setCode( Q_UINT8 code ) {
    mCode = ( mCode & FinalBit ) | ( code & ~FinalBit );
  }

  // get and set the final bit
  bool finalBit() const { return mCode & FinalBit; }
  void setFinalBit( bool enable ) { if ( enable ) mCode |= FinalBit; else mCode &= ~FinalBit; }

  // Bypasses the final bit handling. Mostly for parsing and writing
  Q_UINT8 getRawCode() const { return mCode; }
  void setRawCode( Q_UINT8 code ) { mCode = code; }

  // returns a (english) human readable representation from the given code
  QString stringCode() const;
  static QString stringCode(Q_UINT8 code);

  void setFlags( Q_UINT8 flags );
  Q_UINT8 getFlags() const;

  void setVersion( Q_UINT8 version );
  Q_UINT8 getVersion() const;

  void setMtu( Q_UINT16 mtu );
  Q_UINT16 getMtu() const;

  void setConstants( Q_UINT8 constants );
  Q_UINT8 getConstants() const;

  void setPacketType( PacketType packetType ) { mPacketType = packetType; }
  PacketType getPacketType() const { return mPacketType; }

  // Add a new header to the object, take care of the header order.
  void addHeader( const QObexHeader& );
  // Append a new header at the end of the header list to the object.
  void appendHeader( const QObexHeader& );
  // Append a list of new headers at the end of the header list to the object.
  void appendHeaders( const QValueList<QObexHeader>& );
  // Prepend a list of new headers before the beginning of the header list to the object.
  void prependHeaders( const QValueList<QObexHeader>& );
  // Removes all headers with the given id.
  void removeHeader( Q_UINT8 );
  // Get all headers.
  QValueList<QObexHeader> getHeaders() const;
  // Get all headers with a given type.
  QValueList<QObexHeader> getHeaders( Q_UINT8 ) const;
  // Get the first header with a given type.
  QObexHeader getHeader( Q_UINT8 ) const;
  // Returns the content of the Body or BodyEnd header withever comes first
  // This is a shortcut for checking both types and using the existing one.
  QValueList<QByteArray> getBodyData() const;
  // Returns true if the object contains a header of given type
  bool hasHeader( Q_UINT8 ) const;

  // Used to dump a packet. Be careful, this may be very slow!!!
  QString toString( unsigned indent = 0, bool fullContent = false ) const;

private:
  // FIXME move them into size ...
  // The size of the packet without ObexHeaders
  Q_UINT16 coreHeaderSize() const;

  // Return the size of all headers together
  Q_UINT16 headerSize() const;

  // The type of packet we have. May be we have a connect packet ...
  // Do we need this??? YES, for sending connect responses ...
  PacketType mPacketType;

  // Signals if the object we represent is valid.
  // Objects we create in memory are always valid but objects read
  // from a stream can be invalid.
  // FIXME put this in packet type ...
  bool mValid;
  // The request/response code for this object.
  Q_UINT8 mCode;

  // Flags for connect and setpath
  Q_UINT8 mFlags;
  // For the connect obex version
  Q_UINT8 mVersion;
  // Connect Mtu
  Q_UINT16 mMtu;
  // Zero field in setpath, here for completeness
  Q_UINT8 mConstants;

  // List of all headers
  QValueList<QObexHeader> mHeaders;
};

#endif
