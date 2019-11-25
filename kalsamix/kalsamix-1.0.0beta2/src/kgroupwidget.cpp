/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kgroupwidget.h"
#include <qpainter.h>
#include <qlayout.h>
#include <qcolor.h>

#include "kamconfig.h"

KGroupWidget::KGroupWidget(QWidget *parent, const char *name, bool e, bool dark )
  : QWidget ( parent, name )
{

    QVBoxLayout *lo = new QVBoxLayout(this);
    
    vbox = new QVBox (this);
    
    showFrame = KAMConfig::self()->showFrameAroundGroups;
    vbox->show();
    
    _expanded = e;
    
    if ( dark ) {
    	QColor c = paletteBackgroundColor();
    	setPaletteBackgroundColor ( c.dark(110) );
    }
    
    lhead = new KVertLabel ( this, KVertLabel::GroupShown );
    hgrid = new QGridLayout ( lo );
    lside = new KVertLabel ( this, KVertLabel::GroupHidden );
    
    connect ( lside, SIGNAL(clicked()), this, SLOT(slotToggle() ) );
    connect ( lhead, SIGNAL(clicked()), this, SLOT(slotToggle() ) );
    
    hgrid->addMultiCellWidget ( lhead, 0, 0, 0, 100 );
    hgrid->addWidget ( lside, 1, 1, Qt::AlignBottom );

}

KGroupWidget::~KGroupWidget()
{
}

bool KGroupWidget::expanded() {
    return _expanded;
}

void KGroupWidget::setExpanded(bool b) {
    _expanded = b;
    update();
}

void KGroupWidget::setTitle( const QString &t ) 
{
    lside->setText ( t );
    lhead->setText ( t );
}

void KGroupWidget::paintEvent(QPaintEvent* ev) {
unsigned int i;

    i = 2;
    vbox->setFrameShape ( QFrame::NoFrame );
    if ( _expanded ) {
	lside->hide();
	lhead->show();
    } else {
	lside->show();
	lhead->hide();
    }
    for ( i=0; i<sliders.count(); i++ ) {
        if ( _expanded )
	    hgrid->addWidget ( sliders.at(i), 1, cols[i] );
	else
	    hgrid->remove ( sliders.at(i) );
        sliders.at(i)->setShown ( _expanded );
    }
	
    QWidget::paintEvent(ev);
    
}

void KGroupWidget::slotToggle() {
    _expanded = !_expanded;    
    update();
}

void KGroupWidget::addItem( KMixerSlider *itm, long pos ) {
    sliders.append ( itm );
    cols.append ( pos );
}

#include "kgroupwidget.moc"
