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

#include "rfcommportlistener.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <ksock.h>
#include <qvaluevector.h>
#include <qdom.h>
#include <qfile.h>
#include <kdebug.h>
#include <kconfig.h>
#include <ksockaddr.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <knotifyclient.h>
#include <kapplication.h>
#include <kmessagebox.h>
//#include <libkbluetooth/namerequest.h>
//#include <libkbluetooth/adapter.h>
//#include <libkbluetooth/hcidefault.h>
//#include <libkbluetooth/namecache.h>
#include <bluetooth/l2cap.h>
#include "confirmation.h"
#include "procinheritsock.h"
//#include "metaserver.h"

#if !defined(RFCOMM_LM) || !defined(RFCOMM_LM_AUTH) || !defined(RFCOMM_LM_ENCRYPT)
# define RFCOMM_LM       0x03
# define RFCOMM_LM_AUTH          0x0002
# define RFCOMM_LM_ENCRYPT       0x0004
#endif

using namespace KBluetooth;

RfcommPortListener::RfcommPortListener(KConfig *config,KBluetooth::Adapter* adapter) :
    PortListener(this) 
{

    m_adapter = adapter;
    m_config = config;
    confirmation = new Confirmation(m_config);


    m_showedClassWarning = false;
    m_enabled = false;
//    this->metaServer = metaServer;
    m_rfcommPort = -1;
    m_sdpRecord = NULL;

    m_config->setGroup("ListenerConfig");

    kdDebug() << "Name: " << name() << endl;
    KService::Ptr s = KService::serviceByDesktopName("kbluetooth_kbtobexsrv");
    loadConfig(s);

    if (m_valid && m_enabled)
        acquirePort();
    m_enabled = (m_rfcommPort >= 0);
    kdDebug() << "Kbluetoothd: RfcommPortListener valid=" << m_valid
    << " enabled=" << m_enabled << endl;
}

