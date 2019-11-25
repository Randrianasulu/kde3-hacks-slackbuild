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

#include "deviceaddress.h"

namespace KBluetooth
{

DeviceAddress::DeviceAddress()
{
    this->isValid = false;
    bdaddr_t addr_none = {{0,0,0,0,0,0}};
    this->bdaddr = addr_none;
}

DeviceAddress::DeviceAddress(const bdaddr_t& bdaddr, bool networkbyteorder)
{
    if (networkbyteorder)
    {
        baswap(&this->bdaddr, &bdaddr);
    }
    else
    {
        this->bdaddr = bdaddr;
    }
    this->isValid = true;
}

DeviceAddress::operator QString() const
{
    char addrbuf[20];
    ba2str(&this->bdaddr, addrbuf);
    return QString(addrbuf);
}

DeviceAddress::DeviceAddress(const QString& s)
{
    bdaddr_t a;
    int ret = str2ba(s.ascii(), &a);
    if (ret==0)
    {
        this->isValid = true;
        this->bdaddr = a;
    }
    else
    {
        this->isValid = false;
        this->bdaddr = DeviceAddress::invalid.getBdaddr();
    }
}

bdaddr_t DeviceAddress::getBdaddr(bool networkbyteorder) const
{
    bdaddr_t ret;
    if (networkbyteorder)
    {
        baswap(&ret, &this->bdaddr);
    }
    else
    {
        ret = bdaddr;
    }
    return ret;
}

bool DeviceAddress::operator==(const DeviceAddress& b) const
{
    if (isValid == false && b.isValid == false)
        return true;
    return ((bdaddr.b[0] == b.bdaddr.b[0]) &&
            (bdaddr.b[1] == b.bdaddr.b[1]) &&
            (bdaddr.b[2] == b.bdaddr.b[2]) &&
            (bdaddr.b[3] == b.bdaddr.b[3]) &&
            (bdaddr.b[4] == b.bdaddr.b[4]) &&
            (bdaddr.b[5] == b.bdaddr.b[5]));
}

bool DeviceAddress::operator<(const DeviceAddress& b) const
{
    if (isValid == false && b.isValid == false)
        return false;
    else if (isValid == false && b.isValid == true)
        return false;
    else if (isValid == true && b.isValid == false)
        return true;

    if (bdaddr.b[5] != b.bdaddr.b[5]) return (bdaddr.b[5] < b.bdaddr.b[5]);
    if (bdaddr.b[4] != b.bdaddr.b[4]) return (bdaddr.b[4] < b.bdaddr.b[4]);
    if (bdaddr.b[3] != b.bdaddr.b[3]) return (bdaddr.b[3] < b.bdaddr.b[3]);
    if (bdaddr.b[2] != b.bdaddr.b[2]) return (bdaddr.b[2] < b.bdaddr.b[2]);
    if (bdaddr.b[1] != b.bdaddr.b[1]) return (bdaddr.b[1] < b.bdaddr.b[1]);
    if (bdaddr.b[0] != b.bdaddr.b[0]) return (bdaddr.b[0] < b.bdaddr.b[0]);
    return false;
}


//const bdaddr_t DeviceAddress::bdaddr_any = {{0,0,0,0,0,0}};
//const bdaddr_t DeviceAddress::bdaddr_local = {{0, 0, 0, 0xff, 0xff, 0xff}};

const DeviceAddress DeviceAddress::invalid = DeviceAddress();
const DeviceAddress DeviceAddress::any = DeviceAddress("00:00:00:00:00:00");
const DeviceAddress DeviceAddress::local = DeviceAddress("00:00:00:FF:FF:FF");
}
