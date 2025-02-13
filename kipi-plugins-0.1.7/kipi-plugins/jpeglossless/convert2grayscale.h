/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-14
 * Description : batch images grayscale conversion
 *
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef convert2grayscale_H
#define convert2grayscale_H

// Qt includes.

#include <qobject.h>
#include <qstring.h>

class KTempFile;
class KProcess;

namespace KIPIJPEGLossLessPlugin
{

class ImageGrayScale : public QObject
{
    Q_OBJECT

public:

    ImageGrayScale();
    ~ImageGrayScale();

    bool image2GrayScale(const QString& src, QString& err);

private slots:

    void slotReadStderr(KProcess*, char*, int);

private:

    bool image2GrayScaleJPEG(const QString& src, const QString& dest, QString& err);
    bool image2GrayScaleImageMagick(const QString& src, const QString& dest, QString& err);

private:

    QString    m_stdErr;

    KTempFile *m_tmpFile;
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* convert2grayscale_H */
