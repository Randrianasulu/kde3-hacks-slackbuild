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

#include "btsdp.h"

#include <sys/stat.h>
#include <qregexp.h>
#include <algorithm>
#include <kdebug.h>
#include <kinstance.h>
#include <klocale.h>
#include <kservice.h>
#include <kservicetype.h>

#include <qdom.h>

using namespace KIO;
using namespace KBluetooth;
using namespace std;

SdpProtocol::SdpProtocol(const QCString &pool_socket, const QCString &app_socket) :
        SlaveBase("kio_sdp", pool_socket, app_socket)
{
    KLocale::setMainCatalogue("kdebluetooth");
    kdDebug() << "SdpProtocol::SdpProtocol()" << endl;
    //initHandlerList();


    DBusInit *dbus = new DBusInit();
    conn = dbus->getDBus();
    manager = new Manager(conn);
    adapter = new Adapter(manager->defaultAdapter(), conn);

    if (manager->listAdapters().count() == 0) {
        warning(i18n("No working Bluetooth adapter found."));
    }
}

SdpProtocol::~SdpProtocol()
{
    kdDebug() << "SdpProtocol::~SdpProtocol()" << endl;
}


void SdpProtocol::initHandlerList()
{
    KService::List handlers =
        KServiceType::offers("SdpServiceHandler");
    kdDebug() << "Looking for ServiceHandlers" << endl;
    for(KService::List::ConstIterator it = handlers.begin();
            it != handlers.end(); it++)
    {
        KService::Ptr s = *it;
        QStringList sAttributes;
        QVariant vProtocol = s->property(
                                 "X-SDPSERVICEHANDLER-requiredUIDs");
        QVariant vMimetype = s->property("X-SDPSERVICEHANDLER-mimeType");
        kdDebug() << "Allowed attributes:"
        << s->propertyNames().join(",") << endl;
        if (vProtocol.isValid() && vMimetype.isValid())
        {
            kdDebug() << "ServiceUIDs:" <<
            vProtocol.toStringList().join(" & ") <<
            " mimetype:" << vMimetype.toString() << endl;

            // Parse the UUID list of the service Handler
            QStringList uuidStrList = vProtocol.toStringList();
            for (uint n=0; n<uuidStrList.size(); ++n)
            {
                QString uuid;
                kdDebug() << "sdp: adding uuid " << uuidStrList[n] << endl;
		/*
                if (uuid.fromString(uuidStrList[n]) == true)
                {
                    kdDebug() << "->" << QString(uuid) << endl;
                    ;
                    handlerInfo.uuids.push_back(uuid);
                }
                else
                {
                    kdWarning() << QString("Bad uuid (%1) for service %2").
                    arg(uuidStrList[n]).arg(s->name()) << endl;
                }
		*/
            }

            // Append the handler info to the handlerList
        }
        else
        {
            kdDebug() << "Service has wrong UID List" << endl;
        }
    }
    kdDebug() << "done." << endl;
}


void SdpProtocol::setHost(const QString & host, int /*port*/,
                              const QString &/*user*/, const QString &/*pass*/)
{
    kdDebug() << "kio_sdp::setHost(" << host << ")" << endl ;
    mAddress = host;
}

/*void SdpProtocol::mimetype(const KURL &url)
{
    kdDebug(7101) << "kio_sdp::mimetype(" << url.url() << ")" << endl ;
    //mimeType("text/plain");
    finished();
}*/

void SdpProtocol::stat(const KURL &url)
{
    kdDebug() << "kio_sdp::stat(" << url.url() << ")" << endl ;
    UDSEntry entry;
    QString path = url.path(+1);
    kdDebug() << "Has host: " << url.hasHost() << endl;
    if (url.hasHost() == true)
    {
        kdDebug() << "path: " << path << endl;
        if (path == "/")
        {
            createDirEntry(entry, i18n("Bluetooth neighborhood"));
            statEntry(entry);
            finished();
        }
        else
        {
            QRegExp reg = QRegExp("^/uuid-(0x[a-f,A-F,0-9:]+)/");
            if (reg.search(path) >= 0) {
                createDirEntry(entry, i18n("More services"));
                statEntry(entry);
                finished();
            }
            else {
              error(KIO::ERR_SLAVE_DEFINED,
                    i18n("Could not stat %1. Unknown device").arg(url.url()));
            }

        }
    }
    else {
        redirection(KURL("bluetooth:/"));
        finished();
    }
}


