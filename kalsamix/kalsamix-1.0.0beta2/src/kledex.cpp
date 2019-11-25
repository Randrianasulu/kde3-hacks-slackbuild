/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kledex.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <kiconloader.h>
#include <kglobal.h>

KLedEx::KLedEx(QWidget *parent, KLedEx::Color c, int pSize )
 : QWidget(parent)
{
    setFixedSize ( pSize, pSize );
    _color = c;

    KIconLoader *il = KGlobal::iconLoader();
    
    QString s;
    switch ( _color ) {
    case green:
        s = "green";
	break;
    case red:
        s = "red";
	break;
    case yellow:
        s = "yellow";
	break;
    }
    
    pon  = il->loadIcon ( QString ( s+"_on" ),  KIcon::NoGroup, pSize );
    poff = il->loadIcon ( QString ( s+"_off" ), KIcon::NoGroup, pSize );
    ppart= il->loadIcon ( QString ("partial"),  KIcon::NoGroup, pSize );

    status = KMixConst::Muted;

}

KLedEx::~KLedEx()
{
}

void KLedEx::mousePressEvent ( QMouseEvent* )
{

    if ( status==KMixConst::Active )
        status = KMixConst::Muted;
    else
        status = KMixConst::Active;

    emit clicked();
}

void KLedEx::paintEvent ( QPaintEvent* ) {
    QPainter p(this);
    QPixmap pix;
    
    switch ( status ) {
    case KMixConst::Active:
        pix = pon;
        break;
    case KMixConst::Muted:
        pix = poff;
        break;
    case KMixConst::Partial:
    case KMixConst::Unknown:
        pix = ppart;
        break;
    }
    p.drawPixmap ( 0, 0, pix );
}

void KLedEx::setOn() {
    status = KMixConst::Active;
    repaint();
}

void KLedEx::setOff() {
    status = KMixConst::Muted;
    repaint();
}

void KLedEx::setPartial() {
    status = KMixConst::Partial;
    repaint();
}
#include "kledex.moc"
