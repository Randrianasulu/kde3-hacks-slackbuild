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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <kdebug.h>

#include <termios.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <netinet/in.h>

#include "adapter_old.h"
#include <qstring.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <hcisocket.h>

namespace KBluetooth
{

DeviceAddress RAdapter::getAddress() const
{
    return address;
}

QString RAdapter::getName() const
{
    return nameStr;
}

int RAdapter::getIndex() const
{
    return index;
}

RAdapter::RAdapter(int index, const DeviceAddress& address, QString name)
{
    this->index = index;
    this->address = address;
    this->nameStr = name;
}


RAdapter::ConnectionInfoVector RAdapter::getAclConnections() const
{
    ConnectionInfoVector ret;

    hci_conn_list_req *cl;
    hci_conn_info *ci;
    char buf[10*sizeof(*ci) + sizeof(*cl)];
    cl = (hci_conn_list_req*)buf;
    cl->dev_id = getIndex();
    cl->conn_num = 10;
    ci = cl->conn_info;

    HciSocket s(NULL, "", getIndex());
    if (!s.open()) {
        return ret;
    }

    if (ioctl(s.socket(), HCIGETCONNLIST, (void*)cl)) {
        kdWarning() << "Can't get connection list: "
            << QString::fromLocal8Bit(strerror(errno)) <<  endl;
        return ret;
    }

    for (int i=0; i < cl->conn_num; i++, ci++) {
        if (ci->type == ACL_LINK) {
            ConnectionInfo i;
            i.address = DeviceAddress(ci->bdaddr, false);
            i.handle = ci->handle;
            i.out = (ci->out != 0);
            i.type = ConnectionType(ci->type);
            switch (ci->state) {
            case 0: i.state = NOT_CONNECTED; break;
            case 1: i.state = CONNECTED; break;
            case 5: i.state = CONNECTING; break;
            default: i.state = UNKNOWN_STATE;
            }
            i.link_mode = ci->link_mode;
            ret.push_back(i);
        }
    }

    return ret;
}

RAdapter::ConnectionState RAdapter::getAclConnectionState(const DeviceAddress& addr) const
{
    ConnectionState ret = UNKNOWN_STATE;

    hci_conn_list_req *cl;
    hci_conn_info *ci;
    char buf[10*sizeof(*ci) + sizeof(*cl)];
    cl = (hci_conn_list_req*)buf;
    cl->dev_id = getIndex();
    cl->conn_num = 10;
    ci = cl->conn_info;

    HciSocket s(NULL, "", getIndex());
    if (!s.open()) {
        return ret;
    }

    if (ioctl(s.socket(), HCIGETCONNLIST, (void*)cl)) {
        kdWarning() << "Can't get connection list"
            << QString::fromLocal8Bit(strerror(errno)) <<  endl;
        return ret;
    }

    int state = 0;
    for (int i=0; i < cl->conn_num; i++, ci++) {
        if ((ci->type == ACL_LINK) &&
            (DeviceAddress(ci->bdaddr,false) == addr)) {
            state = ci->state;
        }
    }

    switch (state) {
    case 0: ret = NOT_CONNECTED; break;
    case 1: ret = CONNECTED; break;
    case 5: ret = CONNECTING; break;
    default: ret = UNKNOWN_STATE;
    }

    return ret;
}


int RAdapter::getClassOfDevice() const
{
    HciSocket s(NULL, "", getIndex());
    if (!s.open()) {
        return -1;
    } 
    uint8_t cls[3];
    if (hci_read_class_of_dev(s.socket(), cls, 1000) < 0) {
        s.close();
        return -1;
    }
    s.close();
    return cls[2]*256*256+cls[1]*256+cls[0];
}

Adapters::Adapters()
{
    update();
}

int Adapters::count()
{
    return adapters.size();
}

const RAdapter& Adapters::operator[](int n) const
{
    return adapters[n];
}

void Adapters::update()
{
    adapters.clear();
    hci_for_each_dev(HCI_UP, dev_info, (long int)this);
}

int Adapters::dev_info(int s, int dev_id, long arg)
{
    reinterpret_cast<Adapters*>(arg)->dev_info(s, dev_id);
    return 0;
}

void Adapters::dev_info(int s, int dev_id)
{
    struct hci_dev_info di;
    di.dev_id = dev_id;
    if (ioctl(s, HCIGETDEVINFO, (void*) &di))
        return;

    RAdapter a = RAdapter(dev_id, DeviceAddress(di.bdaddr), QString(di.name));
    adapters.push_back(a);
}
}
