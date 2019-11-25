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
 * This is a subclassed slider class, with a modified signal (changed) that
 * differs from the original (valueChange) in the fact that it sends also
 * the pointer to itself (in KMixerSlider there's an array of KSliderEx,
 * everyone connected to a certain channel, so I have to know WHERE the
 * event occourred).
 */

#ifndef KSLIDEREX_H
#define KSLIDEREX_H

#include <qslider.h>
#include "kchannel.h"

/**
@author Stefano Rivoir
*/
class KSliderEx : public QSlider
{
Q_OBJECT
public:
    KSliderEx(QWidget *parent, KChannel *ch );

    ~KSliderEx();

    KChannel *channel;

    void mousePressEvent( QMouseEvent *ev );
    void mouseReleaseEvent ( QMouseEvent *ev );
    
    void setMixerValue( int val );
    
    bool mid_pressed;
    
private slots:
    void valChanged( int val );
    
signals:
    void changed( KSliderEx *sli, int val, int all );
    void rightClick();
    
};

#endif
