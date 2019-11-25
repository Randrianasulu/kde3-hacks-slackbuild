/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KMIXERVIEW_H
#define KMIXERVIEW_H

#include "kmixer.h"
#include "kmixerslider.h"
#include "kvertlabel.h"
#include "kgroupwidget.h"

#include <qwidget.h>
#include <qtabwidget.h>
#include <qptrlist.h>
#include <qhbox.h>
#include <qlayout.h>
#include "kosd.h"

/**
@author Stefano Rivoir
*/

class KMixerView : public QWidget
{
Q_OBJECT
private:

    KMixer *_mixer;
    QTabWidget *tab;
    QHBox *hbox1, *hbox2;
    
    QPtrList<KMixerSlider> sliders;
    QPtrList<KGroupWidget> pgroupsw;
    QPtrList<KGroupWidget> cgroupsw;
    
    QGridLayout *layout;
    KVertLabel *cname;
    
public:

    KOsd   osd;
    bool   osdEnabled;
    QString configSect;
    
    enum viewType {
        viewTabs      = 0,
	viewTiledVert = 1,
	viewTiledHor  = 2
    } type;
    
    KMixerView(KMixer* mix, KMixerView::viewType vt, QWidget *parent = 0, const char *name = 0, QWidget *tray = 0);
    ~KMixerView();
    
    void initControls();
    
public slots:
    void osdNotify ( KMixerItem* );
};

#endif
