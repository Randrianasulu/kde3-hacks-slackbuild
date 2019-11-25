/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KOSD_H
#define KOSD_H

#include <qwidget.h>
#include <qstring.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qfont.h>
#include <qcolor.h>

#include "kamconfig.h"

/**
@author Stefano Rivoir
*/
class KOsd : public QWidget
{
Q_OBJECT
private:

    QString	osdText;
    QTimer	tmr;
    QPixmap     bmap;
    QBitmap     mask;
    QFont       fnt;
    QColor	colf, colb;
    
public:
    KOsd(QWidget *parent = 0, const char *name = 0);

    ~KOsd();

    int  osdpos;
    int  osdX, osdY;
    void setText( const QString& );
    void prepareBitmap();
    void showOsd();
    bool isActive;
    
public slots:
    void timeout();
    void readConfig();
    
protected:
    void paintEvent(QPaintEvent*);

};

#endif
