/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KMIXER_H
#define KMIXER_H

#include <alsa/asoundlib.h>
#include <qptrlist.h>
#include <qwidget.h>

#include "kcard.h"
#include "kmixeritem.h"
#include "alsacb.h"
#include "kitemsgroup.h"

class KCard;
class KItemsGroup;

/**
 * @short Class that holds all of the mixer items
 * @author Stefano Rivoir
 */
class KMixer : public QWidget {
Q_OBJECT
private:

    char	acard[64];
    int		aerr;
    
public:

    KCard 	*card;
    
    /**
      @brief Standard ctor
    */
    KMixer( KCard *c );
    
    /**
      @brief Standard dtor
    */
    ~KMixer();

    /**
      @brief Initializes and opens the mixer
    */
    int  open();
    
    /**
      @brief Closes mixer resources
    */
    void close();
    
    /**
      @brief Number of elements found
    */
    unsigned int  elemCount();
    
    /**
      @brief Pointer to ALSA library structure
    */
    snd_mixer_t	*amix;

    /**
      @brief Tells if the mixer has been opened
    */
    bool isOpen;
    
    /**
      @brief List of pointers to the mixer elements
    */
    QPtrList<KMixerItem>   items;

    /**
      @brief List of groups
    */
    QPtrList<KItemsGroup>  pgroups;
    QPtrList<KItemsGroup>  cgroups;
      
    /**
      @brief Returns a pointer to the named element
      @param name Name of the element to search
    */
    KMixerItem* itemByName ( QString name );
    KMixerItem* itemById ( long id );
    
    /**
      @brief This is the callback function that ALSA will execute when an item has changed
    */
    void itemChanged ( unsigned int mk, snd_mixer_elem_t* el );
    
public slots:
    void itemChanged ( KMixerItem* );
    
signals:
    void itemHasChangedInt ( KMixerItem* itm );
    void itemHasChangedExt ( KMixerItem* itm );
    
};

#endif
