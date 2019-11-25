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

#ifndef BTADAPTER_H
#define BTADAPTER_H

#include <vector>
#include <qstring.h>
#include <qvaluevector.h>
#include "deviceaddress.h"

namespace KBluetooth
{

/** Class representing a local bluetooth adapter.
 * This class provides access to the properties of a local bluetooth
 * adapter. This class has no public constructors. You can get the
 * list of available adapters using KBluetooth::Adapters.
 */
class RAdapter
{
    friend class Adapters;
public:
    enum ConnectionType { SCO = 0, ACL = 1 };
    enum ConnectionState { NOT_CONNECTED, CONNECTED, CONNECTING, UNKNOWN_STATE };
    struct ConnectionInfo {
        DeviceAddress address;
        int handle;
        ConnectionType type;
        ConnectionState state;
        bool out;
        uint link_mode;
    };
    typedef QValueVector<ConnectionInfo> ConnectionInfoVector;

    /** Returns the device address of the given adapter.
    @return The device address
    */
    DeviceAddress getAddress() const;

    /** Returns the name of the adapter.
    @return The device name
    */
    QString getName() const;

    /** Returns the device/service class
    @return class of device
    */
    int getClassOfDevice() const;
    
    /** Returns the index of the device.
    @return device index
    */
    int getIndex() const;

    /** Return a deviceaddress for each ACL-Connection */
    ConnectionInfoVector getAclConnections() const;

    ConnectionState getAclConnectionState(const DeviceAddress& addr) const;

protected:
    /** Constructor used internally by KBluetooth::Adapters. */
    RAdapter(int index, const DeviceAddress& address, QString nameStr);
private:
    int index;
    QString nameStr;
    DeviceAddress address;
};


/** Enumerates the locally attached bluetooth devices.
 * The information in this class stays static until update()
 * is called.
 */
class Adapters
{
public:
    /** Default constructor.
    Calls update().
    */
    Adapters();

    /** Gets the number of adapters.
    @return number of adapters
    */
    int count();

    /** Returns a single adapter
    @param n index of the adapter (0 <= n < count())
    @return adapter info
    */
    const RAdapter& operator[](int n) const;

    /** Refreshes the list of adapters. */
    void update();
private:
    std::vector<RAdapter> adapters;
    static int dev_info(int s, int dev_id, long arg);
    void dev_info(int s, int dev_id);
};

}

#endif

