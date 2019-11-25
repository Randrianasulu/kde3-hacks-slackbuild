/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "kalsa.h"
#include "alsa/global.h"
#include "alsa/version.h"
#include "config.h"
#include <qstringlist.h>

#include <klocale.h>
#include "kamconfig.h"

KAlsa *KAlsa::me = 0;

KAlsa::KAlsa()
{
}

KAlsa *KAlsa::self() {
    if ( !me )
        me = new KAlsa();
    return me;
}

KAlsa::~KAlsa()
{
    unsigned int i;
    for ( i=0; i<cards.count(); i++ )
        delete cards.at(i);
}

const char* KAlsa::version() {
#ifdef HAS_SND_ASOUNDLIB_VERSION
    return snd_asoundlib_version();
#else
    return i18n("ALSA version is not available");
#endif
}

#include <kdebug.h>
int KAlsa::init() {
    unsigned int i;
    unsigned int j;
    int gret;
    
    KCard *c;
    char *buf;
    QString name;
    QString hwName;
    QString v, v1, v2, v3, v32;
    QStringList vs;
    
    for ( i=0; i<cards.count(); i++ )
        delete cards.at(i);
    cards.clear();

    compat = ALSAC_INITIAL;

    // snd_card_load ret value changed since 1.0.10rc1
    gret = 0;
    v = version();
    if ( v.at(0)<='9' ) {
        vs = QStringList::split ( ".",v );
        if ( vs.count()==3 ) {
            v1 = *(vs.at(0));
            v2 = *(vs.at(1));
            v3 = *(vs.at(2));

            // Strip extraversion
            for ( j=0 ; j<v3.length() ; j++ ) {
                if ( v3.at(j)<='9' )
                    v32 += v3.at(j);
                else
                    j = 9999;
            }

            if ( ( v1.toLong()==0 ) || // 0.x.y
                 ( v1.toLong()==1 && v2.toLong()==0 && v32.toLong()<=9 ) // Up to 1.0.9
               )
                gret = 0;
            else
                gret = 1;
        }
    }

    for ( i=0; i<10; i++ ) {
        if ( snd_card_load(i)==gret ) {
	    hwName.sprintf ( "hw:%d",i ); 
	    snd_card_get_name ( i, &buf );
	    name = buf;
	    c = new KCard( hwName );
	    c->setName( name );
	    if ( c->openMixer() ) {
	        if ( c->mixer()->items.count()>0 ) {
	            cards.append(c);
		    j++;
		} else
		    delete ( c );
	    } else {
	        delete ( c );
            }
	}
    }
    return j;
}

unsigned int KAlsa::cardsCount() {
    return cards.count();
}

int KAlsa::shortItemId( int card ) {
    return KAMConfig::self()->rawLong( cards.at(card)->configSection(), "ShortItem", 0 );
}

KMixerItem* KAlsa::shortItem( int card ) {
    return cards.at(card)->mixer()->itemById( KAMConfig::self()->rawLong( cards.at(card)->configSection(), "ShortItem", 0 ) );
}

int KAlsa::defaultCard() {
    return KAMConfig::self()->defaultCard;
}

KCard* KAlsa::card(int i) {
    return cards.at(i);
}

#include "kalsa.moc"
