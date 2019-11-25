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

#ifndef QOBEXLENGTHVALUEBASE_H
#define QOBEXLENGTHVALUEBASE_H

#include <qcstring.h>
#include <qmap.h>

class QObexLengthValueBase {
public:
  /**
     Creates a new apparam object.
   */
  QObexLengthValueBase() {}

  /**
     Creates a new apparam object from the data given in the argument.
   */
  QObexLengthValueBase( const QByteArray& );

  /**
     Returns the raw apparam data for use in in header creation.
   */
  operator QByteArray () const;

protected:

  /**
     Add a new apparam value with tag @param tag and data @param value
   */
  void appendTag( Q_UINT8 tag, const QByteArray& value ) {
    mParams[ tag ] = value;
  }

  /**
     Returns true if apparam data contains tag @param tag
   */
  bool hasTag( Q_UINT8 tag ) const {
    return mParams.contains( tag );
  }

  /**
     Returns apparam data for tag @param tag
   */
  const QByteArray getTag( Q_UINT8 tag ) const {
    return mParams[ tag ];
  }

private:
  /**
     Holds the apparam data indexed by tag.
   */
  QMap<Q_UINT8,QByteArray> mParams;
};

#endif
