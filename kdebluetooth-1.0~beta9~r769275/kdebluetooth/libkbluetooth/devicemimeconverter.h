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

#ifndef KBLUETOOTHDEVICEMIMECONVERTER_H
#define KBLUETOOTHDEVICEMIMECONVERTER_H

#include <qstring.h>
#include <map>

namespace KBluetooth {

/**
@author Fred Schaettgen
*/
class DeviceClassMimeConverter
{
public:
    static QString classToMimeType(int deviceClass);
    static QString mimeTypeToIcon(QString mimeType);
    static QString classToIconName(int deviceClass);
protected:
    DeviceClassMimeConverter();
    static DeviceClassMimeConverter *getInstance();
private:
    std::map<QString, QString> mimeTypeToIconMap;
    void getIconName(QString mimetype);
};

}

#endif
