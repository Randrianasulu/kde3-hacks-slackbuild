/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-21
 * Description : a generic list view item widget to 
 *               display metadata key like a title
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

// Qt includes.

#include <qpalette.h>
#include <qfont.h>
#include <qpainter.h>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "themeengine.h"
#include "ddebug.h"
#include "mdkeylistviewitem.h"

namespace Digikam
{

MdKeyListViewItem::MdKeyListViewItem(QListView *parent, const QString& key)
                 : QListViewItem(parent)
{
    m_decryptedKey = key;

    // Standard Exif key descriptions.
    if      (key == "Iop")       m_decryptedKey = i18n("Interoperability");
    else if (key == "Image")     m_decryptedKey = i18n("Image Information");
    else if (key == "Photo")     m_decryptedKey = i18n("Photograph Information");
    else if (key == "GPSInfo")   m_decryptedKey = i18n("Global Positioning System");
    else if (key == "Thumbnail") m_decryptedKey = i18n("Embedded Thumbnail");

    // Standard IPTC key descriptions.
    else if (key == "Envelope")     m_decryptedKey = i18n("IIM Envelope");
    else if (key == "Application2") m_decryptedKey = i18n("IIM Application 2");

    setOpen(true);
    setSelected(false);
    setSelectable(false);
}

MdKeyListViewItem::~MdKeyListViewItem()
{
}

QString MdKeyListViewItem::getMdKey()
{
    return m_decryptedKey;
}

void MdKeyListViewItem::paintCell(QPainter* p, const QColorGroup&,
                                  int column, int, int)
{
    p->save();
    QFont fn(p->font());
    fn.setBold(true);
    fn.setItalic(false);
    p->setFont(fn);
    p->setPen(ThemeEngine::instance()->textSelColor());
    int width = listView()->contentsWidth();
    QRect rect(0, 0, width, fn.weight());

    if (column == 1)
        rect.moveLeft(-width/2);

    p->fillRect(rect, ThemeEngine::instance()->thumbSelColor());
    p->drawText(rect, Qt::AlignHCenter, m_decryptedKey);
    p->restore();
}

}  // namespace Digikam
