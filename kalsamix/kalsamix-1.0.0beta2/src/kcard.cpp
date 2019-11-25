/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qstring.h>

#include "kcard.h"
#include "kamconfig.h"

KCard::KCard(QString &devname, QWidget *parent, const char *name)
 : QWidget(parent, name)
{
    _devname = devname;
    _mixer = 0;
    _configSection = "Card_";
    _configSection+= _devname;
}

void KCard::setName( QString &name ) {
    _name = name;
}

const QString& KCard::name() {
    return _name;
}

KMixer* KCard::mixer() {
    return _mixer;
}

KCard::~KCard()
{
}

const QString& KCard::deviceName() {
     return _devname;
}

bool KCard::openMixer() {
    _mixer = new KMixer ( this );
    
    if ( _mixer->open() )
        return false;
    else
        return true;
}

const QString& KCard::configSection() {
    return _configSection;
}

void KCard::writeConfig() {
    QString s;
    
    unsigned int i;
    for ( i=0; i<_mixer->elemCount(); i++ ) {
    
        KMixerItem *itm;
        itm = _mixer->items.at(i);
    
        if ( itm->hasVolume( KMixConst::Playback ) ) {
	    KAMConfig::self()->setRawLong( _configSection, "PlaybackLevel", itm->volume ( KMixConst::Playback ), itm->id );
	}
	if ( itm->hasVolume( KMixConst::Capture ) ) {
	    KAMConfig::self()->setRawLong( _configSection, "CaptureLevel", itm->volume ( KMixConst::Capture ), itm->id );
	}
	if ( itm->hasSwitch( KMixConst::Playback ) ) {
	    KAMConfig::self()->setRawBool( _configSection, "PlaybackSwitch", !itm->muted(KMixConst::Playback,KAMIX_ALL_CHANNELS), itm->id );
	}
	if ( itm->hasSwitch( KMixConst::Capture ) ) {
	    KAMConfig::self()->setRawBool( _configSection, "CaptureSwitch", !itm->muted(KMixConst::Capture,KAMIX_ALL_CHANNELS), itm->id );
	}
	if ( itm->hasEnum(KMixConst::Playback)||itm->hasEnum(KMixConst::Capture) ) {
	    KAMConfig::self()->setRawLong( _configSection, "EnumeratedValue", itm->enumValue(), itm->id );
	}
    }

}

#include "kcard.moc"
