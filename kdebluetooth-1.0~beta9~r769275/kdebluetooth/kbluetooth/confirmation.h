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

#ifndef CONFIRMATION_H
#define CONFIRMATION_H

#include <libkbluetooth/deviceaddress.h>
#include <qstring.h>
#include <qstringlist.h>

class KConfig;

/**
@author Fred Schaettgen
*/
class Confirmation {
public:
    Confirmation(KConfig* c,QString configGroup = "AccessPermissions");
    struct ConnectionRule {
        KBluetooth::DeviceAddress addr;
        QString service;
        QString policy;
    };
    bool askForPermission(QString devicename,
        KBluetooth::DeviceAddress deviceaddress, QString servicename);
    
    int getNumRules();
    QStringList getPolicies();    
    ConnectionRule getRule(int n);
    bool insertRule(int n, const ConnectionRule& rule);
    bool deleteRule(int n);
    
private: 
    void saveBookmark(const QString &url, const QString& name);
    QString getPolicy(KBluetooth::DeviceAddress addr, QString service);
    QString configGroup;
    QStringList policies;
    QString defaultPolicy;
    KConfig* config;
};

#endif
