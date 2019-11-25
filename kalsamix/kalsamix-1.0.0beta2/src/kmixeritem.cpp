/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "kmixeritem.h"
#include <kdebug.h>

#include "config.h"
#include "kamconfig.h"

#define SNDCHIDT snd_mixer_selem_channel_id_t

KMixerItem::KMixerItem( snd_mixer_elem_t *e, KMixer *m, int idd )
{
    int i, j;
    int c, p;

    mix = m;

    id = idd;
    
    aelem = e;
    minPVolume = maxPVolume = 0;
    minCVolume = maxCVolume = 0;
    minPdB = maxPdB = 0;
    minCdB = minCdB = 0;
    
    aName = snd_mixer_selem_get_name ( e );

    canPlayback = false;
    canCapture  = false;

    hPVolume = snd_mixer_selem_has_playback_volume(aelem);
    hCVolume = snd_mixer_selem_has_capture_volume(aelem);
    hCoVolume= snd_mixer_selem_has_common_volume(aelem);

    hPSwitch = snd_mixer_selem_has_playback_switch(aelem);
    hCSwitch = snd_mixer_selem_has_capture_switch(aelem);
    hCoSwitch= snd_mixer_selem_has_common_switch(aelem);

    hPSwitchJoined = snd_mixer_selem_has_playback_switch_joined(aelem);
    hCSwitchJoined = snd_mixer_selem_has_capture_switch_joined(aelem);
    
    isActive = snd_mixer_selem_is_active(aelem);
    
    // Looks for channels
    for ( i=-1; i<=(int)SND_MIXER_SCHN_LAST; i++ ) {
        p = snd_mixer_selem_has_playback_channel(aelem,(SNDCHIDT)i);
        c = snd_mixer_selem_has_capture_channel (aelem,(SNDCHIDT)i);

	if ( p>0 ) {
            if ( !maxPVolume && hPVolume ) {
                snd_mixer_selem_get_playback_volume_range(aelem, &minPVolume, &maxPVolume);
#ifdef HAS_SND_DECIBEL
                snd_mixer_selem_get_playback_dB_range(aelem, &minPdB, &maxPdB);
#endif
            }
            KChannel *ch = new KChannel ( this, i, KMixConst::Playback );
	    pbChannels.append ( ch );
	    canPlayback = true;
	}
	
	if ( c>0 ) {
	    if ( !maxCVolume && hCVolume ) {
                snd_mixer_selem_get_capture_volume_range(aelem, &minCVolume, &maxCVolume);
#ifdef HAS_SND_DECIBEL
                snd_mixer_selem_get_capture_dB_range(aelem, &minCdB, &maxCdB);
#endif         
            }
            KChannel *ch = new KChannel ( this, i, KMixConst::Capture );
	    cpChannels.append ( ch );
            canCapture = true;
	}
	
	// For common volume, the playback rules
	if ( hCoVolume ) {
	    minCVolume = minPVolume;
	    maxCVolume = maxPVolume;
	}
    }

    // Checks for enumerated values
    hPEnum = canPlayback && (snd_mixer_selem_is_enum_playback(aelem)!=0);
    hCEnum = canCapture  && (snd_mixer_selem_is_enum_capture(aelem)!=0);

    if ( hPEnum || hCEnum ) {
        i = snd_mixer_selem_get_enum_items(aelem);
        if ( i>0 ) {
            char* item_name;
	    item_name = (char*)malloc(150);
         
	    for ( j=0; j<i; j++ ) {
	        snd_mixer_selem_get_enum_item_name(aelem,j,150,item_name);
	        valueNames.append ( QString(item_name) );
	    }
	    free ( item_name );
        }
    }
 
    // Loads volume and switches
    if ( KAMConfig::self()->loadLevels && KAMConfig::self()->allowLoadLevels ) {
	long l;
	QString s;
	
        if ( hPVolume ) {
	    l = KAMConfig::self()->rawLong( mix->card->configSection(), "PlaybackLevel", -1, id );
	    if ( l!=-1 )
	        setVolume ( KMixConst::Playback, l );
	}
        if ( hCVolume ) {
	    l = KAMConfig::self()->rawLong( mix->card->configSection(), "CaptureLevel", -1, id );
	    if ( l!=-1 )
	        setVolume ( KMixConst::Capture, l );
	}
	if ( hPSwitch ) {
    	    setMuted ( KMixConst::Playback, !KAMConfig::self()->rawBool( mix->card->configSection(), "PlaybackSwitch", true, id ));
	}
	if ( hCSwitch ) {
    	    setMuted ( KMixConst::Capture, !KAMConfig::self()->rawBool( mix->card->configSection(), "CaptureSwitch", true, id ));
	}
	if ( hPEnum || hCEnum ) {
	    l = KAMConfig::self()->rawLong( mix->card->configSection(), "EnumeratedValue", -1, id );
	    if ( l!=-1 )
	        setEnumValue ( l );
	}
    }
    
    if ( KAMConfig::self()->debugInfo ) {
        kdDebug()<<"Item: id "<<id<<", "<<name()<<endl;
        snd_mixer_elem_get_type ( aelem );
        kdDebug()<<"  SND_MIXER_ELEM_SIMPLE: "<<(snd_mixer_elem_get_type ( aelem )==SND_MIXER_ELEM_SIMPLE)<<endl
	         <<"  pb: "<<canPlayback<<", cp: "<<canCapture<<endl
	         <<"  pb status: "<<(muted(KMixConst::Playback,KAMIX_ALL_CHANNELS)?"off":"on")<<", "
	           "cp status: "<<(muted(KMixConst::Capture,KAMIX_ALL_CHANNELS)?"off":"on")<<endl;
	
	if ( hasVolume(KMixConst::Playback) ) {
	    kdDebug()<<"  pb volume, range: "<<minPVolume<<"<->"<<maxPVolume<<endl;
	    kdDebug()<<"  pb dB, range: "<<minPdB<<"<->"<<maxPdB<<endl;
	}
	if ( hasVolume(KMixConst::Capture) ) {
	    kdDebug()<<"  cp volume, range: "<<minCVolume<<"<->"<<maxCVolume<<endl;
	    kdDebug()<<"  cp dB, range: "<<minCdB<<"<->"<<maxCdB<<endl;
	}
	if ( hasVolume(KMixConst::Common) )
	    kdDebug()<<"  co volume, range: "<<minCVolume<<"<->"<<maxCVolume<<endl;
	if ( hPEnum )
            kdDebug()<<"  pb enumerated"<<endl;
	if ( hCEnum )
            kdDebug()<<"  cp enumerated"<<endl;

	if ( hasSwitch(KMixConst::Playback) ) {
	    kdDebug()<<"  pb switch present"<<endl;
	    if ( hasSwitchJoined(KMixConst::Playback ) )
	        kdDebug()<<"    switch is joined"<<endl;
	}
	if ( hasSwitch(KMixConst::Capture) ) {
	    kdDebug()<<"  cp switch present"<<endl;
	    if ( hasSwitchJoined(KMixConst::Capture ) )
	        kdDebug()<<"    switch is joined"<<endl;
	}
	if ( hasSwitch(KMixConst::Common) )
	    kdDebug()<<"  co switch present"<<endl;
	
	if ( isActive )
	    kdDebug()<<"  active"<<endl;
	else
	    kdDebug()<<"  **inactive**"<<endl;
	
	unsigned int ii;
	kdDebug()<<"  pb channels: "<<pbChannels.count()<<endl;
	for ( ii=0 ; ii<pbChannels.count(); ii++ )
	    kdDebug()<<"    ch "<<ii<<" "<<pbChannels.at(ii)->name()<<endl;
	kdDebug()<<"  cp channels: "<<cpChannels.count()<<endl;
	for ( ii=0 ; ii<cpChannels.count(); ii++ )
	    kdDebug()<<"    ch "<<ii<<" "<<cpChannels.at(ii)->name()<<endl;
    }

    initialized = true;
}

