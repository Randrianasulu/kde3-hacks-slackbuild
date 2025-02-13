/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-06-17
 * Description : A TIFF IO file for DImg framework
 * 
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef TIFFLOADER_H
#define TIFFLOADER_H

// C ansi includes.

extern "C" 
{
#include <tiffio.h>
#include <tiff.h>
}

// Local includes.

#include "dimgloader.h"
#include "digikam_export.h"

namespace Digikam
{

class DImg;
class DMetadata;

class DIGIKAM_EXPORT TIFFLoader : public DImgLoader
{
public:

    TIFFLoader(DImg* image);

    bool load(const QString& filePath, DImgLoaderObserver *observer);
    bool save(const QString& filePath, DImgLoaderObserver *observer);

    virtual bool hasAlpha()   const;
    virtual bool sixteenBit() const;
    virtual bool isReadOnly() const { return false; };

private:

    void tiffSetExifAsciiTag(TIFF* tif, ttag_t tiffTag, const DMetadata *metaData, const char* exifTagName);
    void tiffSetExifDataTag(TIFF* tif, ttag_t tiffTag, const DMetadata *metaData, const char* exifTagName);

    static void dimg_tiff_warning(const char* module, const char* format, va_list warnings);
    static void dimg_tiff_error(const char* module, const char* format, va_list errors);

private:

    bool m_sixteenBit;
    bool m_hasAlpha;
};

}  // NameSpace Digikam

#endif /* TIFFLOADER_H */