bool RfcommPortListener::acquirePort()
{
    kdDebug() << "RfcommPortListener::acquirePort()" << endl;
    // Delete an existing SocketNotifier if
    // its port isn't in the right range
    if (m_socketNotifier)
    {
        if ((m_rfcommPort >= m_rfcommPortBase) &&
                (m_rfcommPort <= m_rfcommPortBase+m_rfcommPortRange))
            return true;
        else
            delete m_socketNotifier;
    }

    // Find an unused rfcomm port, bind() to it and listen()
    int port = -1;
    int s = -1;
    bool bindListenSucceeded = false;
    for (port = m_rfcommPortBase; 
        port <= std::min(255, m_rfcommPortBase+m_rfcommPortRange); ++port)
    {
        // Create an rfcomm socket
        kdDebug() << "Kbluetoothd: looking for a free rfcomm port. " << endl;
        s = ::socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (s < 0)
        {
            printf("Can't create socket. %s (%d)\n", strerror(errno), errno);
            KMessageBox::detailedError(KApplication::kApplication()->mainWidget(),
                i18n("Could not create a socket for service %1.").arg(this->name()),
                i18n("<p>KBluetooth was unable to create a socket \
for the service %1. The error message was <i>%2</i> (Error code %3).<p>\
Make sure that BlueZ is installed correctly and all modules are loaded \
(bluez, l2cap, rfcomm..).").arg(this->name()).arg(strerror(errno)).arg(errno), "KBluetoothD");
            return false;
        }

        sockaddr_rc local_addr;
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.rc_family = AF_BLUETOOTH;
        bdaddr_t bdaddr_any = {{0, 0, 0, 0x00, 0x00, 0x00}};
        local_addr.rc_bdaddr = bdaddr_any;
        local_addr.rc_channel = port;

        if (::bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr)) >= 0)
        {
            int opt = 0;
            if (m_authenticate) {
                opt |= RFCOMM_LM_AUTH;
                kdDebug() << "Requesting authentication for " << name() << endl;
            }
            if (m_encrypt) {
                opt |= RFCOMM_LM_ENCRYPT;
                kdDebug() << "Requesting encryption for " << name() << endl;
            }
            if (opt != 0) {
                if (setsockopt(s, SOL_RFCOMM, RFCOMM_LM, &opt, sizeof(opt)) < 0) 
                {
                    KMessageBox::detailedError(
                        KApplication::kApplication()->mainWidget(), 
i18n("<p>Could not enable security features for <b>%1</b>.\
The service will not be started.</p>")
.arg(this->name()),
i18n("<p>KBluetoothD was unable to enable authentication and/or encryption \
for the service %1, so this service will not be started.</p>\
<p> The error code returned by setsockopt() was %3 (<i>%2</i>)</p>\
<p>To use encryption/authentication for selected services, you need a Linux \
kernel >= 2.6.10-mh3. For older kernels you have to disable encryption and authentication for this service.<br/> <i>Be careful with allowing unconfirmed connections in that case, since the peer address could be faked.</i></p>")
.arg(this->name()).arg(strerror(errno)).arg(errno), "KBluetoothD");
                    return false;
                }
                
            }
        
            // bind succeeded
            if (::listen(s, 10) >= 0)
            {
                // listen succeeded
                kdDebug() << "Kbluetoothd: found free channel: " << port << endl;
                bindListenSucceeded = true;
                break;
            }
            else
            {
                kdDebug() << "Kbluetoothd: could not open channel " << port << endl;
                // listen failed
                ::close(s);
                if (errno == EADDRINUSE)
                {
                    // port is in use -> try another one..
                    continue;
                }
                else
                {
                    // other error -> abort.
                    kdDebug() << "Kbluetoothd: Cannot listen to socket. " << strerror(errno)
                    << " (" << errno << ")" << endl;
                    KMessageBox::detailedError(KApplication::kApplication()->mainWidget(),
                        i18n("Could not listen to a socket for service %1.").arg(this->name()),
                        i18n("<p>KBluetoothD was unable to listen to a socket \
for the service %1. The error message was <i>%2</i> (Error code %3).<p>\
Make sure that BlueZ is installed correctly and all modules are loaded \
(bluez, l2cap, rfcomm..).").arg(this->name()).arg(strerror(errno)).arg(errno), "KBluetoothD");
                    return false;
                }
            }
        }
        else
        {
            kdDebug() << "Kbluetoothd: could not open channel " << port << endl;
            // bind failed
            ::close(s);
            if (errno == EADDRINUSE)
            {
                // port is in use -> try another one..
                continue;
            }
            else
            {
                // other error -> abort.
                kdDebug() << "Kbluetoothd: Cannot bind to socket. " << strerror(errno)
                << " (" << errno << ")" << endl;
                KMessageBox::detailedError(KApplication::kApplication()->mainWidget(),
                    i18n("Could not bind to a socket for service %1.").arg(this->name()),
                    i18n("<p>KBluetoothD was unable to bind to a socket \
for the service %1 on channel %2. The error message was <i>%3</i> (Error code %4).<p>\
Make sure that BlueZ is installed correctly.")
                    .arg(this->name()).arg(port).arg(strerror(errno)).arg(errno),
                    "KBluetoothD");
                return false;
            }
        }
    }

    if (bindListenSucceeded == false)
    {
        // No free port was found
        kdWarning() << "Kbluetoothd: Could not find a free rfcomm port." << endl;
        KMessageBox::detailedError(KApplication::kApplication()->mainWidget(),
            i18n("Could not assign a channel to service %1.").arg(this->name()),
            i18n("<p>KBluetoothD was unable to find an unused Rfcomm channel \
for the service %1, so other devices will not be able to access that service.</p>\
<p>\
Possible reasons:<br/> \
<ul> \
<li>Several services are configured to use the same channel.</li> \
<li>Other Bluetooth services are blocking the ports. </li> \
</ul> \
Checklist: \
<ul> \
<li>%2 will try to use channels form %3 to %4 - are they all used by other services?\
<li>Does <i>/proc/bluetooth/rfcomm</i> show more listening sockets than \
the Bluetooth services tab in the Control Center? \
</ul> \
</p>").arg(this->name()).arg(this->name()).arg(m_rfcommPortBase)
      .arg(std::min(255, m_rfcommPortBase+m_rfcommPortRange)), "KBluetoothD");
        return false;
    }
    m_rfcommPort = port;

    // We've got a listening socket now..
    kdDebug() << "Kbluetoothd: free socket found.  " <<  s << "port: " << port << endl;
	
    m_socketNotifier = new QSocketNotifier(s, QSocketNotifier::Read,this, "rfcommServerSocket");
    connect(m_socketNotifier, SIGNAL(activated(int)),this, SLOT(slotIncomingRfcommConnection(int)));

    bool b = m_registerService;
    setServiceRegistrationEnabledInternal(false);
    setServiceRegistrationEnabledInternal(b);
    return true;
}

