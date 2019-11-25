/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KVUMETER_H
#define KVUMETER_H

#include <qwidget.h>
#include <qcolor.h>
#include <kled.h>
#include <qdatetime.h>

/**
@author Stefano Rivoir
*/
class KVUMeter : public QWidget
{
Q_OBJECT
public:
    KVUMeter(QWidget *parent = 0, const char *name = 0);
    ~KVUMeter();

    void setValueLeft( float );
    void setValueRight( float );
    void setScale( float );
    void paintEvent ( QPaintEvent* );
    void checkPeakTime();
    
    /*
    float levelToDb ( float );
    float DbToLevel ( float );
    */
    
    QColor c1, c2, c3, c4;

private:
    float valueLeft, valueRight;
    float scale;
    float peakLeft, peakRight;
    int   peakFalloff;
    QTime peakTime;
    
};

#endif