KMixerItem::~KMixerItem()
{
    unsigned int i;
    for ( i=0; i<pbChannels.count(); i++ )
        delete pbChannels.at(i);
    for ( i=0; i<cpChannels.count(); i++ )
        delete cpChannels.at(i);
}

const QString& KMixerItem::alsaName() {
	return aName;
}

const QString& KMixerItem::customName() {
	return cName;
}

const QString& KMixerItem::name ()
{
    if ( cName=="" )
        return aName;
    else
        return cName;
}

long KMixerItem::maxVolume(KMixConst::Sense sns)
{
    if ( sns==KMixConst::Playback )
        return maxPVolume;
    else
        return maxCVolume;
}

long KMixerItem::minVolume(KMixConst::Sense sns)
{
    if ( sns==KMixConst::Playback )
        return minPVolume;
    else
        return minCVolume;
}

int KMixerItem::muted(KMixConst::Sense sns, unsigned int chan )
{
    int sw;
    unsigned int sw2;
    int j;
    if ( channels(sns)->count()==1 ) {
        // If item has no switch caps, it is unmuted by default
    	if ( sns==KMixConst::Playback && !( hPSwitch || hCoSwitch ) )
    		return KMixConst::Active;
        if ( sns==KMixConst::Capture  && !( hCSwitch || hCoSwitch ) )
        	return KMixConst::Active;
    	
    	// Single channel item
    	if ( sns==KMixConst::Playback && ( hPSwitch || hCoSwitch ) )
            snd_mixer_selem_get_playback_switch( aelem, (SNDCHIDT)0, &sw );
        if ( sns==KMixConst::Capture && ( hCSwitch || hCoSwitch ) )
            snd_mixer_selem_get_capture_switch( aelem, (SNDCHIDT)0, &sw );
        
        if ( sw == 1 )
            return KMixConst::Active;
        else
            return KMixConst::Muted;
    }
    if ( channels(sns)->count()>1 ) {
        // Multiple channels item
        if ( chan == KAMIX_ALL_CHANNELS ) {
            // See if all channels are in the same state
            j = channels(sns)->at(0)->muted();
            for ( sw2=1; sw2 < channels(sns)->count(); sw2++ ) {
                if ( j!=channels(sns)->at(sw2)->muted() ) {
                    return KMixConst::Partial;
                }
            }
            return j;
        } else {
            sw = channels(sns)->at(chan)->muted();
            if ( sw==KMixConst::Active )
                return KMixConst::Muted;
            else
                return KMixConst::Active;
        }
    }
    
    return KMixConst::Unknown;
}

