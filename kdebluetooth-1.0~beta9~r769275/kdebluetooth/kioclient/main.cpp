//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>
#include <qwidget.h>
#include "commandhandler.h"
#include "../libkbluetooth/configinfo.h"

static const char description[] =
    I18N_NOOP("KIO command line client");

static const char* version = KDEBluetoothConfig::version;

static KCmdLineOptions options[] =
{
//        {"rfcommconnection <sockfd>", I18N_NOOP("Used by kbluetoothd"), 0},
//        {"peeraddr <addr>", I18N_NOOP("Used by kbluetoothd"), 0},
//        {"peername <name>", I18N_NOOP("Used by kbluetoothd"), 0},
    {"d", 0, 0}, {"date", I18N_NOOP("List date"), 0},
    {"u", 0, 0}, {"user", I18N_NOOP("List user"), 0},
    {"i", 0, 0}, {"iconName", I18N_NOOP("List icon name"), 0},
    {"g", 0, 0}, {"group", I18N_NOOP("List group"), 0},
    {"extra", I18N_NOOP("List extra"), 0},
    {"noname", I18N_NOOP("Do not list name"), 0},
    {"p", 0, 0}, {"access", I18N_NOOP("List access permissions"), 0},
    {"m", 0, 0}, {"mtime", I18N_NOOP("List modification time"), 0},
    {"a", 0, 0}, {"atime", I18N_NOOP("List access time"), 0},
    {"c", 0, 0}, {"ctime", I18N_NOOP("List creation time"), 0},
    {"f", 0, 0}, {"filetype", I18N_NOOP("List file type"), 0},
    {"D", 0, 0}, {"linkdest", I18N_NOOP("List link destination"), 0},
    {"U", 0, 0}, {"url", I18N_NOOP("List URL"), 0},
    {"M", 0, 0}, {"mimetype", I18N_NOOP("List mime type"), 0},
    {"G", 0, 0}, {"guessedmimetype", I18N_NOOP("List guessed mime type"), 0},
    {"X", 0, 0}, {"xmlproperties", I18N_NOOP("List XML properties"), 0},
    {"s", 0, 0}, {"size", I18N_NOOP("List size"), 0},
    {"outfile [filename]", I18N_NOOP("Output file. Defaults to stdout"), 0},
    {"infile [filename]", I18N_NOOP("Input file. Defaults to stdin"), 0},
    {"progresswindow", I18N_NOOP("Show a progress window"), 0},
    {"nooverwrite", I18N_NOOP("Ask (graphically) before overwriting files"), 0}, 
    {"messages", I18N_NOOP("Show messages from the kioslave"), 0},
    {"+[cmd]", I18N_NOOP("Command (ls, cat, put, cp, rm, mv, mkdir, rmdir)"), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kioclient",
        I18N_NOOP("kio client"),
        version, description,
        KAboutData::License_GPL,
        "(C) 2004 Fred Schaettgen", 0, 0,
        "kdebluetooth@schaettgen.de");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    //args->isSet("peername")) {
    //QString name = QString(args->getOption("peername"));
    //KURL url = args->arg(0);
    CommandHandler commandHandler(args);
    commandHandler.start();
    return app.exec();
}

