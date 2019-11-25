/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KMASTERVIEW_H
#define KMASTERVIEW_H

#include <kdatetbl.h>
#include <qvbox.h>

#include "kmasterview.h"
#include "kmixerslider.h"
#include "kmixeritem.h"

/**
@author Stefano Rivoir
*/
class KMasterView : public KPopupFrame
{
Q_OBJECT
public:

    KMasterView(KMixerItem* itm, QWidget *parent = 0, const char *name = 0);
    ~KMasterView();
    KMixerSlider *sli;    
    QVBox	 *vb;
    KMixerItem   *pitm;
    
    void popup (const QPoint &pos);
    void hideEvent ( QHideEvent *ev );
};

#endif
