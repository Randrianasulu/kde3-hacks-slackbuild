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

#ifndef QOBEXNULLSERVEROPS_H
#define QOBEXNULLSERVEROPS_H

#include <qcstring.h>
#include <qvaluelist.h>

#include "qobexauth.h"
#include "qobexobject.h"

class QObexNullServerOps
  : public QObexServerOps {
public:
  /**
     Create a new set of QObexNullServerOps.
   */
  QObexNullServerOps();

  /**
     Create a new set of QObexNullServerOps.
   */
  virtual ~QObexNullServerOps();

  /**
     Return true if your implementation is able to handle connections
     with the given uuid.
   */
  virtual bool canHandle( const QByteArray& );

  /**
     Return a new instance of server ops. This is called when a new connection
     request arrives and canHandle returns true.
   */
  virtual QObexServerOps* clone();
};

#endif
