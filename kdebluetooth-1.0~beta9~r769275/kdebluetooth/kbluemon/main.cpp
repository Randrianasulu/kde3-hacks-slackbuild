/*
 *
 *  KDE Blutooth Monitor 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluemon.
 *
 *  kbluemon is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluemon is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluemon; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kuniqueapplication.h>
#include <iostream>


#include "monitor.h"

static const char *description =
    I18N_NOOP("KBlueMon");

static KCmdLineOptions options[] =
{
    { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
    KLocale::setMainCatalogue("kdebluetooth");
    KAboutData aboutData("kbluemon",
                         I18N_NOOP("KBlueMon"),
			 0,
			 description, KAboutData::License_GPL,
                         "(c) 2007, Tom Patzig", 0, "http://kde-bluetooth.sf.net");
    aboutData.addAuthor("Tom Patzig", I18N_NOOP("Author"), "tpatzig@suse.de");
    aboutData.addAuthor("Daniel Gollub", I18N_NOOP(""), "dgollub@suse.de");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    kdDebug() << __func__ << "DEBUG" << endl;   

if (!KUniqueApplication::start()) {
        std::cerr << i18n("KBlueMon is already running.\n").local8Bit();
        return 0;
    }

    KUniqueApplication a(argc, argv); 

    Monitor mon;
    
    return mon.exec();



}
