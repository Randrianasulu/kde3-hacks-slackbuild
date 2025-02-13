/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS file list view item.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <qpainter.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kiconloader.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "gpslistviewitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSListViewItemPriv
{
public:

    GPSListViewItemPriv()
    {
        enabled    = false;
        dirty      = false;
        erase      = false;
        hasGPSInfo = false;
    }

    bool             enabled;
    bool             dirty;
    bool             erase;
    bool             hasGPSInfo;

    QDateTime        date;

    KURL             url;

    GPSDataContainer gpsData;
};

GPSListViewItem::GPSListViewItem(KListView *view, QListViewItem *after, const KURL& url)
               : KListViewItem(view, after)
{
    d = new GPSListViewItemPriv;
    d->url = url;

    setEnabled(false);
    setPixmap(0, SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));
    setText(1, d->url.fileName());

    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load(d->url.path());
    setDateTime(exiv2Iface.getImageDateTime());
    double alt, lat, lng;
    d->hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
    if (hasGPSInfo())
        setGPSInfo(GPSDataContainer(alt, lat, lng, false), false);
}

GPSListViewItem::~GPSListViewItem()
{
    delete d;
}

void GPSListViewItem::setGPSInfo(const GPSDataContainer& gpsData, bool dirty, bool addedManually)
{
    setEnabled(true);
    d->dirty      = dirty;
    d->gpsData    = gpsData;
    d->erase      = false;
    d->hasGPSInfo = true;
    setText(3, QString::number(d->gpsData.latitude(),  'g', 12));
    setText(4, QString::number(d->gpsData.longitude(), 'g', 12));
    setText(5, QString::number(d->gpsData.altitude(),  'g', 12));

    if (isDirty())
    {
        QString status;

        if (d->gpsData.isInterpolated())
            status = i18n("Interpolated");
        else
        {
            if (addedManually)
                status = i18n("Added");
            else
                status = i18n("Found");
        }

        setText(6, status);
    }

    repaint();
}

GPSDataContainer GPSListViewItem::GPSInfo() const
{
    return d->gpsData;
}

void GPSListViewItem::eraseGPSInfo()
{
    d->erase = true;
    d->dirty = true;
    setText(6, i18n("Deleted!"));
    repaint();
}

void GPSListViewItem::setDateTime(const QDateTime& date)
{
    if (date.isValid())
    {
        d->date = date;
        setText(2, date.toString(Qt::LocalDate));
    }
    else
    {
        setText(2, i18n("Not available"));
    }
}

QDateTime GPSListViewItem::dateTime() const
{
    return d->date;
}

KURL GPSListViewItem::url()
{
    return d->url;
}

bool GPSListViewItem::hasGPSInfo()
{
    return d->hasGPSInfo;
}

bool GPSListViewItem::isInterpolated()
{
    return d->gpsData.isInterpolated();
}

void GPSListViewItem::writeGPSInfoToFile()
{
    if (isEnabled() && isDirty())
    {
        setPixmap(1, SmallIcon("run"));
        KExiv2Iface::KExiv2 exiv2Iface;
        bool ret = exiv2Iface.load(d->url.path());

        if (d->erase)
            ret &= exiv2Iface.removeGPSInfo();
        else
        {
            ret &= exiv2Iface.setGPSInfo(d->gpsData.altitude(), 
                                         d->gpsData.latitude(), 
                                         d->gpsData.longitude());
        }

        ret &= exiv2Iface.save(d->url.path());

        if (ret)
            setPixmap(1, SmallIcon("ok"));
        else
            setPixmap(1, SmallIcon("cancel"));

        d->dirty = false;
    }
}

void GPSListViewItem::setEnabled(bool e)
{
    d->enabled = e;
    repaint();
}

bool GPSListViewItem::isEnabled()
{
    return d->enabled;
}

bool GPSListViewItem::isDirty()
{
    return d->dirty;
}

void GPSListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    if (isEnabled())
    {
        if ( isDirty() && !d->erase && column >= 3  && column <= 5 )
        {
            QColorGroup _cg( cg );
            QColor c = _cg.text();
            _cg.setColor( QColorGroup::Text, Qt::red );
            KListViewItem::paintCell( p, _cg, column, width, alignment );
            _cg.setColor( QColorGroup::Text, c );
        }
        else if ( isDirty() && d->erase && column == 6)
        {
            QColorGroup _cg( cg );
            QColor c = _cg.text();
            _cg.setColor( QColorGroup::Text, Qt::red );
            KListViewItem::paintCell( p, _cg, column, width, alignment );
            _cg.setColor( QColorGroup::Text, c );
        }
        else
            KListViewItem::paintCell(p, cg, column, width, alignment);
    }
    else
    {
        QColorGroup _cg( cg );
        QColor c = _cg.text();
        _cg.setColor( QColorGroup::Text, Qt::gray );
        KListViewItem::paintCell( p, _cg, column, width, alignment );
        _cg.setColor( QColorGroup::Text, c );
    }
}

} // NameSpace KIPIGPSSyncPlugin
