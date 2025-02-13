/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RENAMEIMAGESWIDGET_H
#define RENAMEIMAGESWIDGET_H

// KDE includes.

#include <kurl.h>

// Local includes.

#include "renameimagesbase.h"

class KFileItem;
class QTimer;
class QProgressDialog;

namespace KIPI
{
class Interface;
}

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class RenameImagesWidget : public RenameImagesBase
{
    Q_OBJECT
    
public:

    enum SortOrder
    {
        BYNAME = 0,
        BYSIZE,
        BYDATE
    }; 

    RenameImagesWidget(QWidget *parent, KIPI::Interface* interface,
                       const KURL::List& urlList);
    ~RenameImagesWidget();

private:

    void readSettings();
    void saveSettings();
    void reListImages();
    void updateListing();
    QString oldToNewName(BatchProcessImagesItem* item,
                         int itemPosition);
    
    KIPI::Interface* m_interface;
    KURL::List       m_urlList;
    QTimer*          m_timer;
    QProgressDialog* m_progress;
    bool             m_overwriteAll;
    bool             m_autoSkip;

public slots:

    void slotStart();
    void slotAbort();
    void slotNext();
    
private slots:

    void slotListViewDoubleClicked(QListViewItem*);
    void slotImageSelected(QListViewItem*);
    void slotOptionsChanged();
    void slotGotPreview(const KFileItem*, const QPixmap&);

    void slotAddImages();
    void slotRemoveImage();

	void sortList(int);
	void reverseList();

	void moveCurrentItemUp();
	void moveCurrentItemDown();
};

}

#endif /* RENAMEIMAGESWIDGET_H */
