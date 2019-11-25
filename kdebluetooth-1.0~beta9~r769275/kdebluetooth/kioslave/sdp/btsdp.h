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

#ifndef _btsdp_H_
#define _btsdp_H_

#include <kio/slavebase.h>
#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/manager.h>
#include <libkbluetooth/adapter.h>

class SdpProtocol : public KIO::SlaveBase
{

public:
    SdpProtocol(const QCString &pool_socket, const QCString &app_socket);
    virtual ~SdpProtocol();
    virtual void stat(const KURL &url);
    virtual void listDir(const KURL &url);
    virtual void setHost(const QString & host, int port,
                         const QString &user, const QString &pass);
    virtual void get(const KURL &url);

private:
    //bool inquiry(std::vector<InquiryInfo> &addrList);
    bool doListInvalid(const KURL &url);
    bool doListServices(const KURL &url, QString btaddr, const QString& browseGroup);
    void initHandlerList();

    bool createFileEntry(KIO::UDSEntry &entry, QString title,
                         QString mimetype = QString::null,
                         QString url=QString::null);
    bool createDirEntry(KIO::UDSEntry &entry,
                        const QString &title,
                        const QString &url = QString::null,
                        const QString &mimeType = "inode/directory");
    void addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type,
                 QString s);
    void addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type,
                 long s);

    DBusConnection *conn;
    KBluetooth::Manager *manager;
    KBluetooth::Adapter *adapter;

    QString mAddress;
};

#endif
