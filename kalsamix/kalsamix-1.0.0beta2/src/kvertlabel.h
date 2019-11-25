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
 * A vertical label borrowed from KMix (thanks :)
 */

#ifndef KVERTLABEL_H
#define KVERTLABEL_H

#include <qwidget.h>
#include <qstring.h>
#include <qfont.h>

/**
@author Stefano Rivoir
*/
class KVertLabel : public QWidget
{
Q_OBJECT
public:
    
    enum VLType {
        Item       = 0,
	Chan       = 1,
	GroupShown = 2,
	GroupHidden=3,
	Card       = 4
    } labelType;

    KVertLabel(QWidget *parent, KVertLabel::VLType lt, const char *name = 0, WFlags f = 0 );

    ~KVertLabel();

    const QString& text();
    void  setText ( const QString& );
    bool  isRed;
    bool  bottomLine;
    
    
    enum VLOrientation {
        Vertical = 0,
	Horizontal = 1
    } orientation;
    
private:
    QString txt;
    QFont  fnt;
    
signals:
    void clicked();
    
protected:
    void paintEvent ( QPaintEvent *);
    void mousePressEvent ( QMouseEvent * );
    
};

#endif
