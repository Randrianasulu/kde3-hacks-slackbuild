/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-13
 * Description : A widget stack to embedded album content view
 *               or the current image preview.
 *
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

#ifndef ALBUMWIDGETSTACK_H
#define ALBUMWIDGETSTACK_H

// KDE includes.

#include <qwidgetstack.h>
#include <qcstring.h>
#include <qstring.h>

// Local includes.

#include "digikam_export.h"

class KURL::List;

namespace Digikam
{

class ImageInfo;
class AlbumIconView;
class ImagePreviewView;
class AlbumWidgetStackPriv;

class DIGIKAM_EXPORT AlbumWidgetStack : public QWidgetStack
{
Q_OBJECT

public:

    enum AlbumWidgetStackMode
    {
        PreviewAlbumMode=0,
        PreviewImageMode,
        WelcomePageMode,
        MediaPlayerMode
    };

public:

    AlbumWidgetStack(QWidget *parent=0);
    ~AlbumWidgetStack();

    AlbumIconView    *albumIconView();
    ImagePreviewView *imagePreviewView();

    void setPreviewItem(ImageInfo* info=0, ImageInfo *previous=0, ImageInfo *next=0);
    int  previewMode(void);
    void setPreviewMode(int mode);
    void previewLoaded();
    
    void   increaseZoom();
    void   decreaseZoom();
    void   fitToWindow();
    void toggleFitToWindowOr100();
    void   zoomTo100Percents();
    bool   maxZoom();
    bool   minZoom();
    void   setZoomFactor(double z);
    void   setZoomFactorSnapped(double z);
    double zoomFactor();
    double zoomMin();
    double zoomMax();

signals:

    void signalNextItem();
    void signalPrevItem();
    void signalEditItem();
    void signalDeleteItem();
    void signalToggledToPreviewMode(bool);
    void signalBack2Album();
    void signalSlideShow();
    void signalZoomFactorChanged(double);
    void signalInsert2LightTable();

public slots:

    void slotEscapePreview();
    void slotItemsUpdated(const KURL::List&);

private slots: 

    void slotZoomFactorChanged(double);

private:

    AlbumWidgetStackPriv* d;
};

}  // namespace Digikam

#endif /* ALBUMWIDGETSTACK_H */