void RfcommPortListener::freePort()
{
    kdDebug() << "Kbluetoothd: freePort" << endl;
    m_rfcommPort = -1;
    if (m_socketNotifier)
    {
        ::close(m_socketNotifier->socket());
        delete m_socketNotifier;
    }
    setServiceRegistrationEnabledInternal(m_registerService);
}

void RfcommPortListener::loadConfig(KService::Ptr s)
{
    QString p = "X-KDE-KBLUETOOTHD-";

    m_execPath = KStandardDirs().findResource("exe", s->exec());
    kdDebug () << "EXEC: " << m_execPath << endl;
    if (m_execPath.isNull())
    {
        kdDebug() << "Kbluetoothd: freePort" << endl;
        m_valid = false;
    }
    else
    {
        m_valid = true;
    }

    m_registerService = false;

    QVariant vServiceName = s->property(p + "id",QVariant::String);
	
	kdDebug() << "servicename" << vServiceName.toString() << endl;
    if (vServiceName.isValid())
    {
        m_serviceName = vServiceName.toString();
    }
    else
    {
        kdDebug() << "Kbluetoothd cannot load service "<< m_execPath
        <<": no id set" << endl;
        m_valid = false;
        return;
    }

    QVariant vPortBase = s->property(p + "port",QVariant::Int);
    if (vPortBase.isValid())
    {
        m_rfcommPortBase = vPortBase.toInt();
    }
    else
    {
        m_rfcommPortBase = 1;
    }

    QVariant vPortRange = s->property(p + "autoPortRange",QVariant::Int);
    if (vPortRange.isValid())
    {
        m_rfcommPortRange = vPortRange.toInt();
    }
    else
    {
        m_rfcommPortRange = 32;
    }

    QVariant vMultiInstance = s->property(p + "multiInstance",QVariant::Bool);
    if (vMultiInstance.isValid())
    {
        m_multiInstance = vMultiInstance.toBool();
    }
    else
    {
        m_multiInstance = true;
    }

    QVariant vEnabled = s->property(p + "enabled",QVariant::Bool);
    if (vEnabled.isValid())
    {
        m_enabled = vEnabled.toBool();
    }
    else
    {
        m_enabled = true;
    }
    m_enabled = m_config->readBoolEntry("enabled_" + m_serviceName,
                                        m_enabled);

    QVariant vEncrypt = s->property(p + "encryption",QVariant::Bool);
    if (vEncrypt.isValid())
    {
        m_encrypt = vEncrypt.toBool();
    }
    else
    {
        m_encrypt = true;
    }
    m_encrypt = m_config->readBoolEntry("encrypt_" + m_serviceName,
                                        m_encrypt);

    QVariant vAuthenticate = s->property(p + "authentication",QVariant::Bool);
    if (vAuthenticate.isValid())
    {
        m_authenticate = vAuthenticate.toBool();
    }
    else
    {
        m_authenticate = true;
    }
    m_authenticate = m_config->readBoolEntry("authenticate_" + m_serviceName,
                                        m_authenticate);
                                                                                
    QVariant vArgument = s->property(p + "argument",QVariant::String);
    if (vArgument.isValid())
    {
        m_argStrRfcommChannel = vArgument.toString();
    }
    else
    {
        m_argStrRfcommChannel = "--rfcommconnection";
    }

    QVariant vArgAddr = s->property(p + "arg-addr",QVariant::String);
    if (vArgAddr.isValid())
    {
        m_argStrPeerAddr = vArgAddr.toString();
    }
    else
    {
        m_argStrPeerAddr = QString::null;
    }

    QVariant vArgName = s->property(p + "arg-name",QVariant::String);
    if (vArgName.isValid())
    {
        m_argStrPeerName = vArgName.toString();
    }
    else
    {
        m_argStrPeerName = QString::null;
    }

    QVariant vSDPRecord = s->property(p + "sdpRecord",QVariant::String);
    if (vSDPRecord.isValid()) {
        QString recordFilename = KStandardDirs().findResource(
                                     "services", vSDPRecord.toString());
        QFile xmlFile(recordFilename);
        xmlFile.open(IO_ReadOnly);
        QString line;
        m_strSdpRecord = QString();
        while (xmlFile.readLine(line, 2048) > 0) {
            m_strSdpRecord.append(line);
        }
        m_registerService = true;
    }
    else {
        m_strSdpRecord = QString::null;
    }

    QVariant vSuggestedClassMask = s->property(p + "suggestedClassMask",QVariant::Int);
    if (vSuggestedClassMask.isValid())
    {
        m_suggestedClassMask = vSuggestedClassMask.toInt();
    }
    else
    {
        m_suggestedClassMask = 0x000000;
    }

    QVariant vSuggestedClass = s->property(p + "suggestedClass",QVariant::Int);
    if (vSuggestedClass.isValid())
    {
        m_suggestedClass = vSuggestedClass.toInt();
    }
    else
    {
        m_suggestedClass = 0x000000;
    }
    
    sDescription = s->comment();
    
    QVariant vDocPath = s->property("DocPath",QVariant::String);
    if (vDocPath.isValid()) {
        sDocPath = vDocPath.toString();
        kdDebug() << "DocPath: " << sDocPath << endl;
    }
    else {
        sDocPath = QString::null;
        kdDebug() << "DocPath: <none>" << endl;
    }

    QVariant vConfigPara = s->property(p + "arg-config",QVariant::String);
    if (vConfigPara.isValid())
    {
        sConfigParam = vConfigPara.toString();
    }
    else
    {
        sConfigParam = QString::null;
    }
}

