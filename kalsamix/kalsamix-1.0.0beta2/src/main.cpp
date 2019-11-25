/*
 * Copyright (C) 2003 Stefano Rivoir <s.rivoir@gts.it>
 */

#include "kalsamix.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "kamuapp.h"

static const char description[] =
    I18N_NOOP("An ALSA mixer application for KDE");

static const char version[] = "1.0.0beta2";

static KCmdLineOptions options[] =
{
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    
    KAboutData about("kalsamix", I18N_NOOP("kalsamix"), version, description,
                     KAboutData::License_GPL, "(C) 2003-2007 Stefano Rivoir", 0, 0, "s.rivoir@gts.it");
    about.addAuthor( "Stefano Rivoir", 0, "s.rivoir@gts.it" );
    
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();
    
    if ( !KAMUApp::start() )
	exit ( 0 );
    
    KAMUApp app;
    return app.exec();
}
