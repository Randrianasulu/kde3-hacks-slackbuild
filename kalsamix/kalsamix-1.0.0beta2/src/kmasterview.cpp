/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kmasterview.h"
#include "kalsamix.h"

#include <qvbox.h>

KMasterView::KMasterView(KMixerItem *itm, QWidget *parent, const char *name)
 : KPopupFrame(parent, name)
{
    vb = new QVBox ( this );
    
    sli = new KMixerSlider ( vb, itm, KMixConst::Playback );
    connect ( sli, SIGNAL(muteClicked(KMixConst::Sense,unsigned int)), itm,
        SLOT(toggleMuted(KMixConst::Sense,unsigned int)) );
    connect ( sli, SIGNAL(volumeChanged(KMixConst::Sense,long,unsigned int)), itm,
        SLOT(setVolume(KMixConst::Sense, long, unsigned int )) );
    connect ( sli, SIGNAL(valueChanged(int)), itm, SLOT(setEnumValue(int)) );
    
    sli->enableMenu = false;
    sli->showSplitted = false;
}

void KMasterView::popup ( const QPoint &pos )
{
    int t, l;
    l = pos.x();
    t = pos.y() - 150;
    if ( t<0 )
        t = 0;

    vb->setGeometry ( 0,0,40,150 );
    setMainWidget ( vb );
    KPopupFrame::popup ( QPoint (l,t) );

}

KMasterView::~KMasterView()
{
}

void KMasterView::hideEvent( QHideEvent *ev ) {
    kalsamix *ka = (kalsamix*)parent();
    ka->osdEnable();
    QWidget::hideEvent(ev);
}

#include "kmasterview.moc"