void RfcommPortListener::slotIncomingRfcommConnection(int socket)
{
    kdDebug() << " INCOMMING CONNECT!!!!! " << endl;
    struct sockaddr_rc remoteAddr;
    socklen_t opt = sizeof(remoteAddr);
    int newSocket = ::accept(socket, (struct sockaddr *)&remoteAddr, &opt);
    if (newSocket >= 0)
    {
        KProcessInheritSocket process(newSocket);
        
        DeviceAddress deviceaddress(remoteAddr.rc_bdaddr);
        bool authenticationSuccess = true;
        if (false /*bAuthenticate*/) {
            authenticationSuccess = authenticateConnection(deviceaddress);
        }
        
        QString devicename = m_adapter->getRemoteName(QString(deviceaddress));
        if (devicename == "") {
            devicename = QString(deviceaddress);
       	}
        kdDebug() << QString("Accepted connection from %1")
            .arg(QString(deviceaddress)) << endl;

        KNotifyClient::event(
//#if (QT_VERSION >= 0x030200)

//#endif
        KApplication::kApplication()->mainWidget()->winId(),
            "IncomingConnection",
            i18n("Connection from <b>%1</b><br/>to channel %3 (<b>%2</b>)")
            .arg(devicename).arg(this->name()).arg(m_rfcommPort));
        kdDebug() << "WINID" << KApplication::kApplication()->mainWidget()->winId() << endl;


//      NameRequest nameRequest;
	QString peerName = devicename;	
/*
        QString peerName = nameRequest.resolve(
            DeviceAddress(QString(deviceaddress)));
        if (peerName == QString::null)
        {
            kdDebug() << "Could not resolve address: "
            << nameRequest.lastErrorMessage() << endl;
            peerName = QString(deviceaddress);
        }
        else
        {
            kdDebug() << "Resolved name: " << peerName << endl;
        }
*/
        bool askResult = confirmation->askForPermission(
            peerName, deviceaddress, name());
        if (!m_enabled || !askResult)
        {
            ::close(newSocket);
            return;
        }

        process.clearArguments();
        process << m_execPath << m_argStrRfcommChannel
        << QString::number(newSocket);
        if (m_argStrPeerAddr != QString::null)
        {
            process << m_argStrPeerAddr << QString(deviceaddress);
        }
/*      
	  if (m_argStrPeerName != QString::null)
        {
            process << m_argStrPeerName << peerName;
        }
*/
        kdDebug() << QString("Calling \"%1 (channel=%2, socket=%3)\".")
        .arg(m_execPath).arg(m_rfcommPort).arg(newSocket) << endl;
        if (!process.start(KProcess::DontCare))
        {

#if (QT_VERSION >= 0x030200)
#endif
            KNotifyClient::event(
                KApplication::kApplication()->mainWidget()->winId(),
                "ProcessFailed",
                i18n("Call \"%1\" failed").arg(m_execPath));

            kdWarning() << QString("Call to \"%1 (channel=%2)\" failed.")
            .arg(m_execPath).arg(newSocket) << endl;
        }
        ::close(newSocket);
    }
    else
    {
        kdDebug() << "Kbluetoothd: Could not accept connection" << endl;
    }
}