void SdpProtocol::listDir(const KURL &url)
{
    QString host = url.host();
    QString path = url.path(+1);

    kdDebug() << "kio_sdp::listdir(" << host << ") (" << path <<  ")" << endl ;

    if (host == QString::null)
    {
        redirection(KURL("bluetooth:/"));
        finished();
    }
    else
    {
	kdDebug() << "PATH: " << path << endl;
        if (path == "/")
        {
            doListServices(url, host, "0x1002");
        }
        else
        {
	    kdDebug() << "Matching QRepExp ..." << endl;
            QRegExp reg = QRegExp("^/uuid-(0x[a-f,A-F,0-9:]+)/");
            if (reg.search(path) >= 0) {
                doListServices(url, host, reg.cap(0));
            }
            else {
                doListInvalid(url);
            }
        }
    }
}

void SdpProtocol::get(const KURL &/*url*/)
{
    kdDebug() << "kio_bluetooth: get() was called! This is nonsense." << endl ;
    error(KIO::ERR_IS_DIRECTORY, QString::null);
}

bool SdpProtocol::doListServices(const KURL& url, QString hostname, const QString& browseGroup)
{
    kdDebug() << k_funcinfo << endl;

    UDSEntry entry;
    infoMessage(i18n("Retrieving services for %1.").arg(hostname));

    infoMessage(QString::null);

    Adapter::ServiceHandleList serviceList = adapter->getRemoteServiceHandles(mAddress,"");
    Adapter::ServiceHandleList::iterator it;
	
    for (it = serviceList.begin(); it != serviceList.end(); ++it) {
  	ServiceRecord record = adapter->getRemoteServiceRecord(mAddress, *it);

        KURL serviceURL;
        serviceURL.setProtocol("sdp");
        serviceURL.setHost(mAddress);
        //serviceURL.setPath("/params");
	// TODO ... is name needed?
        //serviceURL.addQueryItem("name", "REALNAME");

        // Obex ftp is handled differently from the the other services
        // since in we don't want to start a program in that case,
        // but jump to an obex-url instead.
        // Should any other bluetooth-related kioslave appear eventually,
        // then we'll have to think about a solution.
        // But it seems unlikely at the moment, so we go the simple way here.
	// TODO: if obex : UUID: 0x1106

        if (record.uuid().contains("0x1106"))
        {
            // OBEX ftp service:
            KURL obexFtpURL;
            obexFtpURL.setProtocol("obex");
            obexFtpURL.setUser(QString("[%1]").arg(mAddress));
	 	 	 kdDebug() << "address: " << QString("[%1]").arg(mAddress) << endl;
	 	 	 kdDebug() << "URLaddress*************: " << obexFtpURL.url() << endl;
//	    obexFtpURL.setHost("bluetooth");
            if (record.rfcommChannel() > 0) {
                obexFtpURL.setPort(record.rfcommChannel());
            }
            obexFtpURL.setPath("/");
            createDirEntry(entry, record.name(), QString("obex://[%1]/").arg(mAddress), "bluetooth/obex-ftp-profile");
            listEntry(entry, false);
        }
	// TODO if obex push: UUID: 0x1105
        else if (record.uuid().contains("0x1105"))
        {
            // OBEX Object Push service:
            // This entry is special in that it works differently depending
            // if you click the obex push icon in konqueror or in the file
            // save dialog. In konqueror, kbtobexclient is started.
            // In the file save dialog, kio_obex will be used.
            KURL obexPushURL;
            obexPushURL.setProtocol("obex");
            obexPushURL.setHost(mAddress);
            if (record.rfcommChannel() > 0) {
                obexPushURL.setPort(record.rfcommChannel());
            }
            obexPushURL.setPath("/");
            addAtom(entry, UDS_NAME, record.name());
            addAtom(entry, UDS_URL, obexPushURL.url());
            addAtom(entry, UDS_MIME_TYPE, "bluetooth/obex-object-push-profile");
            addAtom(entry, UDS_FILE_TYPE, S_IFDIR);
            //addAtom(entry, UDS_GUESSED_MIME_TYPE, "inode/directory");
            listEntry(entry, false);
	}
	/*
        } else {
                createFileEntry(entry, record.name(),
                                "bluetooth/unknown-profile",
                                QString::null);
                listEntry(entry, false);
        }
	*/
    }

    listEntry(entry, true); // ready
    finished();

    return true;
}
 

