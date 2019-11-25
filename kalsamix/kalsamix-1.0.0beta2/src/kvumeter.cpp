/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kvumeter.h"

#include <qpainter.h>
#include <qbrush.h>
#include <kled.h>
#include <qlayout.h>
#include <qframe.h>

#include "kvertlabel.h"
#include <math.h>

KVUMeter::KVUMeter(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
    setMinimumHeight(30);
    setFixedWidth ( 16 );
    
    c1.setRgb ( 0,50,0 );
    c2.setRgb ( 0,200,0 );
    c3.setRgb ( 250,250,0 );
    c4.setRgb ( 200,0,0 );
    
    peakLeft = peakRight = 0;
    setBackgroundColor ( c1 );
}


KVUMeter::~KVUMeter()
{
}

void KVUMeter::checkPeakTime() {
    if ( peakTime.elapsed() >= 1000 ) {
        peakLeft = 0;
	peakRight = 0;
	peakTime.restart();
    }
}

void KVUMeter::setValueLeft ( float f ) {
    if ( f > peakLeft ) {
        peakLeft = f;
        peakTime.start();
    }
    valueLeft = f;
    repaint();
    checkPeakTime();
}

void KVUMeter::setValueRight ( float f ) {
    if ( f > peakRight ) {
        peakRight = f;
        peakTime.start();
    }
    valueRight = f;
    repaint();
    checkPeakTime();
}

void KVUMeter::setScale ( float s ) {
    if ( s != scale ) {
        scale = s;
    }
}

/*
float KVUMeter::levelToDb ( float l ) {
    if ( l < (1.f / (1<<20) ) )
        return ( 1.f / (1<<20) );
    return (6.f/log(2.f))*log(l);
}

float KVUMeter::DbToLevel( float d ) {
    return exp(d/(log(2.f)/6.f));
}
*/

void KVUMeter::paintEvent ( QPaintEvent * ) {
    QPainter p(this);
    int m = 1;
    int h = 0;
    int bh;

    if ( valueLeft ) {
        h = height()-m*2;
    
        bh = (int)(h * valueLeft);
        double f = bh*100/h;
        p.setBrush ( c2 );
        if ( f >= 70 )
            p.setBrush ( c3 );
        if ( f >= 90 )
            p.setBrush ( c4 );
        p.drawRect ( m,height()-m,7,-bh );
    }
    
    if ( valueRight ) {
        h = height()-m*2;
    
        bh = (int)(h * valueRight);
        double f = bh*100/h;
        p.setBrush ( c2 );
        if ( f >= 70 )
            p.setBrush ( c3 );
        if ( f >= 90 )
            p.setBrush ( c4 );
        p.drawRect ( m+7,height()-m,7,-bh );
    }
    
    if ( peakLeft ) {
        bh = (int)(h * peakLeft);
        p.setBrush ( c4 );
        p.drawRect ( m,height()-bh,7,-5 );
    }
    if ( peakRight ) {
        bh = (int)(h * peakRight);
        p.setBrush ( c4 );
        p.drawRect ( m+7,height()-bh,7,-5 );
    }
    
}

#include "kvumeter.moc"
