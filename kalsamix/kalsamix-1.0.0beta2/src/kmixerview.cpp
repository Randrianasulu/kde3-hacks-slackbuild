/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kmixerview.h"
#include "kmixeritem.h"
#include "kvumeter.h"
#include "kgroupwidget.h"
#include "kalsamix.h"
#include "kamconfig.h"

#include "config.h"

#include <qhbox.h>
#include <klocale.h>
#include <qlabel.h>

KMixerView::KMixerView(KMixer *mix, viewType tp, QWidget *parent, const char *name, QWidget *tray )
 : QWidget(parent, name)
{
    osdEnabled = true;    
    _mixer = mix;
    tab = 0;
    type = tp;
    
    cname = new KVertLabel(this, KVertLabel::Card );
    cname->setText(_mixer->card->name() );

    configSect = _mixer->card->configSection();
    
    hbox1 = 0;
    hbox2 = 0;
    tab = new QTabWidget ( this );

    if ( KAMConfig::self()->showOSD && !KAMConfig::self()->showOSDExtern )
        connect ( mix, SIGNAL(itemHasChangedInt(KMixerItem*)), this, SLOT(osdNotify(KMixerItem*)) );
    if ( tray ) {
        connect ( mix, SIGNAL(itemHasChangedInt(KMixerItem*)), tray, SLOT(updateTrayTip()) );
        connect ( mix, SIGNAL(itemHasChangedExt(KMixerItem*)), tray, SLOT(updateTrayTip()) );
    }
    
    layout = 0;
}

KMixerView::~KMixerView()
{
    unsigned int i;
    for ( i=0; i<pgroupsw.count(); i++ )
	KAMConfig::self()->setRawBool( configSect, "PlaybackGroupExpanded", pgroupsw.at(i)->expanded(), i );
    for ( i=0; i<cgroupsw.count(); i++ )
	KAMConfig::self()->setRawBool( configSect, "CaptureGroupExpanded", cgroupsw.at(i)->expanded(), i );
}

