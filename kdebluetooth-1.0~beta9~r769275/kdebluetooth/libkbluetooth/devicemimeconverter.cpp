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

#include "devicemimeconverter.h"
#include <kglobal.h>
#include <kmimetype.h>
#include <kdebug.h>

namespace KBluetooth {


DeviceClassMimeConverter::DeviceClassMimeConverter()
{
    getIconName("bluetooth/unknown-device-class");
    getIconName("bluetooth/misc-device-class");
    getIconName("bluetooth/misc-device-class");
    getIconName("bluetooth/computer-device-class");
    getIconName("bluetooth/phone-device-class");
    getIconName("bluetooth/lan-device-class");
    getIconName("bluetooth/av-device-class");
    getIconName("bluetooth/peripheral-device-class");
    getIconName("bluetooth/mouse-device-class");
    getIconName("bluetooth/keyboard-device-class");
    getIconName("bluetooth/imaging-device-class");
    getIconName("bluetooth/unknown-device-class");
}

void DeviceClassMimeConverter::getIconName(QString mime)
{
    QString iconName = KMimeType::mimeType(mime)->icon(QString::null, false);
    mimeTypeToIconMap[mime] = iconName;
}

DeviceClassMimeConverter* DeviceClassMimeConverter::getInstance()
{
    static DeviceClassMimeConverter instance;
    return &instance;
}

QString DeviceClassMimeConverter::classToIconName(int n)
{
    return DeviceClassMimeConverter::mimeTypeToIcon(
        DeviceClassMimeConverter::classToMimeType(n));
}



QString DeviceClassMimeConverter::classToMimeType(int n)
{
    QString mimeType = "bluetooth/unknown-device-class";
    int major = ((n & 0x001F00) >> 8);
    int minor = ((n >> 2) & 0x30);
    kdDebug() << "MAJOR" << QString::number(major,16) << endl;
    kdDebug() << "MINOR" << QString::number(minor,16) << endl;
    switch (major) {
    case 0x00:
        mimeType = "bluetooth/misc-device-class";
        break;
    case 0x01:
        mimeType = "bluetooth/computer-device-class";
        break;
    case 0x02:
        mimeType = "bluetooth/phone-device-class";
        break;
    case 0x03:
        mimeType = "bluetooth/lan-device-class";
        break;
    case 0x04:
        mimeType = "bluetooth/av-device-class";
        break;
    case 0x05:

    	kdDebug() << "MINOR_Periphery" << QString::number(minor,16) << endl;
	switch (minor) {
		case 0x10:
		mimeType = "bluetooth/keyboard-device-class";
		break;
		case 0x20:
		mimeType = "bluetooth/mouse-device-class";
		break;
		default:
        	mimeType = "bluetooth/peripheral-device-class";
	}


        break;
    case 0x06:
        mimeType = "bluetooth/imaging-device-class";
        break;
    case 0x07:
        mimeType = "bluetooth/unknown-device-class";
        break;
    }
    return mimeType;
}

QString DeviceClassMimeConverter::mimeTypeToIcon(QString mime)
{
    DeviceClassMimeConverter* c = DeviceClassMimeConverter::getInstance();
    if (c->mimeTypeToIconMap.find(mime) != c->mimeTypeToIconMap.end()) {
        return c->mimeTypeToIconMap[mime];
    }
    else {
        return c->mimeTypeToIconMap["bluetooth/unknown-device-class"];
    }
}



}
