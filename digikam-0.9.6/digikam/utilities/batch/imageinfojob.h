/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-22-01
 * Description : digikamalbum KIO slave interface to get image 
 *               info from database.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEINFOJOB_H
#define IMAGEINFOJOB_H

// Qt includes.

#include <qobject.h>
#include <qcstring.h>

// Local includes.

#include "imageinfo.h"

namespace KIO
{
class Job;
}

namespace Digikam
{

class Album;
class ImageInfoJobPriv;

class ImageInfoJob : public QObject
{
    Q_OBJECT

public:

    ImageInfoJob();
    ~ImageInfoJob();

    void allItemsFromAlbum(Album *album);
    void stop();
    
signals:

    void signalItemsInfo(const ImageInfoList& items);
    void signalCompleted();

private slots:

    void slotResult(KIO::Job* job);
    void slotData(KIO::Job* job, const QByteArray& data);

private:

    ImageInfoJobPriv *d;
};

}  // namespace Digikam

#endif /* IMAGEINFOJOB_H */
