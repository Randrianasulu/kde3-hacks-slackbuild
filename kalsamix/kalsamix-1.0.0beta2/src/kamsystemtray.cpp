/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kamsystemtray.h"

KAMSystemTray::KAMSystemTray(QWidget *parent, const char *name)
 : KSystemTray(parent,name)
{
}

KAMSystemTray::~KAMSystemTray()
{
}

void KAMSystemTray::wheelEvent( QWheelEvent *ev ) {

    if ( ev->delta() > 0 )
        emit wheelUp();
    if ( ev->delta() < 0 )
        emit wheelDown();
}

void KAMSystemTray::mousePressEvent( QMouseEvent *ev ) {
    
    if ( ev->button() == QEvent::MidButton && ev->type()==QEvent::MouseButtonPress ) {
        emit midButtonPressed();
        return;
    }
    if ( ev->button() == QEvent::LeftButton && ev->type()==QEvent::MouseButtonPress ) {
        if ( ((QWidget*)parent())->isVisible() )
            ((QWidget*)parent())->hide();
	else
	    ((QWidget*)parent())->show();
	return;
    }

    KSystemTray::mousePressEvent(ev);
}

