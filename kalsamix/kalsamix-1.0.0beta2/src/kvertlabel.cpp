/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kvertlabel.h"
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>

#include "kamconfig.h"

KVertLabel::KVertLabel(QWidget *parent, KVertLabel::VLType lt, const char *name, WFlags f)
 : QWidget(parent, name, f)
{
    txt = "";
    isRed = false;
    labelType = lt;
    
    bottomLine = ( lt == KVertLabel::GroupShown || lt == KVertLabel::GroupHidden );
    orientation = ( lt == KVertLabel::GroupShown ? KVertLabel::Horizontal : KVertLabel::Vertical );
    
    fnt = KAMConfig::self()->lblFont;
}

KVertLabel::~KVertLabel()
{
}

void KVertLabel::setText ( const QString& t ) {
    fnt.setBold ( labelType==KVertLabel::Card );
    txt = t;
    QFontMetrics fm(fnt);
    
    if ( orientation==KVertLabel::Vertical ) {
    	setFixedWidth ( fm.size(0,txt).height()+4 );
    	setMinimumHeight( fm.size(0,txt).width() );
    } else {
    	setMinimumWidth ( fm.size(0,txt).width() );
    	setFixedHeight( fm.size(0,txt).height()+4 );
    }
    
}

const QString& KVertLabel::text () {
    return txt;
}

void KVertLabel::paintEvent ( QPaintEvent* ) {
    QPainter p(this);
    
    if ( orientation == KVertLabel::Vertical ) {
        if ( bottomLine )
            p.drawLine ( width()-1, 0, width()-1, height() );
    
        p.rotate(270);

        if ( isRed )
            p.setPen(QColor(255,0,0));

        p.setFont ( fnt );

        if ( labelType!=KVertLabel::Chan )
            p.drawText ( -height(), width()-2, txt );
        else
            p.drawText ( -70, width()-2, txt );
    } else {
        if ( bottomLine )
	    p.drawLine ( 0, height()-1, width()-1, height()-1 );

        if ( isRed )
	    p.setPen(QColor(255,0,0));
	   
	p.setFont ( fnt );
	p.drawText ( 0, height()-2, txt );
    }
    

}

void KVertLabel::mousePressEvent( QMouseEvent* )
{
        emit clicked();
}

#include "kvertlabel.moc"