void KMixerView::initControls() {
    
    unsigned int i;
    KMixerItem *itm;
    
    if ( layout )
        delete layout;
    layout = new QGridLayout(this);

    bool hi = KAMConfig::self()->hideInactive;
    
    for ( i=0; i<sliders.count(); i++ )
        delete sliders.at(i);
    
    for ( i=0; i<pgroupsw.count(); i++ )
        delete pgroupsw.at(i);
    for ( i=0; i<cgroupsw.count(); i++ )
        delete cgroupsw.at(i);
    sliders.clear();
    pgroupsw.clear();
    cgroupsw.clear();

    if ( hbox1 )
        delete hbox1;
    if ( hbox2 )
        delete hbox2;

    // Groups setup
    _mixer->cgroups.clear();
    _mixer->pgroups.clear();
    
    hbox1 = new QHBox ( this );
    hbox2 = new QHBox ( this );
    
    KItemsGroup *base = new KItemsGroup(this);
    base->setName( i18n("Main") );
    _mixer->pgroups.append(base);
    base = new KItemsGroup(this);
    base->setName( i18n("Main") );
    _mixer->cgroups.append(base);
    
    QStringList glst;
    QString s;    
    glst = KAMConfig::self()->playbackGroups ( configSect );
    for ( QStringList::Iterator it = glst.begin(); it != glst.end(); ++it ) {
            base = new KItemsGroup(hbox1);
	    base->setName( *it );
	    _mixer->pgroups.append(base);
    }
    
    glst = KAMConfig::self()->captureGroups ( configSect );
    for ( QStringList::Iterator it = glst.begin(); it != glst.end(); ++it ) {
            base = new KItemsGroup(hbox2);
	    base->setName( *it );
	    _mixer->cgroups.append(base);
    }

    hbox1->setFrameShape  ( QFrame::Box );
    hbox2->setFrameShape  ( QFrame::Box );
    hbox1->setFrameShadow ( QFrame::Sunken );
    hbox2->setFrameShadow ( QFrame::Sunken );

    hbox1->setSpacing(0);
        
    if ( type == viewTabs ) {    
        tab->addTab ( hbox1, i18n("Playback") );
        tab->addTab ( hbox2, i18n("Record") );
    } else {
        KVertLabel *l1 = new KVertLabel ( hbox1, KVertLabel::Card );
        KVertLabel *l2 = new KVertLabel ( hbox2, KVertLabel::Card );
	l1->setText( i18n("Playback" ));
	l2->setText( i18n("Record" ));
        if ( tab->count() ) {
            tab->removePage(hbox1);
	    tab->removePage(hbox2);
	}
    }
    
    KMixerSlider *slider;
    bool ee;
    for ( i=0; i<_mixer->pgroups.count(); i++ ) {
	ee = KAMConfig::self()->rawBool( configSect, "PlaybackGroupExpanded", true, i );
        KGroupWidget *gw = new KGroupWidget ( hbox1, "", ee, (i % 2) );
	gw->setTitle ( _mixer->pgroups.at(i)->name() );
	pgroupsw.append ( gw );
    }
    for ( i=0; i<_mixer->cgroups.count(); i++ ) {
	ee = KAMConfig::self()->rawBool( configSect, "CaptureGroupExpanded", true, i );
        KGroupWidget *gw = new KGroupWidget ( hbox2, "", ee, (i % 2) );
	gw->setTitle ( _mixer->cgroups.at(i)->name() );
	cgroupsw.append ( gw );
    }
    
    long order;
    long lastOrder;
    lastOrder = 0;
    for ( i=0; i<_mixer->items.count(); i++ ) {
        itm = _mixer->items.at(i);

	bool doit = ( itm->isActive && hi ) || ( !hi );
	itm->pbGroup = KAMConfig::self()->rawLong( configSect, "PlaybackGroup", 0, i );
        itm->cpGroup = KAMConfig::self()->rawLong( configSect, "CaptureGroup",  0, i );
	itm->setCustomName( KAMConfig::self()->rawString( configSect, "CustomName", "", i ));
	
	if ( doit && itm->hasPlaybackCapabilities() && KAMConfig::self()->showPlayback( configSect, i ) ) {
	    order = KAMConfig::self()->playbackOrder(configSect, i);
	    if ( order==-1 )
	        order = ++lastOrder;
	    lastOrder = order;
	    
	    if ( !pgroupsw.at(itm->pbGroup) )
	        itm->pbGroup = 0;
	    slider = new KMixerSlider ( pgroupsw.at(itm->pbGroup), itm, KMixConst::Playback );
	    slider->pborder = order;
	    
	    pgroupsw.at(itm->pbGroup)->addItem ( slider, order );
	    slider->show();
	    sliders.append ( slider );
	    connect ( slider, SIGNAL(muteClicked(KMixConst::Sense,unsigned int)), itm,
	    	SLOT(toggleMuted(KMixConst::Sense,unsigned int)) );
	    connect ( slider, SIGNAL(volumeChanged(KMixConst::Sense,long,unsigned int)), itm,
	    	SLOT(setVolume(KMixConst::Sense, long, unsigned int )) );
	    connect ( slider, SIGNAL(valueChanged(int)), itm, SLOT(setEnumValue(int)) );
        }

	if ( doit && itm->hasCaptureCapabilities() && KAMConfig::self()->showCapture( configSect, i ) ) {
	    order = KAMConfig::self()->captureOrder(configSect, i);
	    if ( order==-1 )
	        order = ++lastOrder;
	    lastOrder = order;
	    
	    if ( !cgroupsw.at(itm->cpGroup) )
	        itm->cpGroup = 0;
	    slider = new KMixerSlider ( cgroupsw.at(itm->cpGroup), itm, KMixConst::Capture );
	    slider->cporder = order;
	    
	    cgroupsw.at(itm->cpGroup)->addItem ( slider, order );
	    
	    slider->show();
	    sliders.append ( slider );
	    connect ( slider, SIGNAL(muteClicked(KMixConst::Sense,unsigned int)), itm,
	    	SLOT(toggleMuted(KMixConst::Sense,unsigned int)) );
	    connect ( slider, SIGNAL(volumeChanged(KMixConst::Sense,long,unsigned int)), itm,
	    	SLOT(setVolume(KMixConst::Sense, long, unsigned int )) );
	    connect ( slider, SIGNAL(valueChanged(int)), itm, SLOT(setEnumValue(int)) );
	}
    }

    if ( type == viewTabs ) {
        layout->addWidget ( cname, 0, 0 );
	layout->addWidget ( tab, 0, 1 );
	tab->repaint();
	tab->show();
    } else {
        layout->addMultiCellWidget ( cname, 0, 1, 0, 0 );
        layout->addWidget ( hbox1, 0, 1 );
	if ( type == viewTiledHor )
	    layout->addWidget ( hbox2, 0, 2 );
	else
	    layout->addWidget ( hbox2, 1, 1 );
	
	tab->hide();
        hbox1->show();
        hbox2->show();
    }
    
    KAMConfig::self()->viewType = type;
    
}

