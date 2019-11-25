/***************************************************************************
                          ServiceDiscovery.h  -  description
                             -------------------
    begin                : Sat Ott 18 2003
    copyright            : (C) 2003 by Simone Gotti
    email                : simone.gotti@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "servicediscovery.h"

#include <kglobal.h>
#include <kconfig.h>
#include "inquiry.h"
#include "namerequest.h"
#include "sdpdevice.h"
#include "sdpservice.h"
#include "rfcommsocket.h"
#include "deviceaddress.h"
#include "sdpservice.h"
#include <algorithm>
#include <kdebug.h>
#include <qtimer.h>

using namespace std;

namespace KBluetooth
{

using namespace SDP;

ServiceDiscovery::ServiceDiscovery(QObject *parent, QStringList requiredUUIDs,
    QString configGroup) :
    QObject(parent),
    configGroup(configGroup)
{
    for (size_t n=0; n<requiredUUIDs.count(); ++n) {
        this->requiredUUIDs.insert(KBluetooth::SDP::uuid_t(requiredUUIDs[n]));
    }
    normalInquiry = new Inquiry(NULL, this);

    readConfig();

    connect(normalInquiry, SIGNAL(neighbourFound(const KBluetooth::DeviceAddress&, int)),
            this, SLOT(slotInquiryDeviceFound(const KBluetooth::DeviceAddress&, int)));
    connect(normalInquiry, SIGNAL(finnished()),
            this, SLOT(slotInquiryFinnished()));

}

void ServiceDiscovery::inquiry()
{
    emit update();
    if (normalInquiry->inquiry() == false) {
        emit slotInquiryFinnished();
    }
}

ServiceDiscovery::~ServiceDiscovery()
{
    writeConfig();
}

void ServiceDiscovery::readConfig()
{
    KConfig* config = KGlobal::config();
    config->setGroup(configGroup);
    int nmax = config->readNumEntry("numServiceEntries", 0);
    clearServiceInfos();
    for (int n=0; n<nmax; ++n) {
        ServiceInfo *info = new ServiceInfo();
        info->m_address = DeviceAddress(config->readEntry(QString("%1-address").arg(n)));
        info->m_deviceName = config->readEntry(QString("%1-deviceName").arg(n));
        info->m_deviceClass = config->readNumEntry(QString("%1-deviceClass").arg(n));
        deviceClassMap[info->m_address] = info->m_deviceClass;
        info->m_serviceName = config->readEntry(QString("%1-serviceName").arg(n));
        info->m_rfcommChannel = config->readNumEntry(QString("%1-rfcommChannel").arg(n), 0);
        info->m_lastSeen = config->readDateTimeEntry(QString("%1-lastSeen").arg(n));
        info->m_lastUsed = config->readDateTimeEntry(QString("%1-lastUsed").arg(n));
        info->m_uuids = config->readListEntry(QString("%1-uuids").arg(n));
        serviceInfos.push_back(info);
    }
}

void ServiceDiscovery::clearServiceInfos()
{
    for (uint n=0; n<serviceInfos.size(); ++n) {
        delete serviceInfos[n];
    }
    serviceInfos.clear();
}

void ServiceDiscovery::writeConfig()
{
    const int maxEntries = 100;
    KConfig* config = KGlobal::config();
    config->deleteGroup(configGroup);
    config->setGroup(configGroup);
    //ServiceInfoVector::iterator it;
    int nmax = min(int(serviceInfos.size()), maxEntries);
    for (int n = 0; n < nmax; ++n) {
        ServiceInfo *info = serviceInfos[n];
        config->writeEntry(QString("%1-address").arg(n), QString(info->address()));
        config->writeEntry(QString("%1-deviceName").arg(n), info->deviceName());
        config->writeEntry(QString("%1-deviceClass").arg(n), info->deviceClass());
        config->writeEntry(QString("%1-serviceName").arg(n), info->serviceName());
        config->writeEntry(QString("%1-rfcommChannel").arg(n), info->rfcommChannel());
        config->writeEntry(QString("%1-lastSeen").arg(n), info->lastSeen());
        config->writeEntry(QString("%1-lastUsed").arg(n), info->lastUsed());
        config->writeEntry(QString("%1-uuids").arg(n), info->uuids());
    }
    config->writeEntry("numServiceEntries", nmax);
}


void ServiceDiscovery::slotInquiryDeviceFound(const DeviceAddress& addr, int devClass)
{
    bool bFound = false;
    for (uint n=0; n<serviceInfos.size(); ++n) {
        if (serviceInfos[n]->m_address == addr) {
            bFound = true;
            serviceInfos[n]->m_addressVerified = true;
            serviceInfos[n]->m_deviceClass = devClass;
            serviceInfos[n]->m_lastSeen = QDateTime::currentDateTime();
        }
    }
    inquiryAddresses.insert(addr);
    deviceClassMap[addr] = devClass;
    emit update();
}

ServiceDiscovery::ServiceInfoVector ServiceDiscovery::getServices()
{
    ServiceInfoVector serviceInfoVector;
    ServiceInfoVector::iterator it;
    for (it = serviceInfos.begin(); it != serviceInfos.end(); ++it) {
        //bool accept = true;
        /*for (uint n=0; n<requiredUUIDs.count(); ++n) {
            if (!(*it)->hasServiceClassID(requiredUUIDs[n])) {
                accept = false;
            }
        }*/
        //if (accept) {
        serviceInfoVector.push_back(*it);
        //}
    }
    return serviceInfoVector;
}

