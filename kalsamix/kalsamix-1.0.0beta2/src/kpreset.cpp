/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kpreset.h"
#include <qmap.h>
#include <kalsa.h>
#include <qstringlist.h>
#include <kmixeritem.h>

#include "kamconfig.h"

KPreset::KPreset()
{
}

KPreset::~KPreset()
{
}

unsigned int KPreset::count() {
    KConfig *conf = KAMConfig::self()->config();
    conf->setGroup ( "Presets" );
        
    typedef QMap<QString,QString> _map;
    _map map = conf->entryMap ( "Presets" );
    
    return map.count();
    
}

const QString KPreset::preset(int i) {
    KConfig *conf = KAMConfig::self()->config();
    conf->setGroup ( "Presets" );
    int j;
    
    typedef QMap<QString,QString> _map;
    _map map = conf->entryMap ( "Presets" );
    
    _map::Iterator iter;
    
    j=0;
    for ( iter=map.begin(); iter!=map.end(); iter++ ) {
        if ( i==j )
	    return iter.key();
        j++;
    }

    return "";
}

const QString KPreset::volumesToString() {
    KConfig *conf = KAMConfig::self()->config();
    conf->setGroup ( "Presets" );
    
    QString s, s2;
    unsigned int i, j;
    
    for ( i=0; i<KAlsa::self()->cardsCount(); i++ ) {
        for ( j=0; j<KAlsa::self()->card(i)->mixer()->elemCount(); j++ ) {
	    KMixerItem *itm = KAlsa::self()->card(i)->mixer()->items.at(j);
	    if ( (!itm->hasEnum(KMixConst::Playback)) && (!itm->hasEnum(KMixConst::Capture)) ) {
	        s2.sprintf ( "P:%d:%d:%ld:%d;", i, itm->id, itm->volume(KMixConst::Playback),
	            (itm->muted(KMixConst::Playback,KAMIX_ALL_CHANNELS)?0:1) );
		s += s2;
	        s2.sprintf ( "C:%d:%d:%ld:%d;", i, itm->id, itm->volume(KMixConst::Capture), 
	            (itm->muted(KMixConst::Capture,KAMIX_ALL_CHANNELS)?0:1) );
		s += s2;
	    } else {
	        s2.sprintf ( "P:%d:%d:%d:%d;", i, itm->id, itm->enumValue(),
	            (itm->muted(KMixConst::Playback,KAMIX_ALL_CHANNELS)?0:1) );
		s += s2;
	        s2.sprintf ( "C:%d:%d:%d:%d;", i, itm->id, itm->enumValue(),
	            (itm->muted(KMixConst::Capture,KAMIX_ALL_CHANNELS)?0:1) );
		s += s2;
	    }
	}
    }
    return s;
}

void KPreset::loadPreset(const QString& pset) {
    KConfig *conf = KAMConfig::self()->config();
    conf->setGroup ( "Presets" );
    QString s = conf->readEntry ( pset );

    QStringList pars = QStringList::split(";",s);
    
    for ( QStringList::Iterator pit=pars.begin(); pit!=pars.end(); pit++ ) {
    

        QString vol = *pit;

        QStringList vols = QStringList::split(":",vol);
	QStringList::Iterator vit = vols.begin();
	
	QString sense = *vit;
        long cardno = (*(++vit)).toInt();
        long itemno = (*(++vit)).toInt();

        KMixerItem *itm = KAlsa::self()->card(cardno)->mixer()->items.at(itemno);
	
	if ( itm && sense=="P" ) {
	    if ( !itm->hasEnum(KMixConst::Playback) )
	        itm->setVolume(KMixConst::Playback, (*(++vit)).toInt());
	    else
	        itm->setEnumValue( (*(++vit)).toInt());
	    itm->setMuted( KMixConst::Playback, (*(++vit)).toInt()==0 );
	}
	if ( itm && sense=="C" ) {
	    if ( !itm->hasEnum(KMixConst::Capture) )
	        itm->setVolume(KMixConst::Capture, (*(++vit)).toInt());
	    else
	        itm->setEnumValue( (*(++vit)).toInt());
	    itm->setMuted( KMixConst::Capture, (*(++vit)).toInt()==0 );
	}
    }        
}
