/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef _ALSACB_H
#define _ALSACB_H

#include <alsa/asoundlib.h>

class kalsamix;

int alsa_callback ( snd_mixer_t* mx, unsigned int mk, snd_mixer_elem_t* el);
int alsa_callback_setup ( kalsamix* );
int alsa_callback_item ( snd_mixer_elem_t* el, unsigned int mk );

#endif
