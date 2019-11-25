/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KAMSYSTEMTRAY_H
#define KAMSYSTEMTRAY_H

#include <ksystemtray.h>

/**
@author Stefano Rivoir
*/
class KAMSystemTray : public KSystemTray
{
Q_OBJECT
public:
    KAMSystemTray(QWidget *parent = 0, const char *name = 0);
    ~KAMSystemTray();
    
    void wheelEvent ( QWheelEvent* );
    void mousePressEvent( QMouseEvent* );
  
signals:
    void wheelUp();
    void wheelDown();
    void midButtonPressed();

};

#endif
