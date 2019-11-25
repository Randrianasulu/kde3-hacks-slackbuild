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

#include "inquiry.h"
#include <qcstring.h>
#include <qsocket.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qthread.h>
#include <qapplication.h>
#include <unistd.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "deviceaddress.h"
#include "hcisocket.h"

#include <unistd.h>
#include <kdebug.h>
#include <klocale.h>
#include <assert.h>

using namespace std;

namespace KBluetooth
{

Inquiry::Inquiry(HciSocket* socket, QObject *owner, QString) :
    QObject(owner)
{
    reset();
    inquiryTimeoutTimer = new QTimer(this);
    connect(inquiryTimeoutTimer, SIGNAL(timeout()),
        this, SLOT(slotInquiryTimeout()));
    if (socket == NULL) {
        this->socket = new HciSocket(this, "");
        this->socket->open();
    }
    else {
        this->socket = socket;
    }
    connect(this->socket, SIGNAL(event(unsigned char, QByteArray)),
        this, SLOT(slotHciEvent(unsigned char, QByteArray)));
}

Inquiry::~Inquiry()
{
}

bool Inquiry::inquiry(int numResponses, double timeout, int lap)
{
    QByteArray cmdBuf(5);
    cmdBuf[0] = lap & 0xFF;
    cmdBuf[1] = (lap >> 8) & 0xFF;
    cmdBuf[2] = (lap >> 16) & 0xFF;
    cmdBuf[3] = max(0x01, min(0x30, int(timeout/1.28)));
    cmdBuf[4] = (unsigned char)numResponses;
    kdDebug() << "Send HCI inquiry command.." << endl;
    socket->sendCommand(0x01, 0x0001, cmdBuf);
    inquiryTimeoutTimer->start(int(1000*(timeout+1.0)), true);
    
    int status;
    if (socket->readStatus(0x01, 0x0001, &status)) {
        if (status == 0) {
            successfullyStarted = true;
            kdDebug() << "Inquiry started successfully" << endl;
            return true;
        }
        else {
            kdDebug() << QString("Inquiry::inquiry() failed: 0x%1").arg(status, 2, 16) << endl;
            return false;
        }
    }
    else {
        kdDebug() << "Inquiry::inquiry(): Timeout." << endl;
        return false;
    }
}

bool Inquiry::inquiryInProgress()
{
    return inquiryTimeoutTimer->isActive();
}

bool Inquiry::nextNeighbour(KBluetooth::DeviceAddress& bdaddr,
    int& deviceClass)
{
    while ((infoQueue.size() == 0) && (inquiryTimeoutTimer->isActive()) &&
        socket->socketDevice().isValid())
    {
        if (qApp)
        {
            qApp->processEvents(250);
        }
        else
        {
            bool timeout = false;
            if (socket->socketDevice().bytesAvailable() == 0) {
                socket->socketDevice().waitForMore(250, &timeout);
            }
            if (!timeout) {
                socket->readEvent();
            }
        }
    }
    if (infoQueue.size() > 0)
    {
        InquiryInfo info = infoQueue.front();
        bdaddr = info.addr;
        deviceClass = info.deviceClass;
        infoQueue.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

bool Inquiry::isComplete()
{
    return successfullyStarted && successfullyEnded;
}
      
void Inquiry::reset()
{
    successfullyStarted = false;
    successfullyEnded = false;
    addrCache.clear();
    infoQueue.clear();
}


void Inquiry::onNeighbourFound(const KBluetooth::DeviceAddress& bdaddr,
    int deviceClass)
{
    emit neighbourFound(bdaddr, deviceClass);
}

void Inquiry::slotInquiryTimeout()
{
    emit error(InquiryFinnishedTimeout,
        i18n("Timeout while waiting for end of inquiry."));
}

void Inquiry::slotHciEvent(unsigned char eventCode, QByteArray buf)
{
    kdDebug() << "Inquiry: hci packet received: eventCode="
        << (unsigned int)eventCode << " packetLength="
        << (unsigned int)buf.size() << endl;

    unsigned char *data = (unsigned char*)buf.data();
    switch (eventCode)
    {
    case EVT_INQUIRY_COMPLETE:
        {
            unsigned char status = data[0];
            kdDebug() << "EVT_INQUIRY_COMPLETE status=" << status << endl;
            inquiryTimeoutTimer->stop();
            if (status == 0) {
                if (successfullyStarted == true) {
                    kdDebug() << "Inquiry ended successfully" << endl;
                    successfullyEnded = true;
                }
                emit finnished();
            }
            else {
                emit error(status,
                    QString("Inquiry completed with error (code %1)")
                    .arg(status));
            }
        }
        break;
    case EVT_INQUIRY_RESULT:
        {
            int numResults = data[0];
            inquiry_info *results = (inquiry_info*)(data+1);
            for (int n=0; n<numResults; n++)
            {

                // push the address to the address queue
                // where it can be consumed by nextNeighbour()
                InquiryInfo info;
                info.addr = DeviceAddress(results[n].bdaddr);
                if (addrCache.find(info.addr) == addrCache.end()) {
                    addrCache.insert(info.addr);
                    int deviceClass = (results[n].dev_class[0] << 16) |
                        (results[n].dev_class[1] << 8) |
                        (results[n].dev_class[2] << 0);
                    info.deviceClass = deviceClass;
                    infoQueue.push_back(info);

                    kdDebug() << "INQUIRY_RESULT: "
                        << QString(info.addr) <<  endl;
                    // call the handler. Emits a signal if not overwritten
                    onNeighbourFound(info.addr, info.deviceClass);
                }
            }
        }
        break;
    case EVT_INQUIRY_RESULT_WITH_RSSI:
        {
            int numResults = data[0];
            inquiry_info_with_rssi *results = (inquiry_info_with_rssi*)(data+1);
            for (int n=0; n<numResults; n++)
            {

                // push the address to the address queue
                // where it can be consumed by nextNeighbour()
                InquiryInfo info;
                info.addr = DeviceAddress(results[n].bdaddr);
                if (addrCache.find(info.addr) == addrCache.end()) {
                    addrCache.insert(info.addr);
                    int deviceClass = (results[n].dev_class[0] << 16) |
                        (results[n].dev_class[1] << 8) |
                        (results[n].dev_class[2] << 0);
                    info.deviceClass = deviceClass;
                    infoQueue.push_back(info);

                    kdDebug() << "INQUIRY_RESULT_WITH_RSSI: "
                        << QString(info.addr) <<  endl;
                    // call the handler. Emits a signal if not overwritten
                    onNeighbourFound(info.addr, info.deviceClass);
                }
            }
        }
        break;
    case EVT_CMD_STATUS:
        {
            int status = data[0];
            int numHciCmdPkts = data[1];
            int cmdOpcode = *((uint16_t*)(data+2));
            kdDebug() << "EVT_CMD_STATUS status=" << status
                << " numPkts=" << numHciCmdPkts << " cmdOpcode="
                << cmdOpcode << endl;
            if (cmdOpcode == OCF_INQUIRY)
            {
            }
        }
        break;
    }
}


}

#include "inquiry.moc"
