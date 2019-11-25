/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kmixer.h"
#include "kamconfig.h"

KMixer::KMixer( KCard *c )
{
    isOpen = false;
    card = c;
}

KMixer::~KMixer()
{
    if ( isOpen )
        close();
}

int KMixer::open()
{
    unsigned int j;

    snd_mixer_elem_t *elem;

    KMixerItem *mixitm;

    aerr = snd_mixer_open ( &amix, 0 );
    if ( aerr<0 )
        return 1;

    aerr = snd_mixer_attach ( amix, card->deviceName() );
    if ( aerr<0 )
        return 2;

    aerr = snd_mixer_selem_register ( amix, NULL, NULL );
    if ( aerr<0 )
        return 3;

    aerr = snd_mixer_load ( amix );
    if ( aerr<0 )
        return 4;

    snd_mixer_set_callback ( amix, alsa_callback );

    j = 0;
    for ( elem=snd_mixer_first_elem (amix); elem; elem=snd_mixer_elem_next(elem) ) {
        if ( snd_mixer_elem_get_type ( elem )==SND_MIXER_ELEM_SIMPLE &&
             snd_mixer_selem_is_active(elem) ) {

            snd_mixer_elem_set_callback ( elem, alsa_callback_item );
            mixitm = new KMixerItem( elem, this, j++ );
            connect ( mixitm, SIGNAL(changed(KMixerItem*)), this, SLOT(itemChanged(KMixerItem*)) );
            items.append ( mixitm );
        }
    }

    isOpen = true;
    return 0;

}

unsigned int KMixer::elemCount()
{
    return items.count();
}

void KMixer::close()
{
    unsigned int i;
    for ( i=0; i<items.count(); i++ )
        delete items.at(i);
    items.clear();

    aerr = snd_mixer_detach ( amix, "default" );
    aerr = snd_mixer_close ( amix );
    snd_mixer_free ( amix );
    isOpen = false;
}

void KMixer::itemChanged ( KMixerItem *itm ) {
    emit itemHasChangedInt ( itm );
}

// Callback for ALSA events, force item to reread itself
void KMixer::itemChanged ( unsigned int, snd_mixer_elem_t* elem )
{
    unsigned int i;
    for ( i=0 ; i<items.count(); i++ ) {
        if ( items.at(i)->aelem == elem ) {
            items.at(i)->mixerChanged();
	    if ( KAMConfig::self()->showOSD ) {
	        emit itemHasChangedExt ( items.at(i) );
	    }
	    return;
	}
    }
}

KMixerItem* KMixer::itemByName( QString s ) {
    unsigned int i;
    for ( i=0; i<items.count(); i++ ) {
        if ( items.at(i)->name() == s )
	    return items.at(i);
    }
    return 0;
}

KMixerItem* KMixer::itemById( long id ) {
    unsigned int i;
    for ( i=0; i<items.count(); i++ ) {
        if ( items.at(i)->id == id )
	    return items.at(i);
    }
    return 0;
}

#include "kmixer.moc"
