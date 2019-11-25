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

#ifndef QOBEXBASE_H
#define QOBEXBASE_H

#include <qobject.h>
#include <qguardedptr.h>

#include "qobextransport.h"
#include "qobexobject.h"

class QObexBase
  : public QObject {
  Q_OBJECT
public:
  QObexBase( QObexTransport*, QObject* = 0, const char* = 0 );
  virtual ~QObexBase();
  
  /**
     Access function for the obexserver and client.
     Sends an QObexObject with the base.
     Returns if it was able to send the object.
     Note, that an object is rejected because is too big
     for the negotiated Mtu.
     It the object is a connect object it is rejected if it
     does not fit the base's Mtu requirements or if it
     is smaller then 255.
  */
  bool sendObject( QObexObject* obj );

  QObexTransport* transport() { return mTransport; }

  /**
     Returns the negotiated MTU of this base.
     It is MinimumMtu before a OBEX connection is made. When the
     base has seen the connect request and response it returns
     negotiated MTU.
   */
  Q_UINT16 getMtu() const { return mMtu; }

  /**
     Returns a valid MTU between MinimumMtu and MaximumMtu
     It is usually a multiple of the underlying transports mtu.
   */
  Q_UINT16 getOptimumMtu() const;

  Q_LONG getOptimumBodySize( Q_UINT16 ) const;

  Q_UINT16 maxBodySize() const { return mMaxBodySize; }
  void setMaxBodySize( Q_UINT16 mbs ) { mMaxBodySize = mbs; }

private slots:
  void slotTransportConnected();
  void slotTransportConnectionClosed();
  void slotTransportPacketRecieved( QObexObject& );
  void slotTransportPacketWritten();
  void slotTransportError( QObexTransport::Error );
  void slotTransportSelectModeChanged( QObexTransport::SelectHint, Q_LONG );

protected:
  virtual void transportConnected() = 0;
  virtual void transportConnectionClosed() = 0;
  virtual void transportPacketRecieved( QObexObject& ) = 0;
  virtual void transportPacketWritten() = 0;
  virtual void transportError( QObexTransport::Error ) = 0;
  virtual void transportSelectModeChanged( QObexTransport::SelectHint, Q_LONG ) = 0;


  /**
     Neat utilities for computing MTUs ...
   */
  static Q_LONG min( Q_LONG a, Q_LONG b ) { return a < b ? a : b ; }
  static Q_LONG max( Q_LONG a, Q_LONG b ) { return a > b ? a : b ; }

protected:
  /**
     Holds our transport.
   */
  QGuardedPtr<QObexTransport> mTransport;

private:
  /**
     Holds the negotiated MTU of the current base.
   */
  Q_UINT16 mMtu;
  Q_UINT16 mMaxBodySize;

  /**
     Disable assignment
  */
  QObexBase( const QObexBase& );
  /**
     Disable assignment
  */
  QObexBase& operator=( const QObexBase& );
};

#endif
