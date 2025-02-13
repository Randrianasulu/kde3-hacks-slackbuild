/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : a tread-safe dcraw program interface
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2008 by Guillaume Castagnino <casta at xwing dot info>
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

#ifndef KDCRAW_H
#define KDCRAW_H

// Qt includes.

#include <qstring.h>
#include <qobject.h>
#include <qimage.h>

// KDE includes.

#include <kdemacros.h>

// Local includes.

#include "libkdcraw_export.h"
#include "rawdecodingsettings.h"
#include "dcrawinfocontainer.h"

namespace KDcrawIface
{

class KDcrawPriv;

class LIBKDCRAW_EXPORT KDcraw : public QObject
{
    Q_OBJECT

public:

    /** Standard constructor. */
    KDcraw();

    /** Standard destructor. */
    virtual ~KDcraw();

public:

    /** Return a string version of libkdcraw release */
    static QString version();

    /** This is a non cancelable method witch do not require a class instance to run. 
        It can loadEmbeddedPreview() in first and if it failed, call loadHalfPreview(). 
    */
    static bool loadDcrawPreview(QImage& image, const QString& path);

    /** Get the embedded JPEG preview image from RAW picture as a QByteArray witch will include Exif Data. 
        This is a fast and non cancelable. This method do not require a class instance to run.
    */
    static bool loadEmbeddedPreview(QByteArray& imgData, const QString& path);

    /** Get the embedded JPEG preview image from RAW picture has a QImage. This is a fast and non cancelable 
        This method do not require a class instance to run.
    */
    static bool loadEmbeddedPreview(QImage& image, const QString& path);

    /** Get the half decode RAW picture. This is a more slower than loadEmbeddedPreview() method
        and non cancelable. This method do not require a class instance to run.
    */
    static bool loadHalfPreview(QImage& image, const QString& path);

    /** Get the camera settings witch have taken RAW file. Look into dcrawinfocontainer.h 
        for more details. This is a fast and non cancelable method witch do not require 
        a class instance to run.
    */ 
    static bool rawFileIdentify(DcrawInfoContainer& identify, const QString& path);

    /** Return the string of all RAW file type mime supported. */
    static const char *rawFiles();

    /** Return the list of all RAW file type mime supported,
        as a QStringList, without wildcard and suffix dot.  */
    static QStringList rawFilesList();

    /** Provide a list of supported RAW Camera name. */ 
    static QStringList supportedCamera();

    /** Return LibRaw version string. */
    static QString librawVersion();

public: 

    /** Extract Raw image data undemosaiced and without post processing from 'filePath' picture file. 
        This is a cancelable method which require a class instance to run because RAW pictures loading 
        can take a while.

        This method return:

            - A byte array container 'rawData' with raw data. 
            - All info about Raw image into 'identify' container. 
            - 'false' is returned if loadding failed, else 'true'.  
    */
    bool extractRAWData(const QString& filePath, QByteArray &rawData, DcrawInfoContainer& identify);

    /** Extract a small size of decode RAW data from 'filePath' picture file using 
        'rawDecodingSettings' settings. This is a cancelable method which require 
        a class instance to run because RAW pictures decoding can take a while.

        This method return:

            - A byte array container 'imageData' with picture data. Pixels order is RGB. 
              Color depth can be 8 or 16. In 8 bits you can access to color component 
              using (uchar*), in 16 bits using (ushort*).

            - Size size of image in number of pixels ('width' and 'height').
            - The max average of RGB components from decoded picture.
            - 'false' is returned if decoding failed, else 'true'.  
    */
    bool decodeHalfRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings, 
                            QByteArray &imageData, int &width, int &height, int &rgbmax);

    /** Extract a full size of RAW data from 'filePath' picture file using 
        'rawDecodingSettings' settings. This is a cancelable method which require 
        a class instance to run because RAW pictures decoding can take a while.

        This method return:

            - A byte array container 'imageData' with picture data. Pixels order is RGB. 
              Color depth can be 8 or 16. In 8 bits you can access to color component 
              using (uchar*), in 16 bits using (ushort*).

            - Size size of image in number of pixels ('width' and 'height').
            - The max average of RGB components from decoded picture. 
            - 'false' is returned if decoding failed, else 'true'.  
    */
    bool decodeRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings, 
                        QByteArray &imageData, int &width, int &height, int &rgbmax);

    /** To cancel 'decodeHalfRAWImage' and 'decodeRAWImage' methods running 
        in a separate thread.
    */
    void cancel();

protected:

    /** Used internally to cancel RAW decoding operation. Normally, you don't need to use it 
        directly, excepted if you derivated this class. Usual way is to use cancel() method 
    */
    bool                m_cancel;

    /** The settings container used to perform RAW pictures decoding. See 'rawdecodingsetting.h' 
        for details.
    */
    RawDecodingSettings m_rawDecodingSettings;

protected:

    /** Re-implement this method to control the cancelisation of loop witch wait data 
        from RAW decoding process with your propers envirronement. 
        By default, this method check if m_cancel is true.
    */
    virtual bool checkToCancelWaitingData();

    /** This method is obsolete and will be removed in the future.
    */
    virtual bool checkToCancelReceivingData() KDE_DEPRECATED;

    /** Re-implement this method to control the pseudo progress value during RAW decoding (when dcraw run with an
        internal loop without feedback) with your proper environment. By default, this method do nothing.
        Progress value average for this stage is 0%-n%, with 'n' == 40% max (see setWaitingDataProgress() method).
    */
    virtual void setWaitingDataProgress(double value);

    /** This method is obsolete and will be removed in the future.
    */
    virtual void setReceivingDataProgress(double value) KDE_DEPRECATED;

private:

    bool loadFromDcraw(const QString& filePath, QByteArray &imageData,
                       int &width, int &height, int &rgbmax);

private:

    KDcrawPriv *d;

    friend class KDcrawPriv;
};

}  // namespace KDcrawIface

#endif /* KDCRAW_H */
