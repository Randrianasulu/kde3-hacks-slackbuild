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

#include "qobexserverops.h"
#include "qobexnullserverops.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexNullServerOps::QObexNullServerOps() {
  myDebug(( "QObexNullServerOps::QObexNullServerOps()" ));
}

QObexNullServerOps::~QObexNullServerOps() {
  myDebug(( "QObexNullServerOps::~QObexNullServerOps()" ));
}

bool QObexNullServerOps::canHandle( const QByteArray& uuid ) {
  myDebug(( "QObexNullServerOps::canHandle( ... )" ));
  return uuid.size() == 0;
}

QObexServerOps* QObexNullServerOps::clone() {
  myDebug(( "QObexNullServerOps::clone()" ));
  return new QObexNullServerOps;
}
