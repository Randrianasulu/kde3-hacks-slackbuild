/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-08
 * Description : simple image properties side bar used by 
 *               camera gui.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
 
#ifndef IMAGEPROPERTIESSIDEBARCAMGUI_H
#define IMAGEPROPERTIESSIDEBARCAMGUI_H

// KDE includes.

#include <kurl.h>

// Local includes.

#include "sidebar.h"
#include "digikam_export.h"

class QSplitter;
class QWidget;

namespace Digikam
{

class GPItemInfo;
class CameraIconView;
class CameraIconViewItem;
class NavigateBarTab;
class ImagePropertiesSideBarCamGuiPriv;

class DIGIKAM_EXPORT ImagePropertiesSideBarCamGui : public Sidebar
{
    Q_OBJECT

public:

    ImagePropertiesSideBarCamGui(QWidget* parent, const char *name, QSplitter *splitter,
                                 Side side=Left, bool mimimizedDefault=false);

    ~ImagePropertiesSideBarCamGui();

    void itemChanged(GPItemInfo* itemInfo, const KURL& url, const QByteArray& exifData=QByteArray(),
                     CameraIconView* view=0, CameraIconViewItem* item=0);

public slots:

    virtual void slotNoCurrentItem(void);

signals:

    void signalFirstItem(void);
    void signalPrevItem(void);
    void signalNextItem(void);
    void signalLastItem(void);

private slots:

    virtual void slotChangedTab(QWidget* tab);

private:

    ImagePropertiesSideBarCamGuiPriv* d;
    void connectNavigateSignals(NavigateBarTab *tab);
};

}  // NameSpace Digikam

#endif  // IMAGEPROPERTIESSIDEBARCAMGUI_H