void KMixerItem::setMuted(KMixConst::Sense sns, int m, unsigned int chan )
{
    int am;
    
    if ( muted(sns,chan) == m )
        return;
    
    switch ( m ) {
    case KMixConst::Active:
        am = 1;
        break;
    default:
        am = 0;
    }
    
    if ( chan == KAMIX_ALL_CHANNELS ) {
#ifdef HAS_SND_VOLUME_ALL
        if ( sns == KMixConst::Playback && (hPSwitch || hCoSwitch) )
            snd_mixer_selem_set_playback_switch_all ( aelem, am );
        else
            snd_mixer_selem_set_capture_switch_all ( aelem, am );
#else
        unsigned int j;	
        for ( j=0 ; j<channels(sns)->count() ; j++ ) {
            channels(sns)->at(j)->setMuted ( !am );
	}
#endif
    } else {
        channels(sns)->at(chan)->setMuted ( !am );
    }
    
    if ( initialized )
        emit changed(this);

}

void KMixerItem::toggleMuted ( KMixConst::Sense sns, unsigned int chan ) {
    if ( chan == KAMIX_ALL_CHANNELS ) {
        if ( muted(sns,chan)==KMixConst::Muted )
            setMuted ( sns, KMixConst::Active, chan );
        else
            setMuted ( sns, KMixConst::Muted, chan );
    } else
        channels(sns)->at(chan)->toggleMuted();
}

bool KMixerItem::hasSwitch(KMixConst::Sense sns)
{
    switch ( sns ) {
    case KMixConst::Playback:
        return hPSwitch;
	break;
    case KMixConst::Capture:
        return hCSwitch;
	break;
    case KMixConst::Common:
        return hCoSwitch;
	break;
    default:
        return false;
    }
}

bool KMixerItem::hasSwitchJoined(KMixConst::Sense sns)
{
    switch ( sns ) {
    case KMixConst::Playback:
        return hPSwitchJoined;
	break;
    case KMixConst::Capture:
        return hCSwitchJoined;
	break;
    default:
        return false;
    }
}

bool KMixerItem::mono(KMixConst::Sense sns)
{
    if ( sns==KMixConst::Playback ) 
        return ( pbChannels.count()==1 );
    else
        return ( cpChannels.count()==1 );
    
}

bool KMixerItem::hasVolume( KMixConst::Sense sns )
{
    switch ( sns ) {
    case KMixConst::Playback:
        return hPVolume;
	break;
    case KMixConst::Capture:
        return hCVolume;
	break;
    case KMixConst::Common:
        return hCoVolume;
	break;
    default:
        return -1;
    }
}

bool KMixerItem::hasEnum( KMixConst::Sense sns )
{
    return ( ( (sns==KMixConst::Playback && hPEnum)||(sns==KMixConst::Capture && hCEnum))  && valueNames.count()>0 );
}

