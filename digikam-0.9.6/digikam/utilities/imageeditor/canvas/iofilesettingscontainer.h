/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-03
 * Description : IO file Settings Container.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IOFILESETTINGSCONTAINER_H
#define IOFILESETTINGSCONTAINER_H

// Local includes.

#include "drawdecoding.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT IOFileSettingsContainer
{

public:

    IOFileSettingsContainer()
    {
        JPEGCompression     = 75;
        JPEGSubSampling     = 1;    // Medium subsampling
        PNGCompression      = 9;
        TIFFCompression     = false;
        JPEG2000Compression = 75;
        JPEG2000LossLess    = true;
        useRAWImport        = true;
    };

    ~IOFileSettingsContainer(){};

public:

    // JPEG quality value.
    int  JPEGCompression;

    // JPEG chroma subsampling value.
    int  JPEGSubSampling;

    // PNG compression value.
    int  PNGCompression;

    // TIFF deflat compression.
    bool TIFFCompression;

    // JPEG2000 quality value.
    int  JPEG2000Compression;

    // JPEG2000 lossless compression.
    bool JPEG2000LossLess;

    // Use Raw Import tool to load a RAW picture.
    bool useRAWImport;

    // ------------------------------------------------------
    // RAW File decoding options :

    DRawDecoding rawDecodingSettings;
};

}  // namespace Digikam

#endif  // IOFILESETTINGSCONTAINER_H
