/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-14
 * Description : batch image rotation
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

#define XMD_H

// C++ includes.

#include <cstdio>

// C Ansi includes.

extern "C" 
{
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
}

// Qt includes.

#include <qimage.h>
#include <qwmatrix.h>
#include <qfile.h>
#include <qfileinfo.h>

// KDE includes.

#include <kprocess.h>
#include <klocale.h>
#include <kurl.h>
#include <ktempfile.h>

// Local includes.

#include "utils.h"
#include "transupp.h"
#include "jpegtransform.h"
#include "imagerotate.h"
#include "imagerotate.moc"

namespace KIPIJPEGLossLessPlugin
{

ImageRotate::ImageRotate()
           : QObject()
{
    m_tmpFile = new KTempFile(QString(), QString("kipiplugin-rotate"));
    m_tmpFile->setAutoDelete(true);
}

ImageRotate::~ImageRotate()
{
    delete m_tmpFile;
}

bool ImageRotate::rotate(const QString& src, RotateAction angle, QString& err)
{
    QFileInfo fi(src);

    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    if ( !m_tmpFile->file() )
    {
        err = i18n("Error in opening temporary file");
        return false;
    }

    QString tmp = m_tmpFile->name();

    if (Utils::isRAW(src))
    {
        err = i18n("Cannot rotate RAW file");
        return false;
    }
    else if (Utils::isJPEG(src))
    {
        if (!rotateJPEG(src, tmp, angle, err)) {
            if (err == "nothing to do") { err=QString::null; return true; }
            return false;
        }
    }
    else
    {
        // B.K.O #123499 : we using Image Magick API here instead QT API 
        // else TIFF/PNG 16 bits image are broken!
        if (!rotateImageMagick(src, tmp, angle, err))
            return false;

        // We update metadata on new image.
        Utils tools(this);
        if (!tools.updateMetadataImageMagick(tmp, err))
            return false;
    }

    // Move back to original file
    if (!Utils::MoveFile(tmp, src)) 
    {
        err = i18n("Cannot update source image");
        return false;
    }

    return true;
}

bool ImageRotate::rotateJPEG(const QString& src, const QString& dest, RotateAction angle, QString& err)
{
    Matrix transform=Matrix::none;

    switch(angle)
    {
        case (Rot90):
        {
            transform = Matrix::rotate90;
            break;
        }
        case (Rot180):
        {
            transform = Matrix::rotate180;
            break;
        }
        case (Rot270):
        {
            transform = Matrix::rotate270;
            break;
        }
        case (Rot0):
        {
            transform = Matrix::none;
            break;
        }
        default:
        {
            qDebug("ImageRotate: Nonstandard rotation angle");
            err = i18n("Nonstandard rotation angle");
            return false;
        }
    }

    return transformJPEG(src, dest, transform, err);
}

bool ImageRotate::rotateImageMagick(const QString& src, const QString& dest, 
                                    RotateAction angle, QString& err)
{
    KProcess process;
    process.clearArguments();
    process << "convert";
    process << "-verbose";
    process << "-rotate";

    switch(angle)
    {
        case (Rot90):
        {
            process << "90";
            break;
        }
        case (Rot180):
        {
            process << "180";
            break;
        }
        case (Rot270):
        {
            process << "270";
            break;
        }
        case (Rot0):
        {
            break;
        }
        default:
        {
            qDebug("ImageRotate: Nonstandard rotation angle");
            err = i18n("Nonstandard rotation angle");
            return false;
        }
    }

    process << src + QString("[0]") << dest;

    qDebug("ImageMagick Command line args:");
    QValueList<QCString> args = process.args();
    for (QValueList<QCString>::iterator it = args.begin(); it != args.end(); ++it)
        qDebug("%s", (const char*)(*it));

    connect(&process, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotReadStderr(KProcess*, char*, int)));

    if (!process.start(KProcess::Block, KProcess::Stderr))
        return false;

    if (!process.normalExit())
        return false;

    switch (process.exitStatus())
    {
        case 0:  // Process finished successfully !
        {
            return true;
            break;
        }
        case 15: //  process aborted !
        {
            return false;
            break;
        }
    }

    // Processing error !
    err = i18n("Cannot rotate: %1").arg(m_stdErr.replace('\n', ' '));
    return false;
}

void ImageRotate::slotReadStderr(KProcess*, char* buffer, int buflen)
{
    m_stdErr.append(QString::fromLocal8Bit(buffer, buflen));
}

}  // NameSpace KIPIJPEGLossLessPlugin
