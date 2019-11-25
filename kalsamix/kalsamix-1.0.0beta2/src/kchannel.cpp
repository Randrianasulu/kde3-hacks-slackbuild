/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "kmixconst.h"
#include "kchannel.h"
#include "kmixeritem.h"
#include "config.h"

#define SNDCHIDT snd_mixer_selem_channel_id_t

KChannel::KChannel( KMixerItem *itm, int cid, KMixConst::Sense sns )
{
    mixitm = itm;
    id = cid;
    sense = sns;
    
}

KChannel::~KChannel()
{
}

const QString KChannel::name()
{
    return snd_mixer_selem_channel_name ( (SNDCHIDT)id );
}

void KChannel::toggleMuted()
{
    if ( muted()==KMixConst::Active )
        setMuted ( KMixConst::Muted );
    else
        setMuted ( KMixConst::Active );
}

long KChannel::volume()
{
    long v = 0;
    if ( sense == KMixConst::Playback && mixitm->canPlayback && mixitm->hasVolume(KMixConst::Playback) )
        snd_mixer_selem_get_playback_volume ( mixitm->aelem, (SNDCHIDT)id, &v );
    if ( sense == KMixConst::Capture && mixitm->canCapture && mixitm->hasVolume(KMixConst::Capture) )
        snd_mixer_selem_get_capture_volume ( mixitm->aelem, (SNDCHIDT)id, &v );
    return v;
}

long KChannel::percent() 
{
    long v;
    v = mixitm->maxVolume(sense);
    if ( v )
        return volume()*100/v;
    else
        return 0;
}

int KChannel::muted()
{
    int i;
    
    if ( sense == KMixConst::Playback ) 
        snd_mixer_selem_get_playback_switch ( mixitm->aelem, (SNDCHIDT)id, &i );
    else
        snd_mixer_selem_get_capture_switch ( mixitm->aelem, (SNDCHIDT)id, &i );
    if ( i )
        return KMixConst::Active;
    else
        return KMixConst::Muted;
}

#ifdef HAS_SND_DECIBEL
long KChannel::dB()
{
    long v = -1;
    if ( sense == KMixConst::Playback && mixitm->canPlayback && mixitm->hasVolume(KMixConst::Playback) )
        snd_mixer_selem_get_playback_dB ( mixitm->aelem, (SNDCHIDT)id, &v );
    if ( sense == KMixConst::Capture && mixitm->canCapture && mixitm->hasVolume(KMixConst::Capture) )
        snd_mixer_selem_get_capture_dB ( mixitm->aelem, (SNDCHIDT)id, &v );
    if ( v < 0 )
        return -1;
    else
        return v/100;
}
#endif

void KChannel::setVolume ( long vol )
{
    if ( sense==KMixConst::Playback && mixitm->hasVolume(KMixConst::Playback) ) {
        snd_mixer_selem_set_playback_volume ( mixitm->aelem, (SNDCHIDT)id, vol );
            return;
    }
    
    if ( sense==KMixConst::Capture && mixitm->hasVolume(KMixConst::Capture ) )
        snd_mixer_selem_set_capture_volume ( mixitm->aelem, (SNDCHIDT)id, vol );
}

void KChannel::setMuted ( int m )
{
    int am; 
    am = ( m==KMixConst::Active ? 1 : 0 );
    if ( sense==KMixConst::Playback ) {
        snd_mixer_selem_set_playback_switch ( mixitm->aelem, (SNDCHIDT)id, am );
    } else {
        snd_mixer_selem_set_capture_switch ( mixitm->aelem, (SNDCHIDT)id, am );
    }
    emit ( mixitm->mixerChanged() );
}
