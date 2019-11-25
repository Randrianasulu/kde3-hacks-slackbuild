/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-16
 * Description : Camera interface dialog
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

#ifndef CAMERAUI_H
#define CAMERAUI_H

// Qt includes.

#include <qdatetime.h>
#include <qstring.h>
#include <qimage.h>

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

// Local includes.

#include "gpiteminfo.h"

namespace Digikam
{

class Album;
class CameraIconViewItem;
class CameraUIPriv;

class CameraUI : public KDialogBase
{
    Q_OBJECT

public:

    CameraUI(QWidget* parent, const QString& cameraTitle,
             const QString& model, const QString& port,
             const QString& path, const QDateTime lastAccess);
    ~CameraUI();

    bool isBusy() const;
    bool isClosed() const;

    bool autoRotateJpegFiles() const;

    /** Get status of JPEG conversion files to lossless format during download.*/
    bool convertLosslessJpegFiles() const;
    QString losslessFormat();

    QString cameraTitle() const;

signals:

    void signalLastDestination(const KURL&);
    void signalAlbumSettingsChanged();

public slots:

    void slotDownload(bool onlySelected, bool deleteAfter, Album *album=0);

protected:

    void closeEvent(QCloseEvent* e);
    void keyPressEvent(QKeyEvent *e);

private:

    void readSettings();
    void saveSettings();
    bool dialogClosed();
    bool createAutoAlbum(const KURL& parentURL, const QString& sub,
                         const QDate& date, QString& errMsg);
    void addFileExtension(const QString& ext);
    void finishDialog();
    void deleteItems(bool onlySelected, bool onlyDownloaded);
    void checkItem4Deletion(CameraIconViewItem* iconItem, QStringList& folders, QStringList& files,
                            QStringList& deleteList, QStringList& lockedList);

private slots:

    void slotClose();
    void slotCancelButton();
    void slotProcessURL(const QString& url);

    void slotConnected(bool val);
    void slotBusy(bool val);
    void slotErrorMsg(const QString& msg);
    void slotInformations();
    void slotCameraInformations(const QString&, const QString&, const QString&);

    void slotFolderList(const QStringList& folderList);
    void slotFileList(const GPItemInfoList& fileList);
    void slotThumbnail(const QString&, const QString&, const QImage&);

    void slotIncreaseThumbSize();
    void slotDecreaseThumbSize();

    void slotUpload();
    void slotUploadItems(const KURL::List&);
    void slotDownloadSelected();
    void slotDownloadAll();
    void slotDeleteSelected();
    void slotDownloadAndDeleteSelected();
    void slotDeleteAll();
    void slotDownloadAndDeleteAll();
    void slotToggleLock();

    void slotFileView(CameraIconViewItem* item);

    void slotUploaded(const GPItemInfo&);
    void slotDownloaded(const QString&, const QString&, int);
    void slotSkipped(const QString&, const QString&);
    void slotDeleted(const QString&, const QString&, bool);
    void slotLocked(const QString&, const QString&, bool);

    void slotNewSelection(bool);
    void slotItemsSelected(CameraIconViewItem* item, bool selected);

    void slotExifFromFile(const QString& folder, const QString& file);
    void slotExifFromData(const QByteArray& exifData);

    void slotFirstItem(void);
    void slotPrevItem(void);
    void slotNextItem(void);
    void slotLastItem(void);

private:

    CameraUIPriv* d;
};

}  // namespace Digikam

#endif /* CAMERAUI_H */
