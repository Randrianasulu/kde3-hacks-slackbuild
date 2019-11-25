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

#include "kiobluetooth.h"

#include <sys/stat.h>
#include <qregexp.h>
#include <unistd.h>
#include <algorithm>

#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>
#include <dcopclient.h>
#include <qapplication.h>

#include <kapplication.h>
#include <kcmdlineargs.h>

#include <qapplication.h>
#include <qeventloop.h>

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/devicemimeconverter.h>

using namespace KIO;
using namespace KBluetooth;

static const KCmdLineOptions options[] =
{
        { "+protocol", I18N_NOOP( "Protocol name" ), 0 },
        { "+pool", I18N_NOOP( "Socket name" ), 0 },
        { "+app", I18N_NOOP( "Socket name" ), 0 },
        KCmdLineLastOption
};


KioBluetooth::KioBluetooth(const QCString &pool_socket, const QCString &app_socket) :
        SlaveBase("kio_bluetooth", pool_socket, app_socket)
{
    KLocale::setMainCatalogue("kdebluetooth");
    kdDebug() << "KioBluetooth::KioBluetooth()" << endl;

    wasPeriodicDiscovery = false;
    DevInfo localInfo;
    localInfo.realName = localInfo.uniqueName = "localhost";
    localInfo.address = QString("FF:FF:FF:00:00:00");
    deviceList.push_back(localInfo);

    DBusInit *dbus = new DBusInit;
    DBusConnection *conn = dbus->getDBus();

    m = new Manager(conn);
    a = new Adapter(m->defaultAdapter(), conn);

    connect( a, SIGNAL( remoteDeviceFound(const QString &, int, short) ),
		    this, SLOT( slotAddDevice(const QString &, int, short) ) );

    connect( a, SIGNAL( discoveryCompleted() ),
		    this, SLOT( slotEndDiscover() ) );
}

KioBluetooth::~KioBluetooth()
{
    kdDebug() << "KioBluetooth::~KioBluetooth()" << endl;
    delete m;
    delete a;
}

void KioBluetooth::stat(const KURL &url)
{
    kdDebug() << "kio_bluetooth::stat(" << url.url() << ")" << endl ;
    UDSEntry entry;
    QString path = url.path(+1);
    if (url.hasHost() == false && path == "/")
    {
        createDirEntry(entry, i18n("Bluetooth neighborhood"));
        statEntry(entry);
        finished();
    }
    else {
        error(KIO::ERR_SLAVE_DEFINED,
            i18n("Could not stat %1.").arg(url.url()));
    }

}

void KioBluetooth::get(const KURL &/*url*/)
{
    kdDebug() << "kio_bluetooth: get() was called! This is nonsense." << endl ;
    error(KIO::ERR_IS_DIRECTORY, QString::null);
}

void KioBluetooth::listDir(const KURL &url)
{
    QString host = url.host();
    QString path = url.path(+1);

    kdDebug() << "kio_bluetooth::listdir(" << host << ") (" << path <<  ")" << endl ;

    if (host == QString::null && path == "/") {
        doListBrowse(url);
    }
    else {
        doListInvalid(url);
    }
}

void KioBluetooth::listDevice( const QString& devAddress )
{
    UDSEntry entry;
    UDSEntryList entryList;

    QString name = a->getRemoteName(devAddress);
    QString alias = a->getRemoteAlias(devAddress);
    int devClass = a->getRemoteClass(devAddress);
            
    entryList.clear();
    entry.clear();

    if (!alias.isEmpty())
       name = alias;
    else if (!name.isEmpty())
       name = name;
    else
       name = devAddress; 

    createDirEntry(entry, name, QString("sdp://[%1]/").arg(devAddress),
        DeviceClassMimeConverter::classToMimeType(devClass));

    entryList.append(entry);
    listEntries(entryList);
}

bool KioBluetooth::doListBrowse(const KURL&)
{
    kdDebug() << "kio_bluetooth::doListBrowse()" << endl;
    if (a->isPeriodicDiscovery()) {
	    wasPeriodicDiscovery = true;
	    a->stopPeriodicDiscovery();
    }

    a->discoverDevices();

    qApp->eventLoop()->processEvents( QEventLoop::AllEvents );
    qApp->eventLoop()->enterLoop();

    listEntry(UDSEntry(), true); // ready

    finished();
    return true;
}

void KioBluetooth::slotEndDiscover()
{
	if (wasPeriodicDiscovery) {
		a->startPeriodicDiscovery();
		wasPeriodicDiscovery = false;
	}

	qApp->eventLoop()->exitLoop();
}

bool KioBluetooth::doListInvalid(const KURL &url)
{
    error(KIO::ERR_MALFORMED_URL, url.url());
    return true;
}


bool KioBluetooth::createDirEntry(UDSEntry &entry, const QString &title,
    const QString &url, const QString &mimeType)
{
    entry.clear();

    addAtom(entry, UDS_NAME, title);

    if (url != QString::null)
    {
        addAtom(entry, UDS_URL, url);
    }

    addAtom(entry, UDS_MIME_TYPE, mimeType);
    addAtom(entry, UDS_FILE_TYPE, S_IFDIR);
    // No more necessary
    // addAtom(entry, UDS_GUESSED_MIME_TYPE, "inode/folder");

    return true;
}

void KioBluetooth::slotAddDevice(const QString &address, int devclass, short rssi)
{
	kdDebug() << __func__ << "(): " << address << endl;
	QMap<QString,int>::iterator f=qDevicesList.find(address);
	if(f!=qDevicesList.end() && f.data() == devclass) return;
	qDevicesList.insert(address, devclass);

	listDevice(address);
	listEntry(UDSEntry(), true);

}

void KioBluetooth::addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, QString s)
{
    UDSAtom atom;
    atom.m_uds = type;
    atom.m_str = s;
    entry.append(atom);
}


void KioBluetooth::addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, long l)
{
    UDSAtom atom;
    atom.m_uds = type;
    atom.m_long = l;
    entry.append(atom);
}

extern "C"
{
    int kdemain(int argc, char **argv)
    {
        KInstance instance( "kio_bluetooth" );
        kdDebug() << "*** Starting kio_bluetooth " << endl;
        if (argc != 4)
        {
            kdDebug() << "Usage: kio_bluetooth  protocol domain-socket1 domain-socket2" << endl;
            exit(-1);
        }

	putenv(strdup("SESSION_MANAGER="));
	KCmdLineArgs::init(argc, argv, "kio_bluetooth", 0, 0, 0, 0);
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app( false, false );
	app.dcopClient()->attach();

//	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        KioBluetooth slave(argv[2], argv[3]);
        slave.dispatchLoop();
        kdDebug() << "*** kio_bluetooth Done" << endl;
        return 0;
    }
}

#include "kiobluetooth.moc"
