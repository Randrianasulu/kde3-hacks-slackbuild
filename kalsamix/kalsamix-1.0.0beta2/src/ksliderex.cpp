/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ksliderex.h"
#include <qslider.h>

KSliderEx::KSliderEx(QWidget *parent, KChannel *chan )
 : QSlider(QSlider::Vertical, parent, NULL)
{
    channel = chan;
    setMinimumHeight ( 80 );

    mid_pressed = false;
    
    connect ( this, SIGNAL(valueChanged(int)), this, SLOT(valChanged(int)) );
}

KSliderEx::~KSliderEx()
{
}

void KSliderEx::valChanged( int val )
{
    if ( val == prevValue() )
        return;
    emit changed(this, maxValue() - val, mid_pressed );
}

void KSliderEx::setMixerValue( int val ) {
    setValue ( maxValue()-val );
}

void KSliderEx::mousePressEvent( QMouseEvent *ev ) {
    if ( ev->button() == QEvent::RightButton && ev->type()==QEvent::MouseButtonPress )
        emit rightClick();
    if ( ev->button() == QEvent::MidButton && ev->type()==QEvent::MouseButtonPress )
        mid_pressed = true;
	
    QSlider::mousePressEvent( ev );
}

void KSliderEx::mouseReleaseEvent( QMouseEvent *ev ) {
    mid_pressed = false;
    QSlider::mouseReleaseEvent ( ev );
}

#include "ksliderex.moc"
