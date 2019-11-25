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
 * This class is the graphical element for a mixer items. It displays every
 * information about the item and its channels (names, volumes etc.).
 */

#ifndef KMIXERSLIDER_H
#define KMIXERSLIDER_H

#include <qwidget.h>
#include <qhbox.h>
#include <qptrlist.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <kaction.h>

#include <kpopupmenu.h>
#include "ksliderex.h"
#include "kmixconst.h"
#include "kledex.h"
#include "kvertlabel.h"
#include "kmixeritem.h"
#include "kmutewidget.h"

/**
@author Stefano Rivoir
*/
class KMixerSlider : public QWidget
{
Q_OBJECT
public:

    enum KMixConst::Sense sense;

    KMixerSlider(QWidget *parent,
        KMixerItem* itm,
	enum KMixConst::Sense, const char *name = 0, WFlags f = 0);

    ~KMixerSlider();

    QHBox	*hbox, *hbox2, *hbox3;
    QVBox	*vbox;

    KVertLabel	*litm;
    KMixerItem  *mixitm;
    int		mixitmid;
    QString	configSect;
    
    QComboBox   *cval;
    KMuteWidget *lMute;
    KLedEx      *lbal;

    QPtrList<KSliderEx>		sliders;
    QPtrList<KMuteWidget>	chanLeds;
    QPtrList<KVertLabel>	labels;

    KPopupMenu  *pm;
    KAction     *aHide, *aMute, *aSplit;
    
    bool        enableMenu;
    bool        showSplitted;
    
    int		pborder, cporder;
    
protected:
    void paintEvent ( QPaintEvent* );
    
signals:
    void muteClicked( KMixConst::Sense, unsigned int chan );
    void volumeChanged ( KMixConst::Sense, long val, unsigned int chan );
    void valueChanged( int );
    
public slots:
    void sliderChanged( KSliderEx* sli, int val, int shift );
    void balanceChanged ();
    void muteChanged ( KMuteWidget* );
    void muteChangedAll ();
    void enumChanged ( );
    void itemChanged (KMixerItem*);
    void hideItem();
    void showMenu();

};

#endif
