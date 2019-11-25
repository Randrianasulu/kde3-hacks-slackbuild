/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a ListView to display black frames
 * 
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef BLACKFRAMELISTVIEW_H
#define BLACKFRAMELISTVIEW_H

// Qt includes.

#include <qimage.h>
#include <qstring.h>
#include <qsize.h>
#include <qpoint.h>
#include <qvaluelist.h>
#include <qlistview.h>

// KDE includes.

#include <kurl.h>
#include <klocale.h>

// Local includes.

#include "blackframeparser.h"
#include "hotpixel.h"

namespace DigikamHotPixelsImagesPlugin
{

class BlackFrameListView : public QListView
{
    Q_OBJECT

public:

    BlackFrameListView(QWidget* parent=0);
    ~BlackFrameListView(){};

signals:

    void blackFrameSelected(QValueList<HotPixel>, const KURL&);

private slots:

    void slotParsed(QValueList<HotPixel> hotPixels, const KURL& blackFrameURL)
    {
       emit blackFrameSelected(hotPixels, blackFrameURL);
    };
};

// --------------------------------------------------------------------------

class BlackFrameListViewItem : public QObject, QListViewItem
{
Q_OBJECT

public:

    BlackFrameListViewItem(BlackFrameListView* parent, const KURL &url);
    ~BlackFrameListViewItem(){};

    virtual QString text(int column)const;
    virtual void paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align);
    virtual int width(const QFontMetrics& fm, const QListView* lv, int c)const;

signals:

    void parsed(QValueList<HotPixel>, const KURL&);
    void signalLoadingProgress(float);
    void signalLoadingComplete();

protected:

    void activate();

private:

    QPixmap thumb(const QSize& size);

private slots:

    void slotParsed(QValueList<HotPixel>);

private:

    // Data contained within each listview item
    QImage                m_thumb;
    QImage                m_image;

    QSize                 m_imageSize;

    QValueList <HotPixel> m_hotPixels;

    QString               m_blackFrameDesc;

    KURL                  m_blackFrameURL;

    BlackFrameParser     *m_parser;

    BlackFrameListView   *m_parent;
};

}  // NameSpace DigikamHotPixelsImagesPlugin

#endif  // BLACKFRAMELISTVIEW_H
