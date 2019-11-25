/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kaction.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <qcursor.h>
#include <kstdaccel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>

#include "kmixerslider.h"
#include "kamconfig.h"

KMixerSlider::KMixerSlider(QWidget *parent, KMixerItem *itm,
    enum KMixConst::Sense sns, const char *name, WFlags f)
 : QWidget(parent, name, f)
{
    unsigned int j;
    
    enableMenu = true;
    showSplitted = false;

    // Saves item references for saving splitted setting    
    mixitm   = itm;
    mixitmid = itm->id;
    configSect = mixitm->mix->card->configSection();
    
    aHide = new KAction ( i18n("Hide"), "", KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(hideItem()), this );
    aMute = new KAction ( i18n("Mute"), "", KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(muteChangedAll()) , this );
    aSplit= new KAction ( i18n("Split"), "", KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(balanceChanged()), this );

    pm = new KPopupMenu( this );
    pm->insertTitle ( mixitm->name() );
    aHide->plug ( pm );
    aMute->plug ( pm );
    aSplit->plug ( pm );
    
    if ( mixitm->mono(sns) )
        aSplit->setEnabled ( false );

    connect ( itm, SIGNAL(changed(KMixerItem*)), this, SLOT( itemChanged(KMixerItem*) ) );

    sense  = sns;

    QVBoxLayout *mlayout = new QVBoxLayout(this);
    
    vbox = new QVBox ( this );
    
    // Can adjust single channels
    hbox3 = new QHBox ( vbox );
    lbal = 0;
    if ( !mixitm->mono(sense) && mixitm->hasVolume(sense) ) {
        lbal = new KLedEx ( hbox3, KLedEx::yellow );
        connect ( lbal, SIGNAL(clicked()), this, SLOT(balanceChanged()) );
	hbox3->setFixedHeight ( 25 );
    }
    
    // Box for names and sliders/enums
    hbox = new QHBox ( vbox );

    // Box for switches
    hbox2 = new QHBox ( vbox );
    hbox2->setFrameShape ( QFrame::NoFrame );
    
    if ( KAMConfig::self()->showMute )
        hbox2->setFixedHeight ( 25 );
    else
        hbox2->setFixedHeight ( -1 );

    litm= new KVertLabel ( hbox, KVertLabel::Item );
    litm->setText ( mixitm->name() );
    connect ( litm, SIGNAL(clicked()), this, SLOT(showMenu()) );
    
    if ( sense == KMixConst::Capture )
        litm->isRed = true;

    // Can be muted
    lMute= 0;
    if ( mixitm->hasSwitch(sense) || mixitm->hasSwitch(KMixConst::Common) ) {
        lMute = new KMuteWidget ( hbox2, (sense==KMixConst::Playback ? KLedEx::green : KLedEx::red ) );
	lMute->setHost ( mixitm );
	connect ( lMute, SIGNAL(mute(KMuteWidget*)), this, SLOT(muteChanged(KMuteWidget*)) );
    }

    // Combo for enumerated items
    cval = 0;
    if ( mixitm->hasEnum(sense) ) {
        cval = new QComboBox ( hbox );
        cval->insertStringList ( mixitm->valueNames );
	connect ( cval, SIGNAL(activated(int)), this, SLOT(enumChanged(void)) );
    }

    if ( KAMConfig::self()->itemsFrame ) {
        hbox->setFrameShape  ( QFrame::Panel );
        hbox->setFrameShadow ( QFrame::Raised );
        hbox2->setFrameShape  ( QFrame::Panel );
        hbox2->setFrameShadow ( QFrame::Raised );
        hbox3->setFrameShape  ( QFrame::Panel );
        hbox3->setFrameShadow ( QFrame::Raised );
    }
    
    if ( (sense == KMixConst::Playback && KAMConfig::self()->splitPbChannels( configSect, mixitm->id )) ||
         (sense == KMixConst::Capture  && KAMConfig::self()->splitCpChannels( configSect, mixitm->id )) ) {
        showSplitted = true;
	if ( lbal )
	    lbal->setOn();
    }

    KMuteWidget *chanLed;
    
    // Check for volume in the given sense, or common
    if ( mixitm->hasVolume(sense) || mixitm->hasVolume(KMixConst::Common) ) {
        for ( j=0; j<mixitm->channels(sense)->count() ; j++) {

	    KVertLabel *lch = new KVertLabel ( hbox, KVertLabel::Chan );
	    labels.append ( lch );
            lch->setText ( mixitm->channels(sense)->at(j)->name() );
        
	    QVBox *vv = new QVBox ( hbox );
	    
	    chanLed = 0;
	    if ( mixitm->hasSwitch(sense) && !mixitm->hasSwitchJoined(sense) ) {
	        chanLed = new KMuteWidget ( vv, KLedEx::green, 12 );
		chanLed->setHost ( mixitm->channels(sense)->at(j) );
		chanLeds.append ( chanLed );
		connect ( chanLed, SIGNAL(mute(KMuteWidget*)), this, SLOT(muteChanged(KMuteWidget*)) );
	    }

	    KSliderEx *sli = new KSliderEx ( vv, mixitm->channels(sense)->at(j) );
            sliders.append ( sli );

            sli->setRange ( mixitm->minVolume(sense),mixitm->maxVolume(sense));
	    sli->setPageStep ( 3 );

	    sli->setMixerValue ( mixitm->volume(sense, j) );
	
            if ( !showSplitted ) {
	        if ( j != 0 )
	            sli->hide ();
	        lch->hide ( );
	    }
        }
    
        for ( j=0; j<sliders.count(); j++ ) {
            connect ( sliders.at(j), SIGNAL(rightClick()), this, SLOT(showMenu()) );
	    connect ( sliders.at(j), SIGNAL(changed(KSliderEx*,int,int)), this,
	        SLOT(sliderChanged(KSliderEx*,int,int)) );
        }
        
    }

    mlayout->addWidget ( vbox );
    paintEvent(NULL);
}

