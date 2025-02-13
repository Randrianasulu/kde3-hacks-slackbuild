/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-05-02
 * Description : RAW file identification information container 
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DCRAW_INFO_CONTAINER_H
#define DCRAW_INFO_CONTAINER_H

// Qt includes.

#include <qstring.h>
#include <qdatetime.h>
#include <qsize.h>

// Local includes.

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT DcrawInfoContainer
{

public:

    /** The RAW image orientation values
     */
    enum ImageOrientation
    {
        ORIENTATION_NONE  = 0,
        ORIENTATION_180   = 3,
        ORIENTATION_90CCW = 5,
        ORIENTATION_90CW  = 6
    };

public:

    /** Standard constructor */
    DcrawInfoContainer()
    {
        sensitivity       = -1.0;
        exposureTime      = -1.0;
        aperture          = -1.0;
        focalLength       = -1.0;
        pixelAspectRatio  = 1.0;    // Default value. This can be unavailable (depending of camera model).
        rawColors         = -1;
        rawImages         = -1;
        hasIccProfile     = false;
        isDecodable       = false;
        hasSecondaryPixel = false;
        blackPoint        = 0;
        whitePoint        = 0;
        daylightMult[0]   = 0.0;
        daylightMult[1]   = 0.0;
        daylightMult[2]   = 0.0;
        cameraMult[0]     = 0.0;
        cameraMult[1]     = 0.0;
        cameraMult[2]     = 0.0;
        cameraMult[3]     = 0.0;
        orientation       = ORIENTATION_NONE;
    };

    /** Standard destructor */
    virtual ~DcrawInfoContainer(){}

    /** return 'true' if container is empty, else 'false' */
    bool isEmpty()
    {
        if ( make.isEmpty()          &&
             model.isEmpty()         &&
             filterPattern.isEmpty() &&
             DNGVersion.isEmpty()    &&
             exposureTime == -1.0    && 
             aperture     == -1.0    && 
             focalLength  == -1.0    && 
             pixelAspectRatio == 1.0 &&
             sensitivity  == -1.0    && 
             rawColors    == -1      &&
             rawImages    == -1      &&
             blackPoint   == 0       &&
             whitePoint   == 0       &&
             !dateTime.isValid()     && 
             !imageSize.isValid()    &&
             !fullSize.isValid()     &&
             !outputSize.isValid()   &&
             !thumbSize.isValid()    &&
             orientation == ORIENTATION_NONE
           )
            return true;
        else
            return false;
    };

    /** True if camera sensor use a secondary pixel. */
    bool             hasSecondaryPixel;
    /** True if RAW file include an ICC color profile. */
    bool             hasIccProfile;
    /** True is RAW file is decodable by dcraw. */
    bool             isDecodable;

    /** The number of RAW colors. */
    int              rawColors;

    /** The number of RAW images. */
    int              rawImages;

    /** Black level from Raw histogram. */
    unsigned int     blackPoint;

    /** White level from Raw histogram. */
    unsigned int     whitePoint;

    /** The raw image orientation */
    ImageOrientation orientation;

    /** The sensitivity in ISO used by camera to take the picture. */
    float            sensitivity;

    /** ==> 1/exposureTime = exposure time in seconds. */
    float            exposureTime;
    /** ==> Aperture value in APEX. */
    float            aperture;
    /** ==> Focal Length value in mm. */
    float            focalLength;
    /** The pixel Aspect Ratio if != 1.0. NOTE: if == 1.0, dcraw do not show this value. */
    float            pixelAspectRatio;

    /** White color balance settings. */
    double    daylightMult[3];
    /** Camera multipliers used for White Balance adjustements */
    double           cameraMult[4];

    /** The camera maker. */
    QString          make;
    /** The camera model. */
    QString          model;
    /** The artist name who have picture owner. */
    QString          owner;
    /** The demosaising filter pattern. */
    QString          filterPattern;
    /** The DNG version. NOTE: its only show with DNG RAW files. */
    QString          DNGVersion;

    /** Date & time when have been taken the picture. */
    QDateTime dateTime;


    /** The image dimensions in pixels. */
    QSize            imageSize;

    /** The thumb dimensions in pixels. */
    QSize            thumbSize;

    /** The full RAW image dimensions in pixels. */
    QSize            fullSize;

    /** The output dimensions in pixels. */
    QSize            outputSize;
};

} // namespace KDcrawIface

#endif /* DCRAW_INFO_CONTAINER_H */