void KMixerView::osdNotify( KMixerItem *itm ) {

    if ( !osdEnabled )
        return;

    long v;
    double vp, vc;
    
    QString s1, s2, s3, mkC, mkP;
    s1 = itm->name();
    s1 += ": ";
    s2 = "";
    s3 = "";
    
    v = KAMConfig::self()->volumeMode;
    vp = 0;
    vc = 0;
    if ( v==0 ) {
        vp = itm->volume( KMixConst::Playback );
        vc = itm->volume( KMixConst::Capture );
        mkP = "";
        mkC = "";
    }
    if ( v==1 ) {
        vp = itm->percent( KMixConst::Playback );
        vc = itm->percent( KMixConst::Capture );
        mkP = "%";
        mkC = "%";
    }
#ifdef HAS_SND_DECIBEL
    if ( v==2 ) {
        vp = itm->dB( KMixConst::Playback );
        vc = itm->dB( KMixConst::Capture );
        mkP = "dB";
        mkC = "dB";
        if ( vp==KAMIX_NO_DB ) {
            vp = itm->volume( KMixConst::Playback );
            mkP = "";
        }         
        if ( vc==KAMIX_NO_DB ) {
            vc = itm->volume( KMixConst::Capture );
            mkC = "";
        }         
    }
#endif

    // Level
    if ( itm->hasVolume(KMixConst::Playback) ) {
        if ( mkP!="dB" )
            s2.sprintf ( "Pb: %ld", (long)vp );
        else
            s2.sprintf ( "Pb: %.2f", vp );
        s2 += mkP;
    } else
        s2.sprintf ( "Pb: n/a" );
        
    if ( itm->hasVolume(KMixConst::Capture) ) {
        if ( mkC!="dB" )
            s3.sprintf ( "Cp: %ld", (long)vc );
        else
            s3.sprintf ( "Cp: %.2f", vc );
        s3 += mkC;
    } else
        s3.sprintf ( "Cp: n/a" );
    
    if ( itm->hasSwitch(KMixConst::Playback) ) {
        s2 += ", ";
        switch ( itm->muted( KMixConst::Playback, KAMIX_ALL_CHANNELS ) ) {
        case KMixConst::Muted:
            s2 += i18n("muted");
            break;
        case KMixConst::Partial:
            s2 += i18n("partially muted");
            break;
        default:
            s2 += i18n("active");
        }
    }
    if ( itm->hasSwitch(KMixConst::Capture) ) {
        s3 += ", ";
        switch ( itm->muted( KMixConst::Capture, KAMIX_ALL_CHANNELS ) ) {
        case KMixConst::Muted:
            s3 += i18n("muted");
            break;
        case KMixConst::Partial:
            s3 += i18n("partially muted");
            break;
        default:
            s3 += i18n("active");
        }
    }
    
    s1 += s2;
    s1 += ", ";
    s1 += s3;
    
    osd.setText( s1 );
    osd.showOsd();
	
}

#include "kmixerview.moc"
