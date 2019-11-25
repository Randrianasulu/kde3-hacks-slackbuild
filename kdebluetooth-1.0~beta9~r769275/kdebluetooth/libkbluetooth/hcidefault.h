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

#ifndef HCIDEFAULT_H
#define HCIDEFAULT_H

#include <qobject.h>

/**
@author Fred Schaettgen
*/
namespace KBluetooth
{

class HciDefault : public QObject
{
Q_OBJECT
public:
    static int defaultHciDeviceNum();
    static void addCmdLineOptions(QString option="hcidevice");
    static void setDefaultDevice(int n);
    virtual ~HciDefault();
protected:
    static HciDefault* instance();
    void reInit();
    int defaultDevice;
    QString cmdLineOption;
    HciDefault();
};

}
#endif
