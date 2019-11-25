/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KMIXCONST_H
#define KMIXCONST_H

/**
@author Stefano Rivoir
*/
class KMixConst{
public:
    KMixConst();

    ~KMixConst();

    enum Sense {
        Playback,
	Capture,
	Common
    };

    enum MuteType {
        Muted,
        Active,
        Partial,
        Unknown
    };
    
};

#endif
