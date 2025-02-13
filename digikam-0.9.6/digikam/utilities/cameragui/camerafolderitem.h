/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-23
 * Description : A widget to display a camera folder.
 * 
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef CAMERAFOLDERITEM_H
#define CAMERAFOLDERITEM_H

// Qt includes.

#include <qstring.h>
#include <qlistview.h>

// KDE includes.

#include <kiconloader.h>

namespace Digikam
{

class CameraFolderItemPriv;

class CameraFolderItem : public QListViewItem
{

public:

    CameraFolderItem(QListView* parent, const QString& name, 
                     const QPixmap& pixmap=SmallIcon("folder"));

    CameraFolderItem(QListViewItem* parent, const QString& folderName, const QString& folderPath, 
                     const QPixmap& pixmap=SmallIcon("folder"));

    ~CameraFolderItem();

    QString folderName();
    QString folderPath();
    bool    isVirtualFolder();
    void    changeCount(int val);
    void    setCount(int val);
    int     count();

private:

    CameraFolderItemPriv* d;
};

} // namespace Digikam

#endif /* CAMERAFOLDERITEM_H */
