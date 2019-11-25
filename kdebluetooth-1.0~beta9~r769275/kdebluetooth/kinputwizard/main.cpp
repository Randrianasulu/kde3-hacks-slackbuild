/*
 *
 *  KDE InputDevice Wizard 
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of kinputwizard.
 *
 *  kinputwizard is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  kinputwizard is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with kinputwizard; if not, write to the Free Software
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

#include "inputwizard.h"

static const char *description = I18N_NOOP("KInputWizard");
static const char *copy    = I18N_NOOP("Copyright (C) 2007 Novell, Inc.");

static KCmdLineOptions options[] =
{
    { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
    KLocale::setMainCatalogue("kdebluetooth");
    KAboutData aboutData("kinputwizard",
                         I18N_NOOP("KInputWizard"),
			 0,
			 description, KAboutData::License_GPL,
                         copy,0, "http://opensuse.org");
    aboutData.addAuthor("Tom Patzig", I18N_NOOP("Author"), "tpatzig@suse.de");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    if (!KUniqueApplication::start()) {
        std::cerr << i18n("KInputWizard is already running.\n").local8Bit();
        return 0;
    }

    KUniqueApplication a(argc, argv); 

    InputWizard input;
    
    return input.exec();

}
