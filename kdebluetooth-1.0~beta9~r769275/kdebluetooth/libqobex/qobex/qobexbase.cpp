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

#include <qobject.h>

#include "qobexbase.h"

#undef DEBUG
// #define DEBUG
#ifdef DEBUG
#define myDebug(a) qDebug a
#else
#define myDebug(a) (void)0
#endif

QObexBase::QObexBase( QObexTransport* transport, QObject* parent, const char* name )
  : QObject( parent, name ) {
  myDebug(( "QObexBase::QObexBase( ... )" ));

  insertChild( transport );

  mTransport = transport;
  mMtu = QObexTransport::MinimumMtu;
  mMaxBodySize = 32*1024;
  
  QObject::connect( mTransport, SIGNAL(signalConnected()),
		    SLOT(slotTransportConnected()) );
  QObject::connect( mTransport, SIGNAL(signalConnectionClosed()),
		    SLOT(slotTransportConnectionClosed()) );
  QObject::connect( mTransport, SIGNAL(signalPacketRecieved(QObexObject&)),
		    SLOT(slotTransportPacketRecieved(QObexObject&)) );
  QObject::connect( mTransport, SIGNAL(signalPacketWritten()),
		    SLOT(slotTransportPacketWritten()) );
  QObject::connect( mTransport, SIGNAL(signalError(QObexTransport::Error)),
		    SLOT(slotTransportError(QObexTransport::Error)) );
  QObject::connect( mTransport, SIGNAL(signalSelectModeChanged(QObexTransport::SelectHint,Q_LONG)),
		    SLOT(slotTransportSelectModeChanged(QObexTransport::SelectHint,Q_LONG)) );
}

QObexBase::~QObexBase() {
  myDebug(( "QObexBase::~QObexBase( ... )" ));
}

bool QObexBase::sendObject( QObexObject* obj ) {
  myDebug(( "QObexBase::sendObject( ... )" ));
  if ( obj->getPacketType() == QObexObject::ConnectPacket ) {
    mMtu = obj->getMtu();
    if ( mMtu == 0 )
      mMtu = getOptimumMtu();
    obj->setMtu( max( mMtu, Q_LONG(QObexTransport::MinimumMtu) ) );
  }
  bool ret = mTransport->sendObject( obj ); 
  if ( obj->code() == QObexObject::Disconnect )
    mMtu = QObexTransport::MinimumMtu;
  return ret;
}

Q_UINT16 QObexBase::getOptimumMtu() const {
  myDebug(( "QObexBase::getOptimumMtu() const" ));
  Q_LONG bps = mTransport->bytesPerSecond();
  // We want connections to be interruptable within a fourth second.
  Q_LONG mtu = bps/4;
  Q_LONG tmtu = mTransport->maximumTransferUnit();

  // round to full size packets.
  if ( tmtu < mtu )
    mtu = (mtu/tmtu)*tmtu;
  mtu = max( mtu, Q_LONG(QObexTransport::MinimumMtu) );
  mtu = min( mtu, Q_LONG(QObexTransport::MaximumMtu) );

  myDebug(( "QObexBase::getOptimumMtu() const %d", int( mtu ) ));

  return mtu;
}

Q_LONG QObexBase::getOptimumBodySize( Q_UINT16 reserved ) const {
  myDebug(( "QObexBase::getOptimumnBodySize( ... ) const" ));
  Q_UINT16 avail = mMtu - reserved - 3;
  if ( mMaxBodySize <= avail )
    return mMaxBodySize;
  else if ( 4096 <= avail )
    return (avail/4096)*4096;
  else if ( 1024 <= avail )
    return (avail/1024)*1024;
  else
    return avail;
}

void QObexBase::slotTransportConnected() {
  myDebug(( "QObexBase::slotTransportConnected()" ));
  mMtu = QObexTransport::MinimumMtu;
  transportConnected();
}

void QObexBase::slotTransportConnectionClosed() {
  myDebug(( "QObexBase::slotTransportConnectionClosed()" ));
  transportConnectionClosed();
  mMtu = QObexTransport::MinimumMtu;
}

void QObexBase::slotTransportPacketRecieved( QObexObject& obj ) {
  myDebug(( "QObexBase::slotTransportPacketRecieved( ... )" ));
  if ( obj.getPacketType() == QObexObject::ConnectPacket )
    mMtu = max( obj.getMtu(), Q_LONG(QObexTransport::MinimumMtu) );
  transportPacketRecieved( obj );
  if ( obj.code() == QObexObject::Disconnect )
    mMtu = QObexTransport::MinimumMtu;
}

void QObexBase::slotTransportPacketWritten() {
  myDebug(( "QObexBase::slotTransportPacketWritten()" ));
  transportPacketWritten();
}

void QObexBase::slotTransportError( QObexTransport::Error err ) {
  myDebug(( "QObexBase::slotTransportError( ... )" ));
  transportError( err );
}

void QObexBase::slotTransportSelectModeChanged( QObexTransport::SelectHint hint, Q_LONG timeout ) {
  myDebug(( "QObexBase::slotTransportSelectModeChanged( ... )" ));
  transportSelectModeChanged( hint, timeout );
}

#include "qobexbase.moc"
