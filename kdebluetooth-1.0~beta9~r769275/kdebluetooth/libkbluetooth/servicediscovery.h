/***************************************************************************
                          ultrainquiry.h  -  description
                             -------------------
    begin                : Sat Ott 18 2003
    copyright            : (C) 2003 by Simone Gotti, Fred Schaettgen
    email                : simone.gotti@email.it, kdebluetooth@schaettgen.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _SERVICEDISCOVERY_H_
#define _SERVICEDISCOVERY_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <vector>
#include <map>
#include <qguardedptr.h>
#include "deviceaddress.h"
#include "sdpdevice.h"


namespace KBluetooth
{

class DeviceAddress;
class Inquiry;
class RfcommSocket;

namespace SDP
{
    class Device;
    class uuid_t;
}

/**
@author Simone Gotti, Fred Schaettgen
*/
class ServiceDiscovery : public QObject
{
    Q_OBJECT
public:
    class ServiceInfo
    {
    public:
        void use();
        DeviceAddress address() const;
        QString deviceName() const;
        int deviceClass() const;
        QString serviceName() const;
        uint rfcommChannel() const;
        QDateTime lastSeen() const;
        QDateTime lastUsed() const;
        bool isAddressVerified() const;
        bool isChannelVerified() const;
        QStringList uuids() const;
        bool hasServiceClassID(QString id) const;
    public:
        ServiceInfo();
        DeviceAddress m_address;
        QString m_deviceName;
        int m_deviceClass;
        QString m_serviceName;
        uint m_rfcommChannel;
        QDateTime m_lastSeen;
        QDateTime m_lastUsed;
        bool m_addressVerified;
        bool m_channelVerified;
        QStringList m_uuids;
    };

    ServiceDiscovery(QObject *parent, QStringList requiredUUIDs,
        QString configGroupName = "ServiceDiscoveryCache");
    ~ServiceDiscovery();

	void inquiry();
    typedef std::vector<ServiceInfo*> ServiceInfoVector;
    ServiceInfoVector getServices();
    void clearCache();

signals:
    void update();
    void finished();
private:
    QGuardedPtr<KBluetooth::Inquiry> normalInquiry;

    std::set<DeviceAddress> inquiryAddresses;
    std::map<DeviceAddress, int> deviceClassMap;
    std::set<KBluetooth::SDP::uuid_t> requiredUUIDs;
    ServiceInfoVector serviceInfos;
    QString configGroup;

    void clearServiceInfos();
	void readConfig();
    void writeConfig();

private slots:
    void updateSDPInfo();
    void slotInquiryDeviceFound(const KBluetooth::DeviceAddress& addr, int devclass);
    void slotInquiryFinnished();
};

}

#endif // _ULTRAINQUIRY_H_
