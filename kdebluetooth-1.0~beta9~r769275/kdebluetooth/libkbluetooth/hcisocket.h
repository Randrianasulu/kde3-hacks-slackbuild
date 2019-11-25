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

#ifndef KBLUETOOTHHCISOCKET_H
#define KBLUETOOTHHCISOCKET_H

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qsocketdevice.h>
#include <qguardedptr.h>

class QSocket;

namespace KBluetooth
{

/** Bluetooth HCI Socket class.
 * This class provides socket level access to the Blueztooth HCI layer.
 * It is set up to filter out all but HCI event packets, since more
 * is only allowed for privileged users.
 * @todo Possiblity to us not only the first bluetooth adapter
 * @todo writeHciEvent() function
 * @todo error handling
 */
class HciSocket : public QObject
{
    Q_OBJECT
public:
    /** default constructor */
    HciSocket(QObject* parent = NULL,
        const char* name = NULL);

    /** contstructor with explicit device parameter*/
    HciSocket(QObject* parent,
        const char* name, int devNum);

    /** destructor */
    virtual ~HciSocket();

    /** Opens a HCI socket for the first bluetooth device.
    @return true if successfully opened, false otherwise
    */
    virtual bool open();

    /** Closes the HCI socket. */
    virtual void close();

    bool sendCommand(unsigned char ogf, unsigned short ocf,
        QByteArray buf);
    bool readStatus(unsigned char ogf, unsigned short ocf, 
        int *status, int timeout_ms = 1000);
    
    /** Reads whole HCI packet.
    @param packetType [out] The packet type. Should always be ...
    @param eventCode [out] The event code.
    @param buflen [in/out] The maximum size of the buffer / the number of
    bytes written into the buffer.
    @param paramBuf pointer to a buffer for the HCI event packet
    @return true if successful
    */
    /*bool readEvent(unsigned char &packetType,
                      unsigned char &eventCode, unsigned char &buflen,
                      char* paramBuf);*/

    enum Error { ErrSocket = 1 };

    /** Forces reading the next event packet. */
    void readEvent();

    /** Returns the internal socket */
    int socket();
    QSocketDevice& socketDevice();

signals:
    void event(unsigned char eventCode, QByteArray buf);
    void error(int code, QString message);
    void connectionClosed();

private:
    void updateStatus(const QByteArray& data);

    //QSocketDevice hciSocket;
    QGuardedPtr<QSocketNotifier> hciReadNotifier;
    QSocketDevice hciSocket;
    int devIndex;
    
    bool bStatusSet;
    unsigned short lastStatusOcf;
    unsigned char lastStatusOgf;
    int lastStatus;

private slots:
    void slotSocketActivated();
    void slotSocketError(int e);
    void slotConnectionClosed();
};

}

#endif