void KMixerItem::setVolume ( KMixConst::Sense sns, long val, unsigned int chan )
{

    if ( volume(sns)==val )
        return;
	
    if ( val == -1 )
        return;
    if ( sns==KMixConst::Playback && !hPVolume )
    	return;
    if ( sns==KMixConst::Capture && !hCVolume )
        return;
    
    if ( chan == KAMIX_ALL_CHANNELS ) {
#ifdef HAS_SND_VOLUME_ALL
	if ( sns==KMixConst::Playback )
	    snd_mixer_selem_set_playback_volume_all ( aelem, val );
	else
	    snd_mixer_selem_set_capture_volume_all ( aelem, val );
#else
#warning "No _all_ functions..."
        unsigned int j;
        // Changes the volume for all the channels of this item
	for ( j=0 ; j<channels(sns)->count() ; j++ ) {
            channels(sns)->at(j)->setVolume ( val );
	}        
#endif
    } else {
        channels(sns)->at(chan)->setVolume ( val );
    }
    
    if ( initialized ) {
        emit changed(this);
    }
    
}

long KMixerItem::volume ( KMixConst::Sense sns, unsigned int chan ) {
    long v;
    
    if ( chan == KAMIX_ALL_CHANNELS ) {
        if ( sns == KMixConst::Playback )
            snd_mixer_selem_get_playback_volume ( aelem, (snd_mixer_selem_channel_id_t)0, &v );
	else
            snd_mixer_selem_get_capture_volume  ( aelem, (snd_mixer_selem_channel_id_t)0, &v );
	return v;
    }
    
    if (( sns == KMixConst::Playback && hPVolume && chan<pbChannels.count() ) ||
        ( sns == KMixConst::Capture  && hCVolume && chan<cpChannels.count() ) )
        return channels(sns)->at(chan)->volume();
    else
        return -1;
}

#ifdef HAS_SND_DECIBEL
double KMixerItem::dB( KMixConst::Sense sns, unsigned int chan )
{
    long v;
    float d;

    if ( chan == KAMIX_ALL_CHANNELS ) {
        if ( sns == KMixConst::Playback ) {
            if ( minPdB==maxPdB )
                return KAMIX_NO_DB;
            snd_mixer_selem_get_playback_dB ( aelem, (snd_mixer_selem_channel_id_t)0, &v );
	} else {
            if ( minCdB==maxCdB )
                return KAMIX_NO_DB;
            snd_mixer_selem_get_capture_dB  ( aelem, (snd_mixer_selem_channel_id_t)0, &v );
        }
        d = v;
        d = d/100;
        return d;
    }
    
    if (( sns == KMixConst::Playback && hPVolume && chan<pbChannels.count() ) ||
        ( sns == KMixConst::Capture  && hCVolume && chan<cpChannels.count() ) )
        return channels(sns)->at(chan)->dB();
    else
        return -1;
}
#endif

long KMixerItem::percent( KMixConst::Sense sns, unsigned int chan )
{
    long v = maxVolume(sns);
    if ( v )
        return long(volume(sns,chan)*100/v);
    else
    	return 0;
}
void KMixerItem::setAlsaName ( const QString& name ) {
    aName = name;
}

void KMixerItem::setCustomName( const QString& name ) {
    cName = name;
}
void KMixerItem::setEnumValue ( int en )
{
    if ( (!(hPEnum||hCEnum)) || enumValue()==en )
        return;

    snd_mixer_selem_set_enum_item ( aelem, (snd_mixer_selem_channel_id_t)0, en );
    
    if ( initialized )
        emit changed(this);
}

int KMixerItem::enumValue ()
{
    if ( !(hPEnum||hCEnum) )
        return 0;
	
    unsigned int i;
    snd_mixer_selem_get_enum_item ( aelem, (snd_mixer_selem_channel_id_t)0, &i );
    return i;
}

void KMixerItem::mixerChanged()
{
    if ( initialized )
        emit changed(this);
}

bool KMixerItem::hasCaptureCapabilities() {
    return ( hCoVolume || hCoSwitch || hCSwitch || hCVolume || hasEnum(KMixConst::Capture) );
}

bool KMixerItem::hasPlaybackCapabilities() {
    return ( hCoVolume || hCoSwitch || hPSwitch || hPVolume || hasEnum(KMixConst::Playback) );
}

QPtrList<KChannel>* KMixerItem::channels(KMixConst::Sense sns) {
    if ( sns == KMixConst::Playback )
        return &pbChannels;
    else
        return &cpChannels;
}

#include "kmixeritem.moc"
