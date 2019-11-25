/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-19
 * Description : Album item file tip adapted from kfiletip 
 *               (konqueror - konq_iconviewwidget.cc)
 *
 * Copyright (C) 1998-1999 by Torben Weis <weis@kde.org>
 * Copyright (C) 2000-2002 by David Faure <david@mandrakesoft.com>
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef ALBUMFILETIP_H
#define ALBUMFILETIP_H

// Qt includes.

#include <qframe.h>
#include <qstring.h>

namespace Digikam
{

class AlbumIconView;
class AlbumIconItem;
class AlbumFileTipPriv;

class AlbumFileTip : public QFrame
{
public:

    AlbumFileTip(AlbumIconView* view);
    ~AlbumFileTip();

    void setIconItem(AlbumIconItem* iconItem);

protected:

    bool event(QEvent *e);
    void resizeEvent(QResizeEvent* e);
    void drawContents(QPainter *p);

private:

    void    reposition();
    void    renderArrows();
    void    updateText();
    QString breakString(const QString& str);

private:

    AlbumFileTipPriv *d;
};

}  // namespace Digikam

#endif /* ALBUMFILETIP_H */
