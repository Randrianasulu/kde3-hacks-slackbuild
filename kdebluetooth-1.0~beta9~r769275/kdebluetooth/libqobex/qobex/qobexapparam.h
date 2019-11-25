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

#ifndef QOBEXAPPARAM_H
#define QOBEXAPPARAM_H

#include <qcstring.h>
#include "qobexlengthvaluebase.h"

class QObexApparam
 : public QObexLengthValueBase {
public:
  /**
     Creates a new apparam object.
   */
  QObexApparam();

  /**
     Creates a new apparam object from the data given in the argument.
   */
  QObexApparam( const QByteArray& data );

  /**
     Add a new apparam value with tag @tag and data @data
   */
  void addParam( Q_UINT8 tag, const QByteArray& data );
  /**
     Returns true if apparam data contains tag @tag
   */
  bool hasParam( Q_UINT8 tag ) const;

  /**
     Returns apparam data for tag @tag
   */
  const QByteArray getParam( Q_UINT8 tag ) const;

  /**
   */
  QString toString( int indent ) const;
};

#endif
