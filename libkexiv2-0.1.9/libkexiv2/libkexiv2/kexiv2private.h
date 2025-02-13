/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-09-03
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KEXIV2_PRIVATE_H
#define KEXIV2_PRIVATE_H

 // C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>

// Qt includes.

#include <qfile.h>
#include <qsize.h>
#include <qtextcodec.h>
#include <qwmatrix.h>
#include <qfileinfo.h>

// KDE includes.

#include <ktempfile.h>
#include <kstringhandler.h>
#include <kdeversion.h>

// Exiv2 includes.

// The pragmas are required to be able to catch exceptions thrown by libexiv2:
// See http://gcc.gnu.org/wiki/Visibility, the section about c++ exceptions.
// They are needed for all libexiv2 versions that do not care about visibility.
#pragma GCC visibility push(default)
#include <exiv2/exiv2.hpp>
#pragma GCC visibility pop

// Check if Exiv2 support XMP

#if (EXIV2_MAJOR_VERSION ==0 && EXIV2_MINOR_VERSION ==15 && EXIV2_PATCH_VERSION >=99) || \
    (EXIV2_MAJOR_VERSION ==0 && EXIV2_MINOR_VERSION >15 ) || \
    (EXIV2_MAJOR_VERSION >0)
#   define _XMP_SUPPORT_ 1
#endif

// Make sure an EXIV2_TEST_VERSION macro exists:

#ifdef EXIV2_VERSION
#    ifndef EXIV2_TEST_VERSION
#        define EXIV2_TEST_VERSION(major,minor,patch) \
         ( EXIV2_VERSION >= EXIV2_MAKE_VERSION(major,minor,patch) )
#    endif
#else
#    define EXIV2_TEST_VERSION(major,minor,patch) (false)
#endif

namespace KExiv2Iface
{

class KExiv2Priv
{
public:

    KExiv2Priv();
    ~KExiv2Priv();

    /** Set the Exif data using an Exiv2 byte array. Return true if Exif metadata
        have been changed in memory.
    */
    bool setExif(Exiv2::DataBuf const data);

    /** Set the Iptc data using an Exiv2 byte array. Return true if Iptc metadata
        have been changed in memory.
    */
    bool setIptc(Exiv2::DataBuf const data);

    /** Generic method to print the Exiv2 C++ Exception error message from 'e'.
        'msg' string is printed just before like debug header.
    */
    void printExiv2ExceptionError(const QString& msg, Exiv2::Error& e);

    /** Wrapper method to convert a Comments content to a QString.
    */
    QString convertCommentValue(const Exiv2::Exifdatum &exifDatum);

    /** Charset autodetection to convert a string to a QString.
    */
    QString detectEncodingAndDecode(const std::string &value);

public:

    QString         filePath;

    std::string     imageComments;

    Exiv2::ExifData exifMetadata;

    Exiv2::IptcData iptcMetadata;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_PRIVATE_H */