KMixerSlider::~KMixerSlider()
{
    if ( sense == KMixConst::Playback )
        KAMConfig::self()->setSplitPbChannels ( configSect, mixitm->id, showSplitted );
    else
        KAMConfig::self()->setSplitCpChannels ( configSect, mixitm->id, showSplitted );
}

void KMixerSlider::paintEvent ( QPaintEvent* )
{
    unsigned int j;
    QSlider *sli;
    
    for ( j=0; j<sliders.count() ; j++) {

	sli = sliders.at(j);

	if ( KAMConfig::self()->showTicks && sli->maxValue() ) {
            sli->setTickmarks ( QSlider::Right );
            if(sli->height()-20/sli->maxValue() < 4 ) {
	        sli->setTickInterval ( 4 );
	    } else {
	        sli->setTickInterval ( 1 );
	    }
	} else {
	    sli->setTickmarks ( QSlider::NoMarks );
	}

	if ( !showSplitted ) {
	    if ( j != 0 )
	        sli->hide (  );
	    labels.at(j)->hide ( );
	} else {
	    sli->show();
	    labels.at(j)->show();
	}
        sli->setValue ( mixitm->maxVolume(sense)-mixitm->channels(sense)->at(j)->volume() );
	
    }

    for ( j=0; j<chanLeds.count(); j++ ) {
        if ( showSplitted ) {
	    KChannel *chan = (KChannel*)chanLeds.at(j)->host();
	    chanLeds.at(j)->setMute ( chan->muted() );
	    chanLeds.at(j)->show();
	} else
	    chanLeds.at(j)->hide();
    }
    
    if ( lMute ) {
	lMute->setShown ( KAMConfig::self()->showMute );
	lMute->setMute  ( mixitm->muted(sense,KAMIX_ALL_CHANNELS) );
    }

    if ( lbal ) {
	lbal->setShown ( KAMConfig::self()->showBal );
	hbox3->setShown ( KAMConfig::self()->showBal );
    } else
        hbox3->hide();

    if ( cval )
        cval->setCurrentItem ( mixitm->enumValue() );

}

void KMixerSlider::sliderChanged( KSliderEx* sli, int val, int shift )
{
    if ( showSplitted )
        if ( shift )
            emit volumeChanged( sense, val, KAMIX_ALL_CHANNELS );
	else
            emit volumeChanged( sense, val, sli->channel->id );
    else
        emit volumeChanged( sense, val, KAMIX_ALL_CHANNELS );
}

void KMixerSlider::balanceChanged ()
{
    showSplitted = !showSplitted;
    repaint();
}

void KMixerSlider::muteChanged ( KMuteWidget *mw )
{
    if ( mw->hostType == KMuteWidget::Item )
        emit muteClicked(sense, KAMIX_ALL_CHANNELS);
    else {
        KChannel *chan = (KChannel*)mw->host();
        emit muteClicked(sense, chan->id );
    }
}

void KMixerSlider::muteChangedAll() {
    emit muteClicked ( sense, KAMIX_ALL_CHANNELS );
}

void KMixerSlider::enumChanged ( )
{
    emit valueChanged ( cval->currentItem() );
}

void KMixerSlider::itemChanged(KMixerItem*)
{
    repaint();
}

void KMixerSlider::hideItem()
{
    vbox->hide();
    
    if ( sense == KMixConst::Playback )
        KAMConfig::self()->setShowPlayback( configSect, mixitm->id, false );
    else
        KAMConfig::self()->setShowCapture( configSect, mixitm->id, false );
}

void KMixerSlider::showMenu() {
    QColor c = paletteBackgroundColor();
    QColor cs= c;
    setPaletteBackgroundColor ( c.dark(105) );
    if ( enableMenu ) 
        pm->exec(QCursor::pos());
    setPaletteBackgroundColor ( cs );
}

#include "kmixerslider.moc"
