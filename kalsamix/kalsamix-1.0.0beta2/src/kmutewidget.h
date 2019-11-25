/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KMUTEWIDGET_H
#define KMUTEWIDGET_H

#include <qwidget.h>
#include "kledex.h"
#include "kchannel.h"
#include "kmixeritem.h"

/**
@author Stefano Rivoir
*/
class KMuteWidget : public QWidget
{
Q_OBJECT

private:

    KLedEx	*led;
    
    void *_host;
    
public:
    
    enum hostType_ {
        Channel,
	Item
    } hostType;
    
    KMuteWidget(QWidget *parent, KLedEx::Color, unsigned int size=16 );
    ~KMuteWidget();

    void setHost ( KChannel* );
    void setHost ( KMixerItem* );
    void *host();
    void setMute ( int );
    
public slots:
    void slotMute();

signals:
    void mute(KMuteWidget*);
    
};

#endif
