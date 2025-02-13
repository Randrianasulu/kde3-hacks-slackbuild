/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-07-24
 * Description : a dialog to select a camera folders.
 * 
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CAMERAFOLDERDIALOG_H
#define CAMERAFOLDERDIALOG_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

namespace Digikam
{

class CameraIconView;
class CameraFolderView;
class CameraFolderItem;

class CameraFolderDialog : public KDialogBase
{
    Q_OBJECT

public:

    CameraFolderDialog(QWidget *parent, CameraIconView *cameraView, const QStringList& cameraFolderList,
                       const QString& cameraName, const QString& rootPath);
    ~CameraFolderDialog();

    QString selectedFolderPath();

private slots:

    void slotFolderPathSelectionChanged(CameraFolderItem* item);

private:
    
    QString           m_rootPath;

    CameraFolderView *m_folderView;
};

} // namespace Digikam

#endif /* CAMERAFOLDERDIALOG_H */
