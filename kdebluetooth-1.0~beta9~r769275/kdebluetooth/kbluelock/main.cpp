/*
 *
 *  KDE Bluetooth Screen Locker 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kbluelock.
 *
 *  kbluelock is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kbluelock is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kbluelock; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
//#include <qapplication.h>
#include <kuniqueapplication.h>
#include <qimage.h>
#include <iostream>

#include "kbluelock.h"

static const char *description = I18N_NOOP("KBlueLock");
static const char *copy    = I18N_NOOP("Copyright (C) 2007 Novell, Inc.");

static KCmdLineOptions options[] =
{
    { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
    KLocale::setMainCatalogue("kdebluetooth");
    KAboutData aboutData("kbluelock",
                         I18N_NOOP("KBlueLock"),
			 0,
			 description, KAboutData::License_GPL,
                         copy,0, "http://opensuse.org");
    aboutData.addAuthor("Tom Patzig", I18N_NOOP("Author"), "tpatzig@suse.de");
    aboutData.setAppName("kbluelock");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    if (!KUniqueApplication::start()) {
        std::cerr << i18n("KBlueLock is already running.\n").local8Bit();
        return 0;
    }

    KUniqueApplication a(argc, argv); 


    KBlueLock l;
    a.setTopWidget(&l);

    return a.exec();


}
