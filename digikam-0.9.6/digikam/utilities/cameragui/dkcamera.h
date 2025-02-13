/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-21
 * Description : abstract camera interface class
 * 
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com> 
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

#ifndef DKCAMERA_H
#define DKCAMERA_H

// Qt includes.

#include <qstring.h>

// Local includes.

#include "gpiteminfo.h"

class QStringList;
class QImage;

namespace Digikam
{

class DKCamera
{
public:

    DKCamera(const QString& title, const QString& model, const QString& port, const QString& path);
    virtual ~DKCamera();

    virtual bool doConnect() = 0;
    virtual void cancel() = 0;

    virtual void getAllFolders(const QString& folder, QStringList& subFolderList) = 0;

    /// If getImageDimensions is false, the camera shall set width and height to -1
    /// if the values are not immediately available
    virtual bool getItemsInfoList(const QString& folder, GPItemInfoList& infoList, bool getImageDimensions = true) = 0;

    virtual bool getThumbnail(const QString& folder, const QString& itemName, QImage& thumbnail) = 0;
    virtual bool getExif(const QString& folder, const QString& itemName, char **edata, int& esize) = 0;

    virtual bool downloadItem(const QString& folder, const QString& itemName, const QString& saveFile) = 0;
    virtual bool deleteItem(const QString& folder, const QString& itemName) = 0;
    virtual bool uploadItem(const QString& folder, const QString& itemName, const QString& localFile,
                            GPItemInfo& itemInfo, bool getImageDimensions=true) = 0;
    virtual bool cameraSummary(QString& summary) = 0;
    virtual bool cameraManual(QString& manual) = 0;
    virtual bool cameraAbout(QString& about) = 0;

    virtual bool setLockItem(const QString& folder, const QString& itemName, bool lock) = 0;

    QString title() const;
    QString model() const;
    QString port()  const;
    QString path()  const;

protected:

    QString mimeType(const QString& fileext) const;

protected:

    QString m_imageFilter;
    QString m_movieFilter;
    QString m_audioFilter;
    QString m_rawFilter;

private:

    QString m_title;
    QString m_model;
    QString m_port;
    QString m_path;
};

}  // namespace Digikam

#endif /* DKCAMERA_H */