// Find the mimetypes of service handlers which can
// handle a service using the given uuids
#if 0
void SdpProtocol::findMimeTypesForUUIDList(
    vector<QString> &mimeTypes, const UUIDVec& uuids)
{
    mimeTypes.clear();
    HandlerInfoVec::iterator infoIt = handlerList.begin();
    // Tray each handler in handlerList
    kdDebug() << "sdp::findMimeTypesForUUIDList (" <<
    uuids.size() << " Service-UUIDs)" << endl ;
    for (; infoIt != handlerList.end(); ++infoIt)
    {
        // Check if each uuid needed by the handler
        // is defined in the uuids of the service
        kdDebug() << "sdp: trying "<< infoIt->mimeType <<
        " (" << infoIt->uuids.size() << " UUIDs)" << endl ;
        bool allUUIDsFound = true;
        UUIDVec::iterator neededIt = infoIt->uuids.begin();
        for (; neededIt != infoIt->uuids.end(); ++neededIt)
        {
            SDP::uuid_t neededUUID = *neededIt;
            bool uuidFound = false;
            UUIDVec::const_iterator uIt = uuids.begin();
            while (uIt != uuids.end() && uuidFound == false)
            {
                kdDebug() << QString("sdp: %1==%2 ?").arg(QString(neededUUID))
                .arg(QString(*uIt)) << endl;
                if (*uIt == neededUUID)
                {
                    uuidFound = true;
                }
                ++uIt;
            }
            if (uuidFound == false)
            {
                allUUIDsFound = false;
            }
        }

        if (allUUIDsFound == true)
        {
            kdDebug() << "sdp: UUID match!"<< endl ;
            mimeTypes.push_back(infoIt->mimeType);
        }
    }
}
#endif

bool SdpProtocol::doListInvalid(const KURL &url)
{
    error(KIO::ERR_MALFORMED_URL, url.url());
    return true;
}

bool SdpProtocol::createDirEntry(UDSEntry &entry, const QString &title,
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
    addAtom(entry, UDS_GUESSED_MIME_TYPE, "inode/directory");

    return true;
}

bool SdpProtocol::createFileEntry(KIO::UDSEntry &entry,
                                      QString title, QString mimetype, QString url)
{
    entry.clear();

    addAtom(entry, UDS_NAME, title);
    if (mimetype != QString::null)
    {
        addAtom(entry, UDS_MIME_TYPE, mimetype);
    }
    if (url != QString::null)
    {
        addAtom(entry, UDS_URL, url);
    }

    addAtom(entry, UDS_FILE_TYPE, S_IFREG);

    return true;
}

void SdpProtocol::addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, QString s)
{
    UDSAtom atom;
    atom.m_uds = type;
    atom.m_str = s;
    entry.append(atom);
}

void SdpProtocol::addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, long l)
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
        KInstance instance( "kio_sdp" );
        kdDebug() << "*** Starting kio_sdp " << endl;
        if (argc != 4)
        {
            kdDebug() << "Usage: kio_sdp  protocol domain-socket1 domain-socket2" << endl;
            exit(-1);
        }
        SdpProtocol slave(argv[2], argv[3]);
        slave.dispatchLoop();
        kdDebug(7101) << "*** kio_sdp Done" << endl;
        return 0;
    }
}
