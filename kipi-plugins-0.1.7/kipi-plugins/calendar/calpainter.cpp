/* ============================================================
 * File  : calpainter.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-02
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

#include <qpainter.h>
#include <qrect.h>
#include <qpaintdevice.h>
#include <qdatetime.h>
#include <qpaintdevicemetrics.h>
#include <qstring.h>
#include <qimage.h>
#include <qtimer.h>
#include <qwmatrix.h>

// KDE includes.

#include <kglobal.h>
#include <klocale.h>
#include <kdeversion.h>
#include <kcalendarsystem.h>
#include <kdebug.h>

// Local includes.

#include "calpainter.h"
#include "calsettings.h"
#include "calformatter.h"

namespace KIPICalendarPlugin
{

CalPainter::CalPainter(QPaintDevice *pd)
{
    pd_ = pd;
}

CalPainter::~CalPainter()
{
}

void CalPainter::setYearMonth(int year, int month)
{
    year_  = year;
    month_ = month;
}


void CalPainter::paint(bool useDeviceMetrics)
{
    if (!pd_) return;

    QPainter *painter = new QPainter();
    painter->begin(pd_);

    int width = 0;
    int height = 0;

    CalParams& params = CalSettings::instance()->calParams;
    if (!useDeviceMetrics) {
        width  = params.width;
        height = params.height;
    }
    else {
        QPaintDeviceMetrics metrics( painter->device());
        width  = metrics.width();
        height = metrics.height();
    }

    // --------------------------------------------------

    int days[42];
    int startDayOffset = KGlobal::locale()->weekStartDay();

    for (int i=0; i<42; i++)
        days[i] = -1;

    QDate d;
    KGlobal::locale()->calendar()->setYMD(d, year_, month_, 1);
    int s = d.dayOfWeek();

    if (s+7-startDayOffset >= 7)
            s=s-7;

    for (int i=s; i<(s+KGlobal::locale()->calendar()->daysInMonth(d)); i++) {
        days[i + (7-startDayOffset)] = i-s+1;
    }

    // -----------------------------------------------

    QRect rCal, rImage, rCalHeader;
    int cellSizeX;
    int cellSizeY;

    switch (params.imgPos) {

    case(CalParams::Top): {

        rImage = QRect(0,0,0,0);
        rImage.setWidth(width);
        rImage.setHeight((int)(height *
                               (params.ratio)/(params.ratio + 100)));

        int remainingHeight = height - rImage.height();
        cellSizeX = (width - 20)/7;
        cellSizeY = remainingHeight/8;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight((int)(cellSizeY*7));

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight((int)(cellSizeY));
        rCalHeader.moveTop(rImage.bottom());

        rCalHeader.moveLeft(width/2 - rCalHeader.width()/2);
        rCal.moveTopLeft(rCalHeader.bottomLeft());

        break;

    }

    case(CalParams::Left): {

        rImage = QRect(0,0,0,0);
        rImage.setHeight(height);
        rImage.setWidth((int)(width *
                              (params.ratio)/(params.ratio + 100)));

        int remainingWidth = width - rImage.width();
        cellSizeX = remainingWidth/8;
        cellSizeY = (height - 20)/7;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight(cellSizeY*7);

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight(cellSizeY);
        rCalHeader.moveLeft(rImage.right() + cellSizeX);
        rCalHeader.moveTop(height/2 -
                           (rCalHeader.height()+rCal.height()/2));

        rCal.moveTopLeft(rCalHeader.bottomLeft());

        break;
    }

    case(CalParams::Right): {

        rImage = QRect(0,0,0,0);
        rImage.setHeight(height);
        rImage.setWidth((int)(width *
                              (params.ratio)/(params.ratio + 100)));

        int remainingWidth = width - rImage.width();
        cellSizeX = remainingWidth/8;
        cellSizeY = (height - 20)/7;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight(cellSizeY*7);

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight(cellSizeY);
        rCalHeader.moveTop(height/2 -
                           (rCalHeader.height()+rCal.height()/2));
        rCal.moveTop(rCalHeader.bottom());

        rImage.moveLeft(width - rImage.width());

        break;
    }

    default:
        return;
    }

    int fontPixels = (int)((float)cellSizeX/3.0);
    params.baseFont.setPixelSize(fontPixels);

    // ---------------------------------------------------------------

    painter->fillRect(0,0,width, height, Qt::white);
    painter->fillRect(rImage, Qt::black);
    painter->setFont(params.baseFont);

    // ---------------------------------------------------------------

    painter->save();
    QFont f(params.baseFont);
    f.setBold(true);
    f.setPixelSize(f.pixelSize() + 5);
    painter->setFont(f);
    painter->drawText(rCalHeader, Qt::AlignLeft|Qt::AlignVCenter,
                      QString::number(year_));
#if KDE_IS_VERSION(3,2,0)
    painter->drawText(rCalHeader, Qt::AlignRight|Qt::AlignVCenter,
                      KGlobal::locale()->calendar()->monthName(month_, year_));
#else
    painter->drawText(rCalHeader, Qt::AlignRight|Qt::AlignVCenter,
                      KGlobal::locale()->monthName(month_));
#endif
    painter->restore();

    // ---------------------------------------------------------------

    int   sx, sy;
    QRect r, rsmall;

    r.setWidth(cellSizeX);
    r.setHeight(cellSizeY);

    int index = 0;

    painter->save();

    painter->setPen(Qt::red);
    sy = rCal.top();
    for (int i=0; i<7; i++)
    {
        int dayname = i + startDayOffset;
        if (dayname > 7)
            dayname = dayname-7;

        sx = cellSizeX * i + rCal.left();
        r.moveTopLeft(QPoint(sx,sy));
        rsmall = r;
        rsmall.setWidth(r.width() - 2);
        rsmall.setHeight(r.height() - 2);
#if KDE_IS_VERSION(3,2,0)
        painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                          KGlobal::locale()->calendar()->weekDayName(dayname, true));
#else
        painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                          KGlobal::locale()->weekDayName(dayname, true));
#endif

    }

    painter->restore();

    for (int j=0; j<6; j++) {
        sy = cellSizeY * (j + 1) + rCal.top();
        for (int i=0; i<7; i++) {
            sx = cellSizeX * i + rCal.left();
            r.moveTopLeft(QPoint(sx,sy));
            rsmall = r;
            rsmall.setWidth(r.width() - 2);
            rsmall.setHeight(r.height() - 2);
            if (days[index] != -1)
                painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                           QString::number(days[index]));
            index++;
        }
    }

    // ---------------------------------------------------------------

    if (params.drawLines) {

        sx = rCal.left();
        for (int j=0; j<8; j++) {
            sy = cellSizeY * j + rCal.top();
            painter->drawLine(sx,sy,rCal.right(),sy);
        }

        sy = rCal.top();
        for (int i=0; i<8; i++) {
            sx = cellSizeX * i + rCal.left();
            painter->drawLine(sx,sy,sx,rCal.bottom());
        }

    }


    painter->end();
    delete painter;
}

CalBlockPainter::CalBlockPainter(QObject *parent, int year, int month,
                                 const KURL& imagePath, int angle, CalFormatter *formatter, QPainter *painter)
    : QObject(parent), painter_(painter)
{
    int width = 0;
    int height = 0;

    CalParams& params = CalSettings::instance()->calParams;

    QPaintDeviceMetrics metrics( painter->device());
    width  = metrics.width();
    height = metrics.height();

    // --------------------------------------------------

    int  days[42];
    int startDayOffset = KGlobal::locale()->weekStartDay();

    for (int i=0; i<42; i++)
        days[i] = -1;

    QDate d;
    KGlobal::locale()->calendar()->setYMD(d, year, month, 1);
    int s = d.dayOfWeek();

    if (s+7-startDayOffset >= 7)
            s=s-7;

    for (int i=s; i<(s+KGlobal::locale()->calendar()->daysInMonth(d)); i++) {
        days[i+(7-startDayOffset)] = i-s+1;
    }

    // -----------------------------------------------

    QRect rCal, rImage, rCalHeader;
    int cellSizeX;
    int cellSizeY;

    switch (params.imgPos) {

    case(CalParams::Top): {

        rImage = QRect(0,0,0,0);
        rImage.setWidth(width);
        rImage.setHeight((int)(height *
                               (params.ratio)/(params.ratio + 100)));

        int remainingHeight = height - rImage.height();
        cellSizeX = (width - 20)/7;
        cellSizeY = remainingHeight/8;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight((int)(cellSizeY*7));

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight((int)(cellSizeY));
        rCalHeader.moveTop(rImage.bottom());

        rCalHeader.moveLeft(width/2 - rCalHeader.width()/2);
        rCal.moveTopLeft(rCalHeader.bottomLeft());

        break;

    }

    case(CalParams::Left): {

        rImage = QRect(0,0,0,0);
        rImage.setHeight(height);
        rImage.setWidth((int)(width *
                              (params.ratio)/(params.ratio + 100)));

        int remainingWidth = width - rImage.width();
        cellSizeX = remainingWidth/8;
        cellSizeY = (height - 20)/7;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight(cellSizeY*7);

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight(cellSizeY);
        rCalHeader.moveLeft(rImage.right() + cellSizeX);
        rCalHeader.moveTop(height/2 -
                           (rCalHeader.height()+rCal.height()/2));

        rCal.moveTopLeft(rCalHeader.bottomLeft());

        break;
    }

    case(CalParams::Right): {

        rImage = QRect(0,0,0,0);
        rImage.setHeight(height);
        rImage.setWidth((int)(width *
                              (params.ratio)/(params.ratio + 100)));

        int remainingWidth = width - rImage.width();
        cellSizeX = remainingWidth/8;
        cellSizeY = (height - 20)/7;

        rCal = QRect(0,0,0,0);
        rCal.setWidth(cellSizeX*7);
        rCal.setHeight(cellSizeY*7);

        rCalHeader = QRect(0,0,0,0);
        rCalHeader.setWidth(rCal.width());
        rCalHeader.setHeight(cellSizeY);
        rCalHeader.moveTop(height/2 -
                           (rCalHeader.height()+rCal.height()/2));
        rCal.moveTop(rCalHeader.bottom());

        rImage.moveLeft(width - rImage.width());

        break;
    }

    default:
        return;
    }

    int fontPixels = (int)((float)cellSizeX/3.0);
    params.baseFont.setPixelSize(fontPixels);

    // ---------------------------------------------------------------

    painter->fillRect(0,0,width, height, Qt::white);
    painter->setFont(params.baseFont);

    // ---------------------------------------------------------------

    painter->save();
    QFont f(params.baseFont);
    f.setBold(true);
    f.setPixelSize(f.pixelSize() + 5);
    painter->setFont(f);
    painter->drawText(rCalHeader, Qt::AlignLeft|Qt::AlignVCenter,
                      QString::number(year));
#if KDE_IS_VERSION(3,2,0)
    painter->drawText(rCalHeader, Qt::AlignRight|Qt::AlignVCenter,
                      KGlobal::locale()->calendar()->monthName(month, year));
#else
    painter->drawText(rCalHeader, Qt::AlignRight|Qt::AlignVCenter,
                      KGlobal::locale()->monthName(month));
#endif
    painter->restore();

    // ---------------------------------------------------------------

    int   sx, sy;
    QRect r, rsmall, rSpecial;

    r.setWidth(cellSizeX);
    r.setHeight(cellSizeY);

    int index = 0;

    painter->save();

    painter->setPen(Qt::red);
    sy = rCal.top();
    for (int i=0; i<7; i++) {

        int dayname = i + startDayOffset;
        if (dayname > 7)
            dayname = dayname-7;

        sx = cellSizeX * i + rCal.left();
        r.moveTopLeft(QPoint(sx,sy));
        rsmall = r;
        rsmall.setWidth(r.width() - 2);
        rsmall.setHeight(r.height() - 2);
#if KDE_IS_VERSION(3,2,0)
        painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                          KGlobal::locale()->calendar()->weekDayName(dayname, true));
#else
        painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                          KGlobal::locale()->weekDayName(dayname, true));
#endif
    }

    painter->restore();

    for (int j=0; j<6; j++) {
        sy = cellSizeY * (j + 1) + rCal.top();
        for (int i=0; i<7; i++) {
            sx = cellSizeX * i + rCal.left();
            r.moveTopLeft(QPoint(sx,sy));
            rsmall = r;
            rsmall.setWidth(r.width() - 2);
            rsmall.setHeight(r.height() - 2);
            if (days[index] != -1) {
                if (formatter->isSpecial(month, days[index])) {

                    painter->save();
                    painter->setPen( formatter->getDayColor(month, days[index]) );
                    painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                           QString::number(days[index]));

                    QString descr = formatter->getDayDescr(month, days[index]);
                    kdDebug(51000) << "Painting special info: '" << descr << "' for date " << days[index] << "/" << month << endl;
                    rSpecial = rsmall;
                    rSpecial.moveBy(2,0);
                    QFont f(params.baseFont);
                    f.setPixelSize(f.pixelSize() / 3);
                    painter->setFont(f);

                    painter->drawText(rSpecial, Qt::AlignLeft|Qt::AlignTop, descr);

                    painter->restore();
                }
                else
                {
                    painter->drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                           QString::number(days[index]));
                }
            }
            index++;
        }
    }

    // ---------------------------------------------------------------

    if (params.drawLines) {

        sx = rCal.left();
        for (int j=0; j<8; j++) {
            sy = cellSizeY * j + rCal.top();
            painter->drawLine(sx,sy,rCal.right(),sy);
        }

        sy = rCal.top();
        for (int i=0; i<8; i++) {
            sx = cellSizeX * i + rCal.left();
            painter->drawLine(sx,sy,sx,rCal.bottom());
        }

    }

    // ---------------------------------------------------------------

    QImage img = QImage(imagePath.path()); // PENDING(blackie) handle general URLS.
    if (!img.isNull()) {
        if ( angle != 0 ) {
            QWMatrix matrix;
            matrix.rotate( angle );
            img = img.xForm( matrix );
        }

        image_ = new QImage(img.smoothScale(rImage.width(), rImage.height(),
                                            QImage::ScaleMin));
        // Center the image
        int x = rImage.x() + rImage.width()/2 - image_->width()/2;
        int y = rImage.y() + rImage.height()/2 - image_->height()/2;

        // Print in 10 pixel strips
        int blockSize = 10;
        numBlocks_ = image_->height()/blockSize;

        blocks_ = new struct Block[numBlocks_];
        struct Block* pb = blocks_;
        struct Block* pbNext;

        pb->x  = x;
        pb->y  = y;
        pb->sx = 0;
        pb->sy = 0;
        pb->w  = image_->width();
        pb->h  = blockSize;

        for (int j=1; j<numBlocks_; j++) {
            pbNext = pb + 1;
            pbNext->x  = x;
            pbNext->y  = pb->y + blockSize;
            pbNext->sx = 0;
            pbNext->sy = pb->sy + blockSize;
            pbNext->w  = pb->w;
            pbNext->h  = blockSize;
            pb = pbNext;
        }

        pb->h  = image_->height() - pb->y;

        currBlock_ = 0;
        QTimer::singleShot(10, this, SLOT(slotPaintNextBlock()));

    }
    else {
        image_    = 0;
        blocks_   = 0;
        painter->fillRect(rImage, Qt::blue);
    }
}

CalBlockPainter::~CalBlockPainter()
{
    if (blocks_) delete [] blocks_;
    if (image_) delete image_;
}

void CalBlockPainter::slotPaintNextBlock()
{
    struct Block& b = blocks_[currBlock_];
    painter_->drawImage(b.x, b.y, *image_, b.sx, b.sy,
                        b.w, b.h);
    currBlock_++;
    if (currBlock_ < numBlocks_) {
        QTimer::singleShot(10, this, SLOT(slotPaintNextBlock()));
        emit signalProgress(currBlock_+1, numBlocks_);
    }
    else {
        emit signalCompleted();
        delete this;
    }

}

}  // NameSpace KIPICalendarPlugin

#include "calpainter.moc"
