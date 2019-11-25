/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "alsacb.h"
#include <kalsa.h>
#include "kalsamix.h"

kalsamix *app;

int alsa_callback ( snd_mixer_t*, unsigned int, snd_mixer_elem_t*)
{
    // Here we should reread the whole thing!
    app->reinitApp();
    return 0;
}

int alsa_callback_item ( snd_mixer_elem_t* el, unsigned int mk )
{
    if ( mk == 1 ) {
	unsigned int i;
	for ( i=0; i<KAlsa::self()->cardsCount(); i++ )
   	    KAlsa::self()->card(i)->mixer()->itemChanged ( mk, el );
    }
    return 0;
}

int alsa_callback_setup ( kalsamix *ap )
{
    app  = ap;
    return 0;
}