bool RfcommPortListener::isValid()
{
    return m_valid;
}

bool RfcommPortListener::isEnabled()
{
    return m_enabled && m_valid;
}

QStringList RfcommPortListener::resourceTypes()
{
    QStringList ret;
    if (isEnabled()) {
        ret.append("RfcommChannel");
    }
    return ret;
}

QString RfcommPortListener::resource(QString resourceType)
{
    if (resourceType == "RfcommChannel" &&
        isEnabled()) {
        return QString::number(m_rfcommPort);
    }
    else return QString::null;
}



void RfcommPortListener::setEnabled(bool e)
{
    setEnabledInternal(e, QDateTime());
}

void RfcommPortListener::setEnabledInternal(bool e, const QDateTime &ex)
{
    kdDebug() << "RfcommPortListener::setEnabledInternal " << e << endl;
    m_config->setGroup("ListenerConfig");
    m_config->writeEntry("enabled_" + m_serviceName, e);
    m_config->writeEntry("enabled_expiration_"+m_serviceName, ex);
    m_config->sync();

    if (e)
    {
        m_enabled = true;
        if (m_rfcommPort < 0)
            acquirePort();
        if (m_rfcommPort < 0) {
            kdDebug() << "Failed to acquire port. Setting to disabled again." << endl;
            m_enabled = false;
        }
    }
    else
    {
        freePort();
        m_enabled = false;
    }
}


bool RfcommPortListener::isServiceRegistrationEnabled()
{
    return m_registerService;
}

void RfcommPortListener::setServiceRegistrationEnabled(bool e)
{
    kdDebug() << "Kbluetoothd: setServiceRegistrationEnabled()" << endl;
    setServiceRegistrationEnabledInternal(e);
    m_config->setGroup("ListenerConfig");
    m_config->writeEntry("enable_srvreg_" + m_serviceName, e);
    m_config->sync();
}

void RfcommPortListener::setServiceRegistrationEnabledInternal(bool e)
{


    kdDebug() << "setServiceRegisterationEnabledInternal("<<e<<")"<< endl;
    m_registerService = e;

    if (m_strSdpRecord.isNull()) {
        kdDebug() << "service was not enabled. strSdpRecord not set" << endl;   
        return;
    }

    if (m_serviceRegistered == (m_enabled && e)) {
        kdDebug() << "service was not enabled. m_serviceRegistered=" <<
            m_serviceRegistered << " m_enabled=" << m_enabled << 
            " e=" << e << endl;   
        return;
    }

    if (m_sdpRecord)
    {
        delete m_sdpRecord;
        m_sdpRecord = NULL;
    }
    if (m_enabled && e)
    {
        checkClassOfDevice();
        
        m_serviceRegistered = true;
        m_sdpRecord = new Bluetooth::SdpRecord();
        QMap<QString,QString> valrefMap;
        valrefMap["rfcommchannel"] = QString::number(m_rfcommPort, 10);
        //kdDebug() << "fromXmlDocument" << endl;
        QDomDocument xmlDoc;
        xmlDoc.setContent(m_strSdpRecord);
        m_sdpRecord->fromXmlDocument(xmlDoc, valrefMap);
        m_sdpRecord->sdpRegister();
        //kdDebug() << "sdpRegister done." << endl;
        kdDebug() << "Registered with SDP."<< endl;
    }
    else
    {
        kdDebug() << "Unregistered from SDP."<< endl;
        m_serviceRegistered = false;
    }
    //kdDebug() << "setServiceRegisterationEnabledInternal() done"<< endl;

}

