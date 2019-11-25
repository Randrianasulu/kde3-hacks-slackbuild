/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kamuapp.h"
#include "kalsamix.h"

#include <kcmdlineargs.h>
#include <klocale.h>
#include <kpassivepopup.h>
#include <qpixmap.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "kamconfig.h"

int KAMUApp::newInstance()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    
    kalsamix *mainWin = 0;

    if ( !mainWidget() )
        mainWin = new kalsamix();
    
    setMainWidget( mainWin );

    if ( KAMConfig::self()->restoreToSystray || KAMConfig::self()->alwaysSystray ) {
        mainWin->hide();
	
	if ( !KAMConfig::self()->alwaysSystray )
	    KPassivePopup::message ( "kalsamix", i18n("kalsamix has been\nrestored to system tray!"), 
	        KGlobal::iconLoader()->loadIcon("kalsamix.png",KIcon::NoGroup, 32), mainWin->tray );
    } else
            mainWin->show();

    args->clear();
    return 0;
}

