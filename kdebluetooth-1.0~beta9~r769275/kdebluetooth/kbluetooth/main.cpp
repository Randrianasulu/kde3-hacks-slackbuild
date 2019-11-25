//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>
#include <iostream>

#include "application.h"
static const char *description =
    I18N_NOOP("KBluetooth");

static KCmdLineOptions options[] =
{
    { "dontforceshow", I18N_NOOP("Show the tray icon if it was disabled before." ), 0},
    { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
    KLocale::setMainCatalogue("kdebluetooth");
    KAboutData aboutData("kdebluetooth",
                         I18N_NOOP("KBluetooth"),
			 0,
			 description, KAboutData::License_GPL,
                         "(c) 2003-2004, Fred Schaettgen", 0, "http://kde-bluetooth.sf.net");
    aboutData.addAuthor("Fred Schaettgen", I18N_NOOP("Author"), "kbluetoothd@schaettgen.de");
    aboutData.addAuthor("Tom Patzig", I18N_NOOP(""), "tpatzig@suse.de");
    aboutData.addAuthor("Daniel Gollub", I18N_NOOP(""), "dgollub@suse.de");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start()) {
        std::cerr << i18n("KBluetooth is already running.\n").local8Bit();
        return 0;
    }

    KBluetoothApp a;

/*
    if (a.noAdapter == true) {
		KMessageBox::error(NULL,i18n("No Bluetooth Adapter found!\nUnable to start kbluetooth.\n\n \
									Plug in a Bluetooth Adapter or restart the bluetooth service."));
		KUniqueApplication::kApplication()->quit();
		return 0; 
*/
     if (a.noDBus == true) {
		KMessageBox::error(NULL,i18n("Can't connect to DBus!\nUnable to start kbluetooth. \n\n \
									Restart dbus and the bluetooth service"));
        KUniqueApplication::kApplication()->quit();
		return 0;	
	} else    
    	return a.exec();
    
}

