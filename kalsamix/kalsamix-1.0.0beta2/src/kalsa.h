/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KALSA_H
#define KALSA_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>
#include <alsa/global.h>

#include "kcard.h"
#include "kmixeritem.h"

/**
@author Stefano Rivoir
*/
class KAlsa : public QObject
{
Q_OBJECT

private:

    QPtrList<KCard> cards;
    KAlsa();
    ~KAlsa();

    static KAlsa *me;

public:

    enum ALSA_COMPAT {
        ALSAC_INITIAL,
        ALSAC_FROM_1_0_9,
	ALSAC_FROM_1_0_14rc
    };

    static KAlsa *self();

    const char* version();
    int   compat;
    unsigned int   cardsCount();
    KCard *card(int);

    int   init();
    int   defaultCard();
    int   shortItemId( int );
    KMixerItem* shortItem( int );

};

#endif
