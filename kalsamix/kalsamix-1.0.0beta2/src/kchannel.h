/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/*
 * This class represents the single channel of a mixer item. I has function
 * to get and set the volumes both for playback and for capture mode
 */

#ifndef KCHANNEL_H
#define KCHANNEL_H

#include <qstring.h>
#include "kmixconst.h"
#include "config.h"

class KMixerItem;

/**
@author Stefano Rivoir
*/
class KChannel{
private:

    KMixerItem *mixitm;

public:

    enum KMixConst::Sense sense;

    KChannel( KMixerItem *itm, int id, KMixConst::Sense sns );
    ~KChannel();

    const 	QString name();
    unsigned int id;

    long	volume();
#ifdef HAS_SND_DECIBEL
    long	dB();
#endif
    long	percent();

    void	setVolume (long vol);
    int		muted();
    void	setMuted ( int m );
    void	toggleMuted();
    
};

#endif
