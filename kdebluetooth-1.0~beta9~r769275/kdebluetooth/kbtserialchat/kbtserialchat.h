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

#ifndef _KBTSERIALCHAT_H_
#define _KBTSERIALCHAT_H_

#include <kmainwindow.h>
#include "maindialogbase.h"
#include <libkbluetooth/rfcommsocket.h>
#include <libkbluetooth/deviceaddress.h>

/**
 * @short Application Main Window
 * @author Fred Schaettgen <kbluetoothd@schaettgen.de>
 * @version 0.1
 */
class MainDialog : public MainDialogBase
{
    Q_OBJECT
public:
    MainDialog();
    MainDialog(KBluetooth::DeviceAddress addr, int channel);
    MainDialog(int s, QString addr, QString name);
    virtual ~MainDialog();

private:
    KBluetooth::RfcommSocket *socket;
    void saveMru(KBluetooth::DeviceAddress addr, int channel);

private slots:
    void slotSendCommand();
    void slotSocketReadyRead();
    void slotSocketError(int);
    void slotSocketConnectionClosed();
};

#endif // _KBTSERIALCHAT_H_
