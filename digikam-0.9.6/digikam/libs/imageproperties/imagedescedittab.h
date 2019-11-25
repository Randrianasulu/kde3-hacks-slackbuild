/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2003-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef IMAGEDESCEDITTAB_H
#define IMAGEDESCEDITTAB_H

// Qt includes.

#include <qwidget.h>
#include <qpixmap.h>
#include <qptrlist.h>

// Local includes.

#include "digikam_export.h"
#include "navigatebartab.h"
#include "albummanager.h"

class QListViewItem;

namespace Digikam
{
class TAlbumCheckListItem;
class ImageInfo;
class ImageDescEditTabPriv;

class DIGIKAM_EXPORT ImageDescEditTab : public NavigateBarTab
{
    Q_OBJECT

public:

    ImageDescEditTab(QWidget *parent, bool navBar=true);
    ~ImageDescEditTab();

    void assignRating(int rating);
    void setItem(ImageInfo *info=0);
    void setItems(QPtrList<ImageInfo> infos);
    void populateTags();
    void refreshTagsView();

signals:

    void signalProgressBarMode(int, const QString&);
    void signalProgressValue(int);
    void signalTagFilterMatch(bool);

protected:

    bool eventFilter(QObject *o, QEvent *e);

private:

    void setInfos(QPtrList<ImageInfo> infos);

    void updateTagsView();
    void updateComments();
    void updateRating();
    void updateDate();
    void updateRecentTags();

    void tagNew(TAlbum* parAlbum, const QString& _title=QString(), const QString& _icon=QString()) const;
    void tagEdit(TAlbum* album);
    void tagDelete(TAlbum *album);

    void toggleChildTags(TAlbum *album, bool b);
    void toggleParentTags(TAlbum *album, bool b);

    void setTagThumbnail(TAlbum *album);

    bool singleSelection() const;
    void setMetadataWidgetStatus(int status, QWidget *widget);
    void reloadForMetadataChange(Q_LLONG imageId);

private slots:

    void slotApplyAllChanges();
    void slotCreateNewTag();
    void slotRevertAllChanges();
    void slotChangingItems();
    void slotItemStateChanged(TAlbumCheckListItem *);
    void slotCommentChanged();
    void slotDateTimeChanged(const QDateTime& dateTime);
    void slotRatingChanged(int rating);
    void slotModified();
    void slotRightButtonClicked(QListViewItem *, const QPoint &, int);
    void slotTagsSearchChanged(const QString&);

    void slotAlbumAdded(Album* a);
    void slotAlbumDeleted(Album* a);
    void slotAlbumIconChanged(Album* a);
    void slotAlbumRenamed(Album* a);
    void slotAlbumsCleared();
    void slotAlbumMoved(TAlbum* tag, TAlbum* newParent);

    void slotABCContextMenu();
    void slotGotThumbnailFromIcon(Album *album, const QPixmap& thumbnail);
    void slotThumbnailLost(Album *album);
    void slotReloadThumbnails();

    void slotImageTagsChanged(Q_LLONG imageId);
    void slotImagesChanged(int albumId);
    void slotImageRatingChanged(Q_LLONG imageId);
    void slotImageDateChanged(Q_LLONG imageId);
    void slotImageCaptionChanged(Q_LLONG imageId);

    void slotRecentTagsMenuActivated(int);
    void slotAssignedTagsToggled(bool);

    void slotMoreMenu();
    void slotReadFromFileMetadataToDatabase();
    void slotWriteToFileMetadataFromDatabase();

private:

    ImageDescEditTabPriv* d;
};

}  // NameSpace Digikam

#endif  // IMAGEDESCEDITTAB_H
