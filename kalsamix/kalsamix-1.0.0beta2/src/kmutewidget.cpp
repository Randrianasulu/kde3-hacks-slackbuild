/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kmutewidget.h"
#include <qlayout.h>

KMuteWidget::KMuteWidget(QWidget *parent, KLedEx::Color color, unsigned int size )
 : QWidget(parent)
{
    setFixedSize ( size, size );
    QVBoxLayout *box = new QVBoxLayout ( this );
    led = new KLedEx ( this, color, size );
    _host = 0;
    box->addWidget ( led );
    connect ( led, SIGNAL(clicked()), this, SLOT(slotMute()) );
}

KMuteWidget::~KMuteWidget()
{
}

void KMuteWidget::setHost( KChannel *chan ) {
    _host = chan;
    hostType = KMuteWidget::Channel;
}

void KMuteWidget::setHost( KMixerItem *itm ) {
    _host = itm;
    hostType = KMuteWidget::Item;
}

void* KMuteWidget::host() {
    return _host;
}

void KMuteWidget::slotMute() {
    emit mute(this);
}

void KMuteWidget::setMute( int b ) {
    
    switch ( b ) {
    case KMixConst::Active:
        led->setOn();
        break;
    case KMixConst::Muted:
        led->setOff();
        break;
    case KMixConst::Partial:
        led->setPartial();
        break;
    }
    led->repaint();
}

#include "kmutewidget.moc"