void RfcommPortListener::refreshRegistration()
{
    if (m_serviceRegistered)
    {
        setServiceRegistrationEnabledInternal(false);
        setServiceRegistrationEnabledInternal(true);
    }
}

QDateTime RfcommPortListener::expiration()
{
    //return m_expirationTime;
    return QDateTime();
}

QDateTime RfcommPortListener::serviceLifetimeEnd()
{
    return QDateTime();
}

QString RfcommPortListener::name()
{
    return m_serviceName;
}

QString RfcommPortListener::docPath()
{
    return sDocPath;
}

QString RfcommPortListener::description()
{
    return sDescription;
}

RfcommPortListener::~RfcommPortListener()
{
    kdDebug() << "RfcommPortListener::~RfcommPortListener()"<< endl;
    freePort();
    setServiceRegistrationEnabledInternal(false);
}

void RfcommPortListener::configure()
{
    KProcessInheritSocket process(0);
    process.clearArguments();
    process << m_execPath << sConfigParam;
    if (!process.start(KProcess::DontCare))
    {
        kdWarning() << QString("Call to '%1 %2' failed.")
        .arg(m_execPath).arg(sConfigParam) << endl;
    }
}

bool RfcommPortListener::canConfigure()
{
    return sConfigParam != QString::null;
}

void RfcommPortListener::checkClassOfDevice() {
/*
    Adapters adapters;
    if (adapters.count() == 0) return;
    Adapter adapter = adapters[HciDefault::defaultHciDeviceNum()];
    int oldCls = adapter.getClassOfDevice();
    int newCls = (oldCls & ~m_suggestedClassMask) | (m_suggestedClass & m_suggestedClassMask);
    
    if (oldCls != newCls && m_showedClassWarning == false) {
        m_showedClassWarning = true;
        KMessageBox::information(KApplication::kApplication()->mainWidget(),
QString("<p>To use the <b>%1</b> service, some other devices might require a modified \
class number for your bluetooth adapter in /etc/bluetooth/hcid.conf. </p>\
<p>Currently the class is set to <b>0x%2</b>. We suggest you change this to something \
like <b>0x%3</b> instead and restart BlueZ's hcid. The service will be activated anyway.</p>")
.arg(m_serviceName).arg(oldCls, 0, 16).arg(newCls, 0, 16),
            "Possible Device/Service class mismatch",
            QString("classOfDeviceMismatch-%1-%2-%3")
                .arg(oldCls,0,16).arg(newCls,0,16).arg(m_serviceName));
    }
*/
}

void RfcommPortListener::setEncryption(bool enable)
{
    if (m_encrypt != enable) {
        m_encrypt = enable;
        m_config->setGroup("ListenerConfig");
        m_config->writeEntry("encrypt_" + m_serviceName, enable);
        m_config->sync();
        if (m_enabled) {
            setEnabled(false);
            setEnabled(true);
        }
    }
}

void RfcommPortListener::setAuthentication(bool enable)
{
    if (m_authenticate != enable) {
        m_authenticate = enable;
        m_config->setGroup("ListenerConfig");
        m_config->writeEntry("authenticate_" + m_serviceName, enable);
        m_config->sync();
        if (m_enabled) {
            setEnabled(false);
            setEnabled(true);
        }
    }
}

bool RfcommPortListener::getAuthentication()
{
    return m_authenticate;
}

bool RfcommPortListener::getEncryption()
{
    return m_encrypt;
}

#include "rfcommportlistener.moc"
