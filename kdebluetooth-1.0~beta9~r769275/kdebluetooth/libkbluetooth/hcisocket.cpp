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

#include "hcisocket.h"

#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "deviceaddress.h"
#include <qbuffer.h>
#include <assert.h>
#include <kdebug.h>
#include "hcidefault.h"
#include <qtimer.h>
#include <klocale.h>
#include <qdatastream.h>

#include <unistd.h>

namespace KBluetooth
{

HciSocket::HciSocket(QObject* parent, const char* name) :
    QObject(parent, name)
{
    bStatusSet = false;
    this->devIndex = HciDefault::defaultHciDeviceNum();
}

HciSocket::HciSocket(QObject* parent, const char* name, int devNum) :
    QObject(parent, name)
{
    this->devIndex = devNum;
}

HciSocket::~HciSocket()
{
    close();
}

bool HciSocket::open()
{
    kdDebug() << "HciSocket::open()" << endl;
    int s;
    s = ::socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (s < 0)
    {
        kdDebug() << "HciSocket::open(): Error creating socket" << endl;
        emit error(errno, i18n("Error creating socket"));
        return false;
    }

    /* Bind socket to the HCI device */
    struct sockaddr_hci sa;
    sa.hci_family = AF_BLUETOOTH;
    sa.hci_dev = devIndex;
    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        kdDebug()<< "Bind failed: " << strerror(errno) << endl;
        ::close(s);
        emit error(errno, i18n("Error binding to socket"));
        return false;
    }

    struct hci_filter flt;
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);
    if (setsockopt(s, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0)
    {
        kdWarning() << "HCI filter setup failed: " << strerror(errno) << endl;
        ::close(s);
        emit error(errno, i18n("HCI filter setup failed"));
        return false;
    }

    if (hciReadNotifier) {
        delete hciReadNotifier;
    }
    hciSocket.setSocket(s, QSocketDevice::Datagram);
    hciReadNotifier = new QSocketNotifier(s, QSocketNotifier::Read, this);

    connect(hciReadNotifier, SIGNAL(activated(int)),
        this, SLOT(slotSocketActivated()));
    //connect(hciSocket, SIGNAL(error(int)),
    //    this, SLOT(slotSocketError(int)));
    //connect(hciSocket, SIGNAL(connectionClosed()),
    //    this, SLOT(slotConnectionClosed()));
    //hciSocket->setSocket(s);

    return true;
}

void HciSocket::close()
{
    if (hciReadNotifier) {
        delete hciReadNotifier;
    }
    if (hciSocket.isValid()) {
        hciSocket.close();
    }
}

void HciSocket::slotSocketError(int e)
{
    close();
    emit error(e, i18n("HCI socket error: 0x%1.").arg(e,2,16));
}

void HciSocket::slotSocketActivated()
{
    QSocketDevice::Error err = hciSocket.error();
    if ((err == QSocketDevice::NoError) && (hciSocket.isValid())) {
        //kdDebug() << "HCI socket ready read." << endl;

        unsigned char buf[512];
        int psize = hciSocket.readBlock((char*)buf, 512);
        if (psize <= 0) {
            slotSocketError(hciSocket.error());
            hciSocket.close();
            return;
        }

        //unsigned char packetType = buf[0];
        unsigned char eventCode = buf[1];
        unsigned char len = buf[2];
        if (psize-3 == len) {
            QByteArray databuf;
            databuf.duplicate((char*)(buf+3), len);
            emit event(eventCode, databuf);
            if (eventCode == EVT_CMD_STATUS) {
                updateStatus(databuf);
            }
        }
        else {
            kdWarning() << 
                QString("Error reading hci packet: packetSize(%1)-3 != dataSize(%2)")
                .arg(psize).arg(len) << endl;
        }
    }
    else if (err == QSocketDevice::NoError) {
        slotConnectionClosed();
    }
    else {
        hciSocket.close();
        slotSocketError(err);
    }
}

void HciSocket::updateStatus(const QByteArray& data)
{
    QDataStream stream(data, IO_ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    bStatusSet = true;
    Q_UINT8 status, dummy;
    Q_UINT16 opcode;
    stream >> status >> dummy >> opcode;
    //kdDebug() << "updatestatus opcode=" << uint32_t(opcode) << endl;
    lastStatus = status;
    lastStatusOgf = cmd_opcode_ogf(opcode);
    lastStatusOcf = cmd_opcode_ocf(opcode);
}

void HciSocket::slotConnectionClosed()
{
    kdDebug() << "HCI connection closed." << endl;
    emit connectionClosed();
}

void HciSocket::readEvent()
{
    if (hciReadNotifier) {
        slotSocketActivated();
    }
}

bool HciSocket::sendCommand(unsigned char ogf,
    unsigned short ocf, QByteArray buf)
{
    QBuffer packet;
    QDataStream stream(&packet);
    stream.setByteOrder(QDataStream::LittleEndian);
    packet.open(IO_WriteOnly);
    if (buf.size() > 255) return false;
    //kdDebug() << "sendCommand. ogf=" << ogf << " ocf=" << ocf << endl;
    Q_UINT16 opcode = cmd_opcode_pack(ogf, ocf);
    Q_UINT8 pType = HCI_COMMAND_PKT;
    Q_UINT8 buflen = buf.size();
    stream << pType << opcode << buflen;
    stream.writeRawBytes(buf.data(), buflen);
    packet.close();
    hciSocket.writeBlock((const char*)packet.buffer(), packet.buffer().size());
    return true;
}

bool HciSocket::readStatus(unsigned char ogf, unsigned short ocf, 
    int *status, int timeout_ms)
{
    QTimer timer;
    timer.start(timeout_ms, true);
    bStatusSet = false;
    while (timer.isActive() && hciSocket.isValid()) {
        kdDebug() << "HciSocket::readStatus()" << endl;
        bool timeout = false;
        if (hciSocket.bytesAvailable() == 0) {
            hciSocket.waitForMore(timeout_ms, &timeout);
        }
        if (!timeout) {
            slotSocketActivated();
        }
        if (bStatusSet == true && ogf == lastStatusOgf && ocf == lastStatusOcf) {
            *status = lastStatus;
            kdDebug() << QString("HciSocket::readStatus(ogf=%1,ocf=%2,timeout=%3) = %4")
                .arg(ogf).arg(ocf).arg(timeout_ms).arg(lastStatus) << endl;
            return true;
        }
    }
    kdDebug() << QString("HciSocket::readStatus(ogf=%1,ocf=%2,timeout=%3): timeout!")
        .arg(ogf).arg(ocf).arg(timeout_ms) << endl;
    return false;
}

int HciSocket::socket()
{
    return hciSocket.socket();
}

QSocketDevice& HciSocket::socketDevice()
{
    return hciSocket;
}


}
#include "hcisocket.moc"
