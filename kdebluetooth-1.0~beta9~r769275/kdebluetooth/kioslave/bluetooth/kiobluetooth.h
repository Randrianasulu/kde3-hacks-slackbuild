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

#include <qstring.h>
#include <kio/slavebase.h>
#include <vector>

#include <libkbluetooth/dbusinit.h>
#include <libkbluetooth/adapter.h>
#include <libkbluetooth/manager.h>

class QCString;

typedef QMap<QString,QString> QStrMap;

class KioBluetooth : public QObject, public KIO::SlaveBase
{
    Q_OBJECT

    struct InquiryInfo
    {
        QString address;
        int deviceClass;
    };
    struct DevInfo
    {
        QString realName;
        QString uniqueName;
        QString mimeType;
        QString address;
    };
    typedef std::vector<DevInfo> DevInfoVec;
    DevInfoVec deviceList;

public:
    KioBluetooth(const QCString &pool_socket, const QCString &app_socket);
    virtual ~KioBluetooth();
    virtual void stat(const KURL &url);
    virtual void listDir(const KURL &url);
    void get(const KURL &url);



private:
    bool wasPeriodicDiscovery;

    std::vector<QString> getCurrentConnections();
    std::vector<QString> getCurrentNonDiscoverableDevices();
    bool doListBrowse(const KURL &url);
    bool doListInvalid(const KURL &url);
    void listDevice(const QString& devAddress);
    bool createDirEntry(KIO::UDSEntry &entry, const QString &title,
       const QString &url = QString::null, const QString &mimeType = "inode/directory");
    void addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, QString s);
    void addAtom(KIO::UDSEntry &entry, KIO::UDSAtomTypes type, long s);
    DBusConnection *conn;

    KBluetooth::Adapter *a;
    KBluetooth::Manager *m;
    QMap<QString,int> qDevicesList;

private slots:    
    void slotAddDevice(const QString &address, int devclass, short rssi);
    void slotEndDiscover();


};

#endif
