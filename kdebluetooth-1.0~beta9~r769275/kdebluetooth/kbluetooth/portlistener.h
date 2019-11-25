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

#ifndef _PORTLISTENER_H_
#define _PORTLISTENER_H_

#include <qobject.h>
#include <qstringlist.h>

//class KServiceRegistry;
class KConfig;
class QDateTime;

class PortListener : public QObject
{
    Q_OBJECT
public:
    PortListener(QObject* owner) : QObject(owner) { }
    virtual bool acquirePort() = 0;
    virtual bool isValid() = 0;
    virtual QString name() = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual void setServiceRegistrationEnabled(bool enabled) = 0;
    virtual bool isServiceRegistrationEnabled() = 0;
    virtual QDateTime expiration() = 0;
    virtual QDateTime serviceLifetimeEnd() = 0;
    virtual bool isEnabled() = 0;
    virtual void setAuthentication(bool enable) = 0;
    virtual void setEncryption(bool enable) = 0;
    virtual bool getAuthentication() = 0;
    virtual bool getEncryption() = 0;
    virtual QStringList resourceTypes() = 0;
    virtual QString resource(QString resourceType) = 0;
    virtual QString docPath() = 0;
    virtual QString description() = 0;
    virtual void configure() = 0;
    virtual bool canConfigure() = 0;

    //virtual QStringList processServiceTemplate(const QString &a) = 0;
    //virtual bool setPort(int port = -1, int autoProbeRange = 1) = 0;
    virtual void refreshRegistration() = 0;
};

#endif