void ServiceDiscovery::slotInquiryFinnished()
{
    // I update the service information
    QTimer::singleShot(0, this, SLOT(updateSDPInfo()));
}

void ServiceDiscovery::updateSDPInfo()
{
    if (inquiryAddresses.size() > 0) {
        DeviceAddress updateAddress = *inquiryAddresses.begin();
        inquiryAddresses.erase(updateAddress);

        ServiceInfoVector::iterator it;
        ServiceInfoVector updatedServices;
        ServiceInfoVector removedServices;
        // Copy all services to the new service list, except for those
        // which have the current device address.
        for (it = serviceInfos.begin(); it != serviceInfos.end(); ++it) {
            if ((*it)->m_address != updateAddress) {
                updatedServices.push_back(*it);
            }
            else {
                removedServices.push_back(*it);
            }
        }

        // Re-add the services we just deleted by using up to date information
        SDP::Device sdpDev;
        sdpDev.setTarget(updateAddress, requiredUUIDs);

        SDP::ServiceVector services = sdpDev.services();
        SDP::ServiceVector::iterator servIt;
        for (servIt = services.begin(); servIt != services.end(); ++servIt) {
            NameRequest nameRequest;
            QString deviceName = nameRequest.resolve(updateAddress);
            QString serviceName = "";
            servIt->getServiceName(serviceName);
            uint rfcommChannel = 0;
            servIt->getRfcommChannel(rfcommChannel);
            QDateTime lastUsedTime;
            for (uint n=0; n<removedServices.size(); ++n) {
                ServiceInfo* r = removedServices[n];
                if (r->m_address == updateAddress && r->m_rfcommChannel == rfcommChannel) {
                    lastUsedTime = r->m_lastUsed;
                }
            }
            SDP::UUIDVec uuidvec = servIt->getClassIdList();
            QStringList uuids;
            for (uint n=0; n<uuidvec.size(); ++n) {
                uuids.append(QString(uuidvec[n]));
            }

            ServiceInfo* service = new ServiceInfo();
            service->m_address = updateAddress;
            service->m_addressVerified = true;
            service->m_channelVerified = true;
            service->m_deviceClass = deviceClassMap[updateAddress];
            service->m_deviceName = deviceName;
            service->m_lastSeen = QDateTime::currentDateTime();
            service->m_lastUsed = lastUsedTime;
            service->m_rfcommChannel = rfcommChannel;
            service->m_serviceName = serviceName;
            service->m_uuids = uuids;

            updatedServices.push_back(service);
            kdDebug() << "Service updated: " <<
                QString("addr=%1 aVer=%2 cVer=%3 devCl=%4 ch=%5 uuids=%6")
                .arg(service->m_address)
                .arg(service->m_addressVerified)
                .arg(service->m_channelVerified)
                .arg(service->m_deviceClass)
                .arg(service->m_rfcommChannel)
                .arg(uuids.join("|")) << endl;
        }

        // Replace the old service list with the updated one
        serviceInfos = updatedServices;

        emit update();
        writeConfig();
        QTimer::singleShot(100, this, SLOT(updateSDPInfo()));
    }
    else {
        emit finished();
    }
}

void ServiceDiscovery::clearCache()
{
    KConfig* config = KGlobal::config();
    config->deleteGroup(configGroup);
    readConfig();
}

ServiceDiscovery::ServiceInfo::ServiceInfo()
{
    m_address = DeviceAddress::invalid;
    m_deviceClass = 0;
    m_deviceName = QString::null;
    m_serviceName = QString::null;
    m_rfcommChannel = 0;
    m_lastSeen = QDateTime();
    m_lastUsed = QDateTime();
    m_addressVerified = false;
    m_channelVerified = false;
}

void ServiceDiscovery::ServiceInfo::use()
{
    m_lastUsed = QDateTime::currentDateTime();
    kdDebug() << "Service '" << m_serviceName << "' used (" <<
        m_lastUsed.toString() << ")" << endl;
}

DeviceAddress ServiceDiscovery::ServiceInfo::address() const
{
    return m_address;
}

int ServiceDiscovery::ServiceInfo::deviceClass() const
{
    return m_deviceClass;
}

QString ServiceDiscovery::ServiceInfo::deviceName() const
{
    return m_deviceName;
}

QString ServiceDiscovery::ServiceInfo::serviceName() const
{
    return m_serviceName;
}

uint ServiceDiscovery::ServiceInfo::rfcommChannel() const
{
    return m_rfcommChannel;
}

QDateTime ServiceDiscovery::ServiceInfo::lastSeen() const
{
    return m_lastSeen;
}

QDateTime ServiceDiscovery::ServiceInfo::lastUsed() const
{
    return m_lastUsed;
}

bool ServiceDiscovery::ServiceInfo::isAddressVerified() const
{
    return m_addressVerified;
}

bool ServiceDiscovery::ServiceInfo::isChannelVerified() const
{
    return m_channelVerified;
}

QStringList ServiceDiscovery::ServiceInfo::uuids() const
{
    return m_uuids;
}

bool ServiceDiscovery::ServiceInfo::hasServiceClassID(QString id) const
{
    for (uint n=0; n<m_uuids.count(); ++n) {
        if (SDP::uuid_t(id) == SDP::uuid_t(m_uuids[n])) {
            return true;
        }
    }
    return false;
}

} // namespace KBluetooth

#include "servicediscovery.moc"
