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

#ifndef KBLUETOOTHRFCOMMSOCKET_H
#define KBLUETOOTHRFCOMMSOCKET_H

#include <qstring.h>
#include <qsocket.h>
#include "deviceaddress.h"

namespace KBluetooth {


/** Bluetooth Rfcomm socket class.
 * Allows to open Rfcomm connections to other bluetooth devices
 * and to use the socket the "Qt way" by connecting to the
 * inherited signals socketReadyRead() and error().
 */
class RfcommSocket : public QSocket
{
Q_OBJECT
public:
    /** Constructor.
    @param parent parent object
    @name object name
    */
    RfcommSocket(QObject* parent = NULL, const char* name = NULL);

    /** Destructor */
    virtual ~RfcommSocket();

    /** Opens a rfcomm connection.
    @param addr The target bluetooth device address.
    @param channel The rfcomm channel to use.
    */
    virtual void connectToHost(
        const KBluetooth::DeviceAddress& addr, int channel);
        
    KBluetooth::DeviceAddress addr() const;
private:
    KBluetooth::DeviceAddress m_addr;
};

};

#endif
