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

#ifndef QOBEX_HEADER_H
#define QOBEX_HEADER_H

#include <qcstring.h>
#include <qstring.h>
#include "qobexarray.h"

class QString;
class QDateTime;

class QObexHeader {
public:
  enum {
    // The header datatypes.
    Unicode        = 0x00,
    ByteSequence   = 0x40,
    Byte           = 0x80,
    DWord          = 0xc0,

    // The headervalues, here you can see how they are represented
    Count          = 0x00 | DWord,
    Name           = 0x01 | Unicode,
    Type           = 0x02 | ByteSequence,
    Length         = 0x03 | DWord,
    Time           = 0x04 | ByteSequence,
    TimeCompat     = 0x04 | DWord,
    Description    = 0x05 | Unicode,
    Target         = 0x06 | ByteSequence,
    HTTP           = 0x07 | ByteSequence,
    Body           = 0x08 | ByteSequence,
    BodyEnd        = 0x09 | ByteSequence,
    Who            = 0x0a | ByteSequence,
    ConnectionId   = 0x0b | DWord,
    AppParameters  = 0x0c | ByteSequence,
    AuthChallenge  = 0x0d | ByteSequence,
    AuthResponse   = 0x0e | ByteSequence,
    CreatorId      = 0x0f | DWord,
    WanUUID        = 0x10 | ByteSequence,
    ObjectClass    = 0x11 | ByteSequence,
    SessParameters = 0x12 | ByteSequence,
    SessSequence   = 0x13 | Byte,
    
    Invalid
  };

  /**
     Creates an invalid obex header.
  */
  QObexHeader() : mId( Invalid ) {}

  /**
     Creates an obex header of type t.
  */
  QObexHeader( const Q_UINT8 );

  /**
     Creates an obex header of type t with data in the @ref QByteArray.
     No data or byteorder conversion is done here. The array contained in the
     header is not detached from the given array. Use @ref detach() if you
     want to make the data inside the header independent of the original array.
     If the headertype given does not match in any way with the given data
     an invalid header is created.
  */
  QObexHeader( const Q_UINT8, const QByteArray& );

  /**
     Creates an obex header of type t with data in the @ref QString.
     The string is converted to the data format used for the specified
     header type.
     If the headertype given does not match in any way with the given data
     an invalid header is created.
  */
  QObexHeader( const Q_UINT8, const QString& );

  /**
     Creates an obex header of type t with data in the @ref Q_UINT32.
     Byteorder conversion is done by QObexHeader.
     If the headertype given does not match in any way with the given data
     an invalid header is created.
  */
  QObexHeader( const Q_UINT8, const Q_UINT32 );

  /**
     Creates an obex header of type t with data in the @ref Q_UINT8.
     If the headertype given does not match in any way with the given data
     an invalid header is created.
  */
  QObexHeader( const Q_UINT8, const Q_UINT8 );

  /**
     Creates an obex header of type t with data in the @ref QDateTime.
     If the headertype given does not match in any way with the given data
     an invalid header is created.
  */
  QObexHeader( const Q_UINT8, const QDateTime& );

  /**
     Returns the total length of this header including all fields.
  */
  Q_UINT16 length() const;
  
  /**
     Returns the header id. This is the first 8 bits of the header.
  */
  Q_UINT8 headerId() const { return mId; }

  /**
     Returns a human readable string description for the header id.
  */
  QString stringHeaderId() const;

  /**
     Returns the data type of this header. It can be either
     @ref Unicode, @ref ByteSequence, @ref Byte or @ref DWord.
  */
  Q_UINT8 dataType() const { return mId & 0xc0; }

  /**
     Returns the data type for a header with id @ref id. It can be either
     @ref Unicode, @ref ByteSequence, @ref Byte or @ref DWord.
  */
  static Q_LONG dataType( Q_LONG id ) { return id & 0xc0; }

  /**
     Returns if the header data is empty.
  */
  bool isEmpty() { return 0 == mData.size(); }

  /**
     Detaches the QByteArray inside the header from other QByteArrays
     referencing the same data.
  */
  void detach() { mData.detach(); }

  /**
     Sets the array data to @ref data. 
     No data or byteorder conversion is done here. The array contained in the
     header is not detached from the given array. Use @ref detach() if you
     want to make the data inside the header independent of the original array.
  */
  void setArrayData( const QByteArray& data ) { mData = data; }

  /**
     Returns the array data of the header. 
     No data or byteorder conversion is done here. The array contained in the
     header references the same array than the returned one.
     Use @ref detach() if you want to make the data inside the header
     independent of the original array.
  */
  QByteArray arrayData() const { return mData; }

  /**
     Returns the string data of the header.
     Data type conversion is done here, even for integer types.
  */
  QString stringData() const;

  /**
     Returns the uint32 data of the header.
  */
  Q_UINT32 uint32Data() const;

  /**
     Returns the uint8 data of the header.
  */
  Q_UINT8 uint8Data() const;

  /**
     Returns the QDateTime data of the header.
  */
  QDateTime timeData() const;

  /**
     Returns if the Header is valid.
  */
  bool isValid() const { return mId != Invalid; }

  /**
     Returns the core part of the header in obex(network) byteorder.
  */
  QByteArray coreHeader() const;

  /**
     Returns true if the core header returned by @ref coreHeader() does not
     contain the whole data.
  */
  bool hasArrayData() const {
    return dataType() == Unicode || dataType() == ByteSequence;
  }

  QString toString( unsigned indent = 0, bool fullContent = false ) const;
private:
  /**
     The header id of the OBEX header. This is the first 8 bits of the header.
  */
  Q_UINT8 mId;
  
  /**
     The data contained in the header. This data will always be in raw form
     ready to send.
  */
  QObexArray mData;
};

#endif
