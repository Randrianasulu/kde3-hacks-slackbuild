/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KGROUPWIDGET_H
#define KGROUPWIDGET_H

#include <qhgroupbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qptrlist.h>

#include "kvertlabel.h"
#include "kitemsgroup.h"
#include "kmixerslider.h"

/**
@author Stefano Rivoir
*/
class KGroupWidget : public QWidget
{
Q_OBJECT

private:

    bool	_expanded;
    KVertLabel*  lside;
    KVertLabel*  lhead;
    QVBox*	 vbox;
    bool	 showFrame;
    QGridLayout* hgrid;
    QPtrList<KMixerSlider> sliders;
    QValueList<long>  cols;

public:

    KGroupWidget(QWidget *parent = 0, const char *name = 0, bool e = true, bool dark = false );
    ~KGroupWidget();

    bool	expanded();
    void	setExpanded(bool);
    void	setTitle ( const QString& );
    void	addItem ( KMixerSlider* itm, long pos );
    
protected:
    void        paintEvent ( QPaintEvent* );
      
public slots:
    void	slotToggle();
    
};

#endif
