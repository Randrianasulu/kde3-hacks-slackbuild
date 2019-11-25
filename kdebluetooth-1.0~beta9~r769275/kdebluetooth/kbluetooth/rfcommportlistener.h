//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _RFCOMMPORTLISTENER_H_
#define _RFCOMMPORTLISTENER_H_
#include "portlistener.h"

#include <qguardedptr.h>
#include <qsocketnotifier.h>
#include <kprocess.h>
#include <kservice.h>
#include <algorithm>
#include <libkbluetooth/adapter.h>
#include "confirmation.h"
#include "sdprecord.h"

class MetaServer;
namespace KBluetooth {
    class DeviceAddress;
}

class RfcommPortListener : public PortListener
{
    Q_OBJECT
public:
    RfcommPortListener(KConfig *c,KBluetooth::Adapter*);
    virtual ~RfcommPortListener();
    virtual bool acquirePort();
    virtual bool isValid();
    virtual QString name();
    virtual void setEnabled(bool enabled);
    virtual void setServiceRegistrationEnabled(bool enabled);
    virtual bool isServiceRegistrationEnabled();
    virtual QDateTime expiration();
    virtual QDateTime serviceLifetimeEnd();
    virtual bool isEnabled();
    virtual void setAuthentication(bool enable);
    virtual void setEncryption(bool enable);
    virtual bool getAuthentication();
    virtual bool getEncryption();
    QStringList resourceTypes();
    QString resource(QString resourceType);
    virtual void refreshRegistration();
    virtual QString docPath();
    virtual QString description();
    virtual void configure();
    virtual bool canConfigure();
    QGuardedPtr<QSocketNotifier> m_socketNotifier;

private:
    void freePort();
    bool setPort(int port = -1, int autoProbeRange = 1);
    void loadConfig(KService::Ptr s);
    void setServiceRegistrationEnabledInternal(bool e);
    void setEnabledInternal(bool e, const QDateTime &ex);
    bool authenticateConnection(const KBluetooth::DeviceAddress& deviceaddress);
    void checkClassOfDevice();
    
    int m_rfcommPort;
    int m_rfcommPortBase;
    int m_rfcommPortRange;
    int m_suggestedClass;
    int m_suggestedClassMask;
    bool m_showedClassWarning;
    int m_rfcommPortPreferred;
    bool m_multiInstance;
    bool m_valid;
    bool m_enabled;
    bool m_authenticate;
    bool m_encrypt;
    bool m_serviceRegistered;
    bool m_registerService;
    QString m_execPath;
    QString m_argStrRfcommChannel;
    QString m_argStrPeerAddr;
    QString m_argStrPeerName;
    QString m_strSdpRecord;
    Bluetooth::SdpRecord *m_sdpRecord;
    QString m_serviceName;


    QGuardedPtr<KConfig> m_config;
    QString sDocPath;
    QString sDescription;
    QString sConfigParam;
//    MetaServer *metaServer;
    KBluetooth::Adapter* m_adapter;
    Confirmation* confirmation;
            
public slots:
    void slotIncomingRfcommConnection(int socket);

signals:
    void kbtobexsrvConn(QString&,QString,int&);
};

#endif
