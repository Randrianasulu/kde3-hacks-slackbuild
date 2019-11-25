/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KLEDEX_H
#define KLEDEX_H

//#include <kled.h>
#include <qpixmap.h>
#include <qwidget.h>
#include "kmixconst.h"

/**
@author Stefano Rivoir
*/
class KLedEx : public QWidget
{
Q_OBJECT
public:

    enum Color {
    	green = 0,
	red,
	yellow
    } _color;
    
    KLedEx(QWidget *parent, const KLedEx::Color, int size=16 );
    ~KLedEx();

    QPixmap pon, poff, ppart;

    void setOn();
    void setOff();
    void setPartial();
    
    KMixConst::MuteType status;
    
protected:
    void mousePressEvent ( QMouseEvent* );
    void paintEvent ( QPaintEvent* );

signals:
    void clicked();

};

#endif
