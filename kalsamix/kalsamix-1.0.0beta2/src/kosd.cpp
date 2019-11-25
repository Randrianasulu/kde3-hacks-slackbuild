/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the KAMConfig of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kosd.h"
#include <qbitmap.h>
#include <qfont.h>
#include <qrect.h>
#include <qpainter.h>
#include "kamconfig.h"
#include <qapplication.h>

KOsd::KOsd(QWidget *parent, const char *name)
 : QWidget(parent, name, WType_TopLevel|WNoAutoErase|WStyle_Customize|WX11BypassWM|WStyle_StaysOnTop)
{
    setBackgroundMode ( NoBackground );
    setFocusPolicy ( NoFocus );
    osdText = "";
    connect ( &tmr, SIGNAL(timeout()), this, SLOT(timeout()) );
    
    readConfig();
        
    isActive = false;
}

void KOsd::readConfig() {
    osdpos = KAMConfig::self()->osdPosition;
    osdX = KAMConfig::self()->osdPositionX;
    osdY = KAMConfig::self()->osdPositionY;
    fnt = KAMConfig::self()->osdFont;
    colb = KAMConfig::self()->osdBackcolor;
    colf = KAMConfig::self()->osdForecolor;
}

KOsd::~KOsd()
{
}

void KOsd::setText( const QString &s ) {
    osdText = s;
    prepareBitmap();
}

void KOsd::showOsd() {
    if ( osdText == "" )
        return;
    if ( isActive )
        hide();
    show();
    isActive = true;
    tmr.start(1000, true);
}

void KOsd::timeout() {
    hide();
    isActive = false;
}

void KOsd::prepareBitmap() {

    QFontMetrics fntm(fnt);
    QRect r = fntm.boundingRect( osdText );
    
    r.addCoords ( 0, 0, 14, 10 );
    bmap.resize ( r.size() );
    mask.resize ( r.size() );
    
    switch ( osdpos ) {
    case 0:
        // Top left
        move ( 10, 10 );
	break;
    case 1:
        // Top right
	move ( QApplication::desktop()->screen(0)->size().width()-10-r.width(), 10 );
	break;
    case 2:
        // Bottom left
        move ( 10, QApplication::desktop()->screen(0)->size().height()-10-r.height() );
	break;
    case 3:
        // Bottom right
        move ( QApplication::desktop()->screen(0)->size().width()-10-r.width(),
	       QApplication::desktop()->screen(0)->size().height()-10-r.height()
	);
	break;
    case 4:
        // Custom position
        move ( osdX, osdY );
    }
    
    bmap.fill( colb );
    
    QPainter pnt ( &bmap );
    pnt.setBackgroundMode ( Qt::TransparentMode );
    QPainter mpnt( &mask );
    
    pnt.setBrush ( colb );
    
    pnt.setFont ( fnt );
    pnt.setPen ( colb.dark() );
    pnt.drawText ( 6, r.height()-4, osdText );
    pnt.setPen ( colf );
    pnt.drawText ( 5, r.height()-5, osdText );
    
    mask.fill ( Qt::black );
    mpnt.setBrush ( Qt::white );
    mpnt.drawRoundRect ( 0,0,r.width(), r.height(), 5, 50 );
    
    setMask ( mask );
    
}

void KOsd::paintEvent(QPaintEvent* ev) {
    bitBlt ( this, 0, 0, &bmap );
    QWidget::paintEvent(ev);
}

#include "kosd.moc"
