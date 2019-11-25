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

#ifndef KBLUETOOTHNAMEREQUEST_H
#define KBLUETOOTHNAMEREQUEST_H

#include <qobject.h>
#include "deviceaddress.h"

namespace KBluetooth
{

/** Gets the name of a bluetooth device from its device address.
With NameRequest it is possible to retrieve the name of a remote bluetooth
device. Note that you shouldn't try to resolve names during
an ongoing inquiry.
*/
class NameRequest : public QObject
{
    Q_OBJECT
public:
    /** Returns the name of a bluetooth device.
    @param addr The bluetooth address of the other device
    @return The name of the device or QString::null in case of an error.
    */
    QString resolve(DeviceAddress addr);

    /** Gets the error message in case resolve() has failed.
    @return The last error message. Undefined if there was no error.
    */
    QString lastErrorMessage();

private:
    QString errorMessage;
};

}

#endif
