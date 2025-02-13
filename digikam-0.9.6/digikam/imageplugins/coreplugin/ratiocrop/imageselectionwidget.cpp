/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-09
 * Description : image selection widget used by ratio crop tool.
 *
 * Copyright (C) 2007 by Jaromir Malenko <malenko at email.cz>
 * Copyright (C) 2008 by Roberto Castagnola <roberto dot castagnola at gmail dot com>
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define OPACITY  0.7
#define RCOL     0xAA
#define GCOL     0xAA
#define BCOL     0xAA

#define MINRANGE 0

// Golden number (1+sqrt(5))/2
#define PHI      1.61803398874989479
// 1/PHI
#define INVPHI   0.61803398874989479

// C++ includes.

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>

// Qt includes.

#include <qregion.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpen.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qsizepolicy.h>

// KDE includes.

#include <kstandarddirs.h>
#include <kcursor.h>
#include <kglobal.h>

// Local includes.

#include "ddebug.h"
#include "imageiface.h"
#include "dimg.h"
#include "imageselectionwidget.h"
#include "imageselectionwidget.moc"

namespace DigikamImagesPluginCore
{

class ImageSelectionWidgetPriv
{
public:

    enum ResizingMode
    {
        ResizingNone = 0,
        ResizingTopLeft,
        ResizingTopRight,
        ResizingBottomLeft,
        ResizingBottomRight
    };

    ImageSelectionWidgetPriv()
    {
        currentResizing = ResizingNone;
        iface           = 0;
        pixmap          = 0;
        guideSize       = 1;
    }

    // Golden guide types.
    bool                 drawGoldenSection;
    bool                 drawGoldenSpiralSection;
    bool                 drawGoldenSpiral;
    bool                 drawGoldenTriangle;

    // Golden guide translations.
    bool                 flipHorGoldenGuide;
    bool                 flipVerGoldenGuide;

    bool                 moving;
    bool                 autoOrientation;
    bool                 preciseCrop;

    int                  guideLinesType;
    int                  guideSize;

    int                  currentAspectRatioType;
    int                  currentResizing;
    int                  currentOrientation;

    float                currentWidthRatioValue;
    float                currentHeightRatioValue;

    QPoint               lastPos;

    QRect                rect;
    QRect                image;                   // Real image dimension.
    QRect                regionSelection;         // Real size image selection.
    QRect                localRegionSelection;    // Local size selection.

    // Draggable local region selection corners.
    QRect                localTopLeftCorner;
    QRect                localBottomLeftCorner;
    QRect                localTopRightCorner;
    QRect                localBottomRightCorner;

    QPixmap             *pixmap;

    QColor               guideColor;

    Digikam::DImg        preview;

    Digikam::ImageIface *iface;
};

ImageSelectionWidget::ImageSelectionWidget(int w, int h, QWidget *parent,
                                           int widthRatioValue, int heightRatioValue,
                                           int aspectRatioType, int orient, int guideLinesType)
                    : QWidget(parent, 0, Qt::WDestructiveClose)
{
    d = new ImageSelectionWidgetPriv;
    d->currentAspectRatioType  = aspectRatioType;
    d->currentWidthRatioValue  = widthRatioValue;
    d->currentHeightRatioValue = heightRatioValue;
    d->currentOrientation      = orient;
    d->guideLinesType          = guideLinesType;
    d->autoOrientation         = false;
    d->preciseCrop             = false;
    d->moving                  = true;
    reverseRatioValues();

    setBackgroundMode(Qt::NoBackground);
    setMinimumSize(w, h);
    setMouseTracking(true);

    d->iface        = new Digikam::ImageIface(w, h);
    uchar *data     = d->iface->getPreviewImage();
    int width       = d->iface->previewWidth();
    int height      = d->iface->previewHeight();
    bool sixteenBit = d->iface->previewSixteenBit();
    bool hasAlpha   = d->iface->previewHasAlpha();
    d->preview      = Digikam::DImg(width, height, sixteenBit, hasAlpha, data);
    delete [] data;
    d->preview.convertToEightBit();
    d->pixmap  = new QPixmap(w, h);

    d->image = QRect(0, 0, d->iface->originalWidth(), d->iface->originalHeight());
    d->rect = QRect(w/2-d->preview.width()/2, h/2-d->preview.height()/2,
                    d->preview.width(), d->preview.height());
    updatePixmap();
    setGoldenGuideTypes(true, false, false, false, false, false);
}

ImageSelectionWidget::~ImageSelectionWidget()
{
    delete d->iface;
    delete d->pixmap;
    delete d;
}

Digikam::ImageIface* ImageSelectionWidget::imageIface()
{
    return d->iface;
}

void ImageSelectionWidget::resizeEvent(QResizeEvent *e)
{
    delete d->pixmap;

    int w           = e->size().width();
    int h           = e->size().height();

    uchar *data     = d->iface->setPreviewImageSize(w, h);
    int width       = d->iface->previewWidth();
    int height      = d->iface->previewHeight();
    bool sixteenBit = d->iface->previewSixteenBit();
    bool hasAlpha   = d->iface->previewHasAlpha();
    d->preview      = Digikam::DImg(width, height, sixteenBit, hasAlpha, data);
    delete [] data;
    d->preview.convertToEightBit();

    d->pixmap = new QPixmap(w, h);

    d->rect = QRect(w/2-d->preview.width()/2, h/2-d->preview.height()/2,
                    d->preview.width(), d->preview.height());
    updatePixmap();
}

int ImageSelectionWidget::getOriginalImageWidth()
{
    return d->image.width();
}

int ImageSelectionWidget::getOriginalImageHeight()
{
    return d->image.height();
}

QRect ImageSelectionWidget::getRegionSelection()
{
    return d->regionSelection;
}

int ImageSelectionWidget::getMinWidthRange()
{
    return MINRANGE;
}

int ImageSelectionWidget::getMinHeightRange()
{
    return MINRANGE;
}

int ImageSelectionWidget::getMaxWidthRange()
{
    int maxW = d->image.width() - d->regionSelection.left();

    if (d->currentAspectRatioType != RATIONONE)
    {
        // Compute max width taking aspect ratio into account
        int t = d->currentWidthRatioValue > d->currentHeightRatioValue ? 1 : 0;
        int h = d->image.height() - d->regionSelection.top();
        int w = rint( ( h + t ) * d->currentWidthRatioValue /
                        d->currentHeightRatioValue ) - t;
        if ( w < maxW )
            maxW = w;
    }

    // Return max width adjusted if a precise crop is wanted
    return computePreciseSize(maxW, d->currentWidthRatioValue);
}

int ImageSelectionWidget::getMaxHeightRange()
{
    int maxH = d->image.height() - d->regionSelection.top();

    if (d->currentAspectRatioType != RATIONONE)
    {
        // Compute max height taking aspect ratio into account
        int t = d->currentHeightRatioValue > d->currentWidthRatioValue ? 1 : 0;
        int w = d->image.width() - d->regionSelection.left();
        int h = rint( ( w + t ) * d->currentHeightRatioValue /
                        d->currentWidthRatioValue ) - t;
        if ( h < maxH )
            maxH = h;
    }

    // Return max height adjusted if a precise crop is wanted
    return computePreciseSize(maxH, d->currentHeightRatioValue);
}

int ImageSelectionWidget::getWidthStep()
{
    if ( d->preciseCrop && preciseCropAvailable() )
        return d->currentWidthRatioValue;
    else
        return 1;
}

int ImageSelectionWidget::getHeightStep()
{
    if ( d->preciseCrop && preciseCropAvailable() )
        return d->currentHeightRatioValue;
    else
        return 1;
}

// Draw a new centered selection with half width (if orientation = Landscape)
// or with half height (if orientation = Portrait)
void ImageSelectionWidget::resetSelection()
{
    d->regionSelection.setWidth(d->image.width()/2);
    d->regionSelection.setHeight(d->image.height()/2);
    applyAspectRatio(d->currentOrientation == Portrait, false);

    setCenterSelection(CenterImage);
}

void ImageSelectionWidget::setCenterSelection(int centerType)
{
    // Adjust selection size if bigger than real image
    if ( d->regionSelection.height() > d->image.height() )
    {
        d->regionSelection.setHeight(d->image.height());
        applyAspectRatio(true, false);
    }
    if ( d->regionSelection.width() > d->image.width() )
    {
        d->regionSelection.setWidth(d->image.width());
        applyAspectRatio(false, false);
    }

    // Set center point for selection
    QPoint center = d->image.center();
    switch (centerType)
    {
        case CenterWidth:
            center.setY(d->regionSelection.center().y());
            break;

        case CenterHeight:
            center.setX(d->regionSelection.center().x());
            break;
    }
    d->regionSelection.moveCenter(center);

    // Repaint
    updatePixmap();
    repaint(false);
    regionSelectionChanged();
}

// Draw a new centered selection with max size
void ImageSelectionWidget::maxAspectSelection()
{
    d->regionSelection.setWidth(d->image.width());
    d->regionSelection.setHeight(d->image.height());
    if ( d->currentAspectRatioType != RATIONONE )
        applyAspectRatio(d->currentOrientation == Portrait, false);

    setCenterSelection(CenterImage);
}

void ImageSelectionWidget::setGoldenGuideTypes(bool drawGoldenSection,  bool drawGoldenSpiralSection,
                                               bool drawGoldenSpiral,   bool drawGoldenTriangle,
                                               bool flipHorGoldenGuide, bool flipVerGoldenGuide)
{
    d->drawGoldenSection       = drawGoldenSection;
    d->drawGoldenSpiralSection = drawGoldenSpiralSection;
    d->drawGoldenSpiral        = drawGoldenSpiral;
    d->drawGoldenTriangle      = drawGoldenTriangle;
    d->flipHorGoldenGuide      = flipHorGoldenGuide;
    d->flipVerGoldenGuide      = flipVerGoldenGuide;
}

void ImageSelectionWidget::slotGuideLines(int guideLinesType)
{
    d->guideLinesType = guideLinesType;
    updatePixmap();
    repaint(false);
}

void ImageSelectionWidget::slotChangeGuideColor(const QColor &color)
{
    d->guideColor = color;
    updatePixmap();
    repaint(false);
}

void ImageSelectionWidget::slotChangeGuideSize(int size)
{
    d->guideSize = size;
    updatePixmap();
    repaint(false);
}

void ImageSelectionWidget::setSelectionOrientation(int orient)
{
    d->currentOrientation = orient;
    reverseRatioValues();
    applyAspectRatio(true);
    emit signalSelectionOrientationChanged( d->currentOrientation );
}

void ImageSelectionWidget::setSelectionAspectRatioType(int aspectRatioType)
{
    d->currentAspectRatioType = aspectRatioType;

    // Set ratio values
    switch(aspectRatioType)
    {
       case RATIO01X01:
          d->currentWidthRatioValue = 1.0;
          d->currentHeightRatioValue = 1.0;
          break;

       case RATIO03X04:
          d->currentWidthRatioValue = 4.0;
          d->currentHeightRatioValue = 3.0;
          break;

       case RATIO02x03:
          d->currentWidthRatioValue = 3.0;
          d->currentHeightRatioValue = 2.0;
          break;

       case RATIO05x07:
          d->currentWidthRatioValue = 7.0;
          d->currentHeightRatioValue = 5.0;
          break;

       case RATIO07x10:
          d->currentWidthRatioValue = 10.0;
          d->currentHeightRatioValue = 7.0;
          break;

       case RATIO04X05:
          d->currentWidthRatioValue = 5.0;
          d->currentHeightRatioValue = 4.0;
          break;

       case RATIOGOLDEN:
          d->currentWidthRatioValue = PHI;
          d->currentHeightRatioValue = 1.0;
          break;
    }

    reverseRatioValues();
    applyAspectRatio(false);
}

void ImageSelectionWidget::setSelectionAspectRatioValue(int widthRatioValue,
                                                        int heightRatioValue)
{
    int gdc = widthRatioValue;

    // Compute greatest common divisor using Euclidean algorithm
    for (int tmp, mod = heightRatioValue; mod != 0; mod = tmp % mod)
    {
        tmp = gdc;
        gdc = mod;
    }

    d->currentWidthRatioValue  = widthRatioValue / gdc;
    d->currentHeightRatioValue = heightRatioValue / gdc;
    d->currentAspectRatioType  = RATIOCUSTOM;

    // Fix orientation
    if ( d->autoOrientation )
    {
        if ( heightRatioValue > widthRatioValue &&
             d->currentOrientation == Landscape )
        {
            d->currentOrientation = Portrait;
            emit signalSelectionOrientationChanged( d->currentOrientation );
        }
        else if ( widthRatioValue > heightRatioValue &&
                  d->currentOrientation == Portrait )
        {
            d->currentOrientation = Landscape;
            emit signalSelectionOrientationChanged( d->currentOrientation );
        }
    }
    else
        reverseRatioValues();

    applyAspectRatio(false);
}

void ImageSelectionWidget::reverseRatioValues()
{
    // Reverse ratio values if needed
    if ( ( d->currentWidthRatioValue > d->currentHeightRatioValue &&
           d->currentOrientation == Portrait ) ||
         ( d->currentHeightRatioValue > d->currentWidthRatioValue &&
           d->currentOrientation == Landscape ) )
    {
        float tmp = d->currentWidthRatioValue;
        d->currentWidthRatioValue = d->currentHeightRatioValue;
        d->currentHeightRatioValue = tmp;
    }
}

bool ImageSelectionWidget::preciseCropAvailable()
{
    // Define when precise crop feature can be used
    // No needed when aspect ratio is 1:1
    switch(d->currentAspectRatioType)
    {
        case RATIONONE:
        case RATIO01X01:
        case RATIOGOLDEN:
            return false;

        case RATIOCUSTOM:
            return ( d->currentWidthRatioValue != d->currentHeightRatioValue );

        default:
            return true;
    }
}

void ImageSelectionWidget::setPreciseCrop(bool precise)
{
    d->preciseCrop = precise;
    applyAspectRatio(false, true);
    regionSelectionChanged();
}

void ImageSelectionWidget::setAutoOrientation(bool orientation)
{
    d->autoOrientation = orientation;
}

void ImageSelectionWidget::setSelectionX(int x)
{
    d->regionSelection.moveLeft(x);
    regionSelectionMoved();
}

void ImageSelectionWidget::setSelectionY(int y)
{
    d->regionSelection.moveTop(y);
    regionSelectionMoved();
}

void ImageSelectionWidget::setSelectionWidth(int w)
{
    d->regionSelection.setWidth(w);
    applyAspectRatio(false, true);

    regionSelectionChanged();
}

void ImageSelectionWidget::setSelectionHeight(int h)
{
    d->regionSelection.setHeight(h);
    applyAspectRatio(true, true);

    regionSelectionChanged();
}

QPoint ImageSelectionWidget::convertPoint(const QPoint pm, bool localToReal)
{
    return convertPoint(pm.x(), pm.y(), localToReal);
}

QPoint ImageSelectionWidget::convertPoint(int x, int y, bool localToReal)
{
    int pmX, pmY;

    if (localToReal)
    {
        pmX = ( x - d->rect.left() ) * (float)d->image.width() /
                    (float)d->preview.width();

        pmY = ( y - d->rect.top() ) * (float)d->image.height() /
                    (float)d->preview.height();
    }
    else
    {
        pmX = d->rect.left() + ( x * (float)d->preview.width() /
                                    (float)d->image.width() );

        pmY = d->rect.top() + ( y * (float)d->preview.height() /
                                    (float)d->image.height() );
    }

    return QPoint(pmX, pmY);
}

int ImageSelectionWidget::computePreciseSize(int size, int step)
{
    // Adjust size if precise crop is wanted
    if ( d->preciseCrop && preciseCropAvailable() )
        size = int(size / step) * step;

    return size;
}

void ImageSelectionWidget::applyAspectRatio(bool useHeight, bool repaintWidget)
{
    // Save selection area for re-adjustment after changing width and height.
    QRect oldRegionSelection = d->regionSelection;

    if ( !useHeight )  // Width changed.
    {
        int w = computePreciseSize(d->regionSelection.width(),
                                   d->currentWidthRatioValue);

        d->regionSelection.setWidth(w);
        switch(d->currentAspectRatioType)
        {
            case RATIONONE:
                break;

            default:
                d->regionSelection.setHeight(rint( w * d->currentHeightRatioValue /
                                             d->currentWidthRatioValue ) );
                break;
        }
    }
    else      // Height changed.
    {
        int h = computePreciseSize(d->regionSelection.height(),
                                   d->currentHeightRatioValue);

        d->regionSelection.setHeight(h);
        switch(d->currentAspectRatioType)
        {
            case RATIONONE:
                break;

            default:
                d->regionSelection.setWidth(rint( h * d->currentWidthRatioValue /
                                            d->currentHeightRatioValue ) );
                break;
        }
    }

    // If we change selection size by a corner, re-adjust the oposite corner position.
    switch(d->currentResizing)
    {
        case ImageSelectionWidgetPriv::ResizingTopLeft:
            d->regionSelection.moveBottomRight( oldRegionSelection.bottomRight() );
            break;

        case ImageSelectionWidgetPriv::ResizingTopRight:
            d->regionSelection.moveBottomLeft( oldRegionSelection.bottomLeft() );
            break;

        case ImageSelectionWidgetPriv::ResizingBottomLeft:
            d->regionSelection.moveTopRight( oldRegionSelection.topRight() );
            break;

        case ImageSelectionWidgetPriv::ResizingBottomRight:
             d->regionSelection.moveTopLeft( oldRegionSelection.topLeft() );
             break;
    }

    if (repaintWidget)
    {
        updatePixmap();
        repaint(false);
    }
}

void ImageSelectionWidget::normalizeRegion()
{
    // Perform normalization of selection area.

    if (d->regionSelection.left() < d->image.left())
        d->regionSelection.moveLeft(d->image.left());

    if (d->regionSelection.top() < d->image.top())
        d->regionSelection.moveTop(d->image.top());

    if (d->regionSelection.right() > d->image.right())
        d->regionSelection.moveRight(d->image.right());

    if (d->regionSelection.bottom() > d->image.bottom())
        d->regionSelection.moveBottom(d->image.bottom());
}

void ImageSelectionWidget::regionSelectionMoved()
{
    normalizeRegion();

    updatePixmap();
    repaint(false);

    emit signalSelectionMoved( d->regionSelection );
}

void ImageSelectionWidget::regionSelectionChanged()
{
    // Compute the intersection of selection region and image region
    QRect cut = d->regionSelection & d->image;

    // Adjust selection size if it was cropped
    if ( d->regionSelection.width() > cut.width() )
    {
        d->regionSelection = cut;
        applyAspectRatio(false);
    }
    if ( d->regionSelection.height() > cut.height() )
    {
        d->regionSelection = cut;
        applyAspectRatio(true);
    }

    emit signalSelectionChanged( d->regionSelection );
}

void ImageSelectionWidget::updatePixmap()
{
    // Updated local selection region.
    d->localRegionSelection.setTopLeft(
            convertPoint(d->regionSelection.topLeft(), false));
    d->localRegionSelection.setBottomRight(
            convertPoint(d->regionSelection.bottomRight(), false));

    // Updated dragging corners region.
    d->localTopLeftCorner.setRect(d->localRegionSelection.left(),
                                  d->localRegionSelection.top(), 8, 8);
    d->localBottomLeftCorner.setRect(d->localRegionSelection.left(),
                                     d->localRegionSelection.bottom() - 7, 8, 8);
    d->localTopRightCorner.setRect(d->localRegionSelection.right() - 7,
                                   d->localRegionSelection.top(), 8, 8);
    d->localBottomRightCorner.setRect(d->localRegionSelection.right() - 7,
                                      d->localRegionSelection.bottom() - 7, 8, 8);

    // Drawing background and image.
    d->pixmap->fill(colorGroup().background());

    if (d->preview.isNull())
        return;

    // Drawing region outside selection grayed.

    Digikam::DImg image = d->preview.copy();

    uchar* ptr = image.bits();
    uchar  r, g, b;

    for (int y=d->rect.top() ; y <= d->rect.bottom() ; y++)
    {
        for (int x=d->rect.left() ; x <= d->rect.right() ; x++)
        {
            if (! d->localRegionSelection.contains(x, y, true) )
            {
                b = ptr[0];
                g = ptr[1];
                r = ptr[2];

                r += (uchar)((RCOL - r) * OPACITY);
                g += (uchar)((GCOL - g) * OPACITY);
                b += (uchar)((BCOL - b) * OPACITY);

                ptr[0] = b;
                ptr[1] = g;
                ptr[2] = r;
            }

            ptr+=4;
        }
    }

    QPixmap pix = d->iface->convertToPixmap(image);
    bitBlt(d->pixmap, d->rect.x(), d->rect.y(), &pix);

    // Stop here if no selection to draw
    if ( d->regionSelection.isEmpty() )
        return;

    QPainter p(d->pixmap);

    // Drawing selection borders.

    p.setPen(QPen(QColor(250, 250, 255), 1, Qt::SolidLine));
    p.drawRect(d->localRegionSelection);

    // Drawing selection corners.

    p.drawRect(d->localTopLeftCorner);
    p.drawRect(d->localBottomLeftCorner);
    p.drawRect(d->localTopRightCorner);
    p.drawRect(d->localBottomRightCorner);

    // Drawing guide lines.

    // Constraint drawing only on local selection region.
    // This is needed because arcs and incurved lines can draw
    // outside a little of local selection region.
    p.setClipping(true);
    p.setClipRect(d->localRegionSelection);

    switch (d->guideLinesType)
    {
       case RulesOfThirds:
       {
            int xThird = d->localRegionSelection.width()  / 3;
            int yThird = d->localRegionSelection.height() / 3;

            p.setPen(QPen(Qt::white, d->guideSize, Qt::SolidLine));
            p.drawLine( d->localRegionSelection.left() + xThird,   d->localRegionSelection.top(),
                        d->localRegionSelection.left() + xThird,   d->localRegionSelection.bottom() );
            p.drawLine( d->localRegionSelection.left() + 2*xThird, d->localRegionSelection.top(),
                        d->localRegionSelection.left() + 2*xThird, d->localRegionSelection.bottom() );

            p.drawLine( d->localRegionSelection.left(),  d->localRegionSelection.top() + yThird,
                        d->localRegionSelection.right(), d->localRegionSelection.top() + yThird );
            p.drawLine( d->localRegionSelection.left(),  d->localRegionSelection.top() + 2*yThird,
                        d->localRegionSelection.right(), d->localRegionSelection.top() + 2*yThird );

            p.setPen(QPen(d->guideColor, d->guideSize, Qt::DotLine));
            p.drawLine( d->localRegionSelection.left() + xThird,   d->localRegionSelection.top(),
                        d->localRegionSelection.left() + xThird,   d->localRegionSelection.bottom() );
            p.drawLine( d->localRegionSelection.left() + 2*xThird, d->localRegionSelection.top(),
                        d->localRegionSelection.left() + 2*xThird, d->localRegionSelection.bottom() );

            p.drawLine( d->localRegionSelection.left(),  d->localRegionSelection.top() + yThird,
                        d->localRegionSelection.right(), d->localRegionSelection.top() + yThird );
            p.drawLine( d->localRegionSelection.left(),  d->localRegionSelection.top() + 2*yThird,
                        d->localRegionSelection.right(), d->localRegionSelection.top() + 2*yThird );
            break;
       }

       case DiagonalMethod:
       {
           // Move coordinates to top, left
           p.translate(d->localRegionSelection.topLeft().x(), d->localRegionSelection.topLeft().y());

           float w = (float)d->localRegionSelection.width();
           float h = (float)d->localRegionSelection.height();

           p.setPen(QPen(Qt::white, d->guideSize, Qt::SolidLine));
           if (w > h) 
           {
               p.drawLine( 0, 0, h, h);
               p.drawLine( 0, h, h, 0);
               p.drawLine( w-h, 0, w, h);
               p.drawLine( w-h, h, w, 0);

           } 
           else 
           {
               p.drawLine( 0, 0, w, w);
               p.drawLine( 0, w, w, 0);
               p.drawLine( 0, h-w, w, h);
               p.drawLine( 0, h, w, h-w);
           }

           p.setPen(QPen(d->guideColor, d->guideSize, Qt::DotLine));
           if (w > h) 
           {
               p.drawLine( 0, 0, h, h);
               p.drawLine( 0, h, h, 0);
               p.drawLine( w-h, 0, w, h);
               p.drawLine( w-h, h, w, 0);

           }
           else 
           {
               p.drawLine( 0, 0, w, w);
               p.drawLine( 0, w, w, 0);
               p.drawLine( 0, h-w, w, h);
               p.drawLine( 0, h, w, h-w);
           }
           break;
       }

       case HarmoniousTriangles:
       {
            // Move coordinates to local center selection.
            p.translate(d->localRegionSelection.center().x(), d->localRegionSelection.center().y());

            // Flip horizontal.
            if (d->flipHorGoldenGuide)
                p.scale(-1, 1);

            // Flip verical.
            if (d->flipVerGoldenGuide)
                p.scale(1, -1);

            float w = (float)d->localRegionSelection.width();
            float h = (float)d->localRegionSelection.height();
            int dst = (int)((h*cos(atan(w/h)) / (cos(atan(h/w)))));

            p.setPen(QPen(Qt::white, d->guideSize, Qt::SolidLine));
            p.drawLine( -d->localRegionSelection.width()/2, -d->localRegionSelection.height()/2,
                         d->localRegionSelection.width()/2,  d->localRegionSelection.height()/2);

            p.drawLine( -d->localRegionSelection.width()/2 + dst, -d->localRegionSelection.height()/2,
                        -d->localRegionSelection.width()/2,        d->localRegionSelection.height()/2);

            p.drawLine( d->localRegionSelection.width()/2,       -d->localRegionSelection.height()/2,
                        d->localRegionSelection.width()/2 - dst,  d->localRegionSelection.height()/2);

            p.setPen(QPen(d->guideColor, d->guideSize, Qt::DotLine));
            p.drawLine( -d->localRegionSelection.width()/2, -d->localRegionSelection.height()/2,
                         d->localRegionSelection.width()/2,  d->localRegionSelection.height()/2);

            p.drawLine( -d->localRegionSelection.width()/2 + dst, -d->localRegionSelection.height()/2,
                        -d->localRegionSelection.width()/2,        d->localRegionSelection.height()/2);

            p.drawLine( d->localRegionSelection.width()/2,       -d->localRegionSelection.height()/2,
                        d->localRegionSelection.width()/2 - dst,  d->localRegionSelection.height()/2);
            break;
       }

       case GoldenMean:
       {
            // Move coordinates to local center selection.
            p.translate(d->localRegionSelection.center().x(), d->localRegionSelection.center().y());

            // Flip horizontal.
            if (d->flipHorGoldenGuide)
                p.scale(-1, 1);

            // Flip vertical.
            if (d->flipVerGoldenGuide)
                p.scale(1, -1);

            int w = d->localRegionSelection.width();
            int h = d->localRegionSelection.height();

            // lengths for the golden mean and half the sizes of the region:
            int w_g = (int)(w*INVPHI);
            int h_g = (int)(h*INVPHI);
            int w_2 = w/2;
            int h_2 = h/2;

            QRect R1(-w_2, -h_2, w_g, h);
            // w - 2*w_2 corrects for one-pixel difference
            // so that R2.right() is really at the right end of the region
            QRect R2(w_g-w_2, h_2-h_g, w-w_g+1-(w - 2*w_2), h_g);

            QRect R3((int)(w_2 - R2.width()*INVPHI), -h_2,
                     (int)(R2.width()*INVPHI), h - R2.height());
            QRect R4(R2.x(), R1.y(), R3.x() - R2.x(),
                     (int)(R3.height()*INVPHI));
            QRect R5(R4.x(), R4.bottom(), (int)(R4.width()*INVPHI),
                     R3.height() - R4.height());
            QRect R6(R5.x() + R5.width(), R5.bottom() - (int)(R5.height()*INVPHI),
                     R3.x() - R5.right(), (int)(R5.height()*INVPHI));
            QRect R7(R6.right() - (int)(R6.width()*INVPHI), R4.bottom(),
                     (int)(R6.width()*INVPHI), R5.height() - R6.height());

            p.setPen(QPen(Qt::white, d->guideSize, Qt::SolidLine));

            // Drawing Golden sections.
            if (d->drawGoldenSection)
            {
               // horizontal lines:
               p.drawLine( R1.left(), R2.top(),
                           R2.right(), R2.top());

               p.drawLine( R1.left(), R1.top() + R2.height(),
                           R2.right(), R1.top() + R2.height());

               // vertical lines:
               p.drawLine( R1.right(), R1.top(),
                           R1.right(), R1.bottom() );

               p.drawLine( R1.left()+R2.width(), R1.top(),
                           R1.left()+R2.width(), R1.bottom() );
            }

            // Drawing Golden triangle guides.
            if (d->drawGoldenTriangle)
            {
               p.drawLine( R1.left(),  R1.bottom(),
                           R2.right(), R1.top() );

               p.drawLine( R1.left(), R1.top(),
                           R2.right() - R1.width(), R1.bottom());

               p.drawLine( R1.left() + R1.width(), R1.top(),
                           R2.right(), R1.bottom() );
            }

            // Drawing Golden spiral sections.
            if (d->drawGoldenSpiralSection)
            {
               p.drawLine( R1.topRight(),   R1.bottomRight() );
               p.drawLine( R2.topLeft(),    R2.topRight() );
               p.drawLine( R3.topLeft(),    R3.bottomLeft() );
               p.drawLine( R4.bottomLeft(), R4.bottomRight() );
               p.drawLine( R5.topRight(),   R5.bottomRight() );
               p.drawLine( R6.topLeft(),    R6.topRight() );
               p.drawLine( R7.topLeft(),    R7.bottomLeft() );
            }

            // Drawing Golden Spiral.
            if (d->drawGoldenSpiral)
            {
               p.drawArc ( R1.left(),
                           R1.top() - R1.height(),
                           2*R1.width(), 2*R1.height(),
                           180*16, 90*16);

               p.drawArc ( R2.right() - 2*R2.width(),
                           R1.bottom() - 2*R2.height(),
                           2*R2.width(), 2*R2.height(),
                           270*16, 90*16);

               p.drawArc ( R2.right() - 2*R3.width(),
                           R3.top(),
                           2*R3.width(), 2*R3.height(),
                           0, 90*16);

               p.drawArc ( R4.left(),
                           R4.top(),
                           2*R4.width(), 2*R4.height(),
                           90*16, 90*16);

               p.drawArc ( R5.left(),
                           R5.top()-R5.height(),
                           2*R5.width(), 2*R5.height(),
                           180*16, 90*16);

               p.drawArc ( R6.left()-R6.width(),
                           R6.top()-R6.height(),
                           2*R6.width(), 2*R6.height(),
                           270*16, 90*16);

               p.drawArc ( R7.left()-R7.width(),
                           R7.top(),
                           2*R7.width(), 2*R7.height(),
                           0, 90*16);
            }

            p.setPen(QPen(d->guideColor, d->guideSize, Qt::DotLine));

            // Drawing Golden sections.
            if (d->drawGoldenSection)
            {
               // horizontal lines:
               p.drawLine( R1.left(), R2.top(),
                           R2.right(), R2.top());

               p.drawLine( R1.left(), R1.top() + R2.height(),
                           R2.right(), R1.top() + R2.height());

               // vertical lines:
               p.drawLine( R1.right(), R1.top(),
                           R1.right(), R1.bottom() );

               p.drawLine( R1.left()+R2.width(), R1.top(),
                           R1.left()+R2.width(), R1.bottom() );
            }

            // Drawing Golden triangle guides.
            if (d->drawGoldenTriangle)
            {
               p.drawLine( R1.left(),  R1.bottom(),
                           R2.right(), R1.top() );

               p.drawLine( R1.left(), R1.top(),
                           R2.right() - R1.width(), R1.bottom());

               p.drawLine( R1.left() + R1.width(), R1.top(),
                           R2.right(), R1.bottom() );
            }

            // Drawing Golden spiral sections.
            if (d->drawGoldenSpiralSection)
            {
               p.drawLine( R1.topRight(),   R1.bottomRight() );
               p.drawLine( R2.topLeft(),    R2.topRight() );
               p.drawLine( R3.topLeft(),    R3.bottomLeft() );
               p.drawLine( R4.bottomLeft(), R4.bottomRight() );
               p.drawLine( R5.topRight(),   R5.bottomRight() );
               p.drawLine( R6.topLeft(),    R6.topRight() );
               p.drawLine( R7.topLeft(),    R7.bottomLeft() );
            }

            // Drawing Golden Spiral.
            if (d->drawGoldenSpiral)
            {
               p.drawArc ( R1.left(),
                           R1.top() - R1.height(),
                           2*R1.width(), 2*R1.height(),
                           180*16, 90*16);

               p.drawArc ( R2.right() - 2*R2.width(),
                           R1.bottom() - 2*R2.height(),
                           2*R2.width(), 2*R2.height(),
                           270*16, 90*16);

               p.drawArc ( R2.right() - 2*R3.width(),
                           R3.top(),
                           2*R3.width(), 2*R3.height(),
                           0, 90*16);

               p.drawArc ( R4.left(),
                           R4.top(),
                           2*R4.width(), 2*R4.height(),
                           90*16, 90*16);

               p.drawArc ( R5.left(),
                           R5.top()-R5.height(),
                           2*R5.width(), 2*R5.height(),
                           180*16, 90*16);

               p.drawArc ( R6.left()-R6.width(),
                           R6.top()-R6.height(),
                           2*R6.width(), 2*R6.height(),
                           270*16, 90*16);

               p.drawArc ( R7.left()-R7.width(),
                           R7.top(),
                           2*R7.width(), 2*R7.height(),
                           0, 90*16);
            }

            break;
       }
    }

    p.setClipping(false);

    p.end();
}

void ImageSelectionWidget::paintEvent( QPaintEvent * )
{
    bitBlt(this, 0, 0, d->pixmap);
}

QPoint ImageSelectionWidget::opposite()
{
    QPoint opp;

    switch(d->currentResizing)
    {
        case ImageSelectionWidgetPriv::ResizingTopRight:
            opp = d->regionSelection.bottomLeft();
            break;

        case ImageSelectionWidgetPriv::ResizingBottomLeft:
            opp = d->regionSelection.topRight();
            break;

        case ImageSelectionWidgetPriv::ResizingBottomRight:
            opp = d->regionSelection.topLeft();
            break;

        case ImageSelectionWidgetPriv::ResizingTopLeft:
        default:
            opp = d->regionSelection.bottomRight();
            break;
    }

    return opp;
}

float ImageSelectionWidget::distance(QPoint a, QPoint b)
{
    return sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2));
}

void ImageSelectionWidget::setCursorResizing()
{
    switch(d->currentResizing)
    {
        case ImageSelectionWidgetPriv::ResizingTopLeft:
            setCursor( KCursor::sizeFDiagCursor() );
            break;

        case ImageSelectionWidgetPriv::ResizingTopRight:
            setCursor( KCursor::sizeBDiagCursor() );
            break;

        case ImageSelectionWidgetPriv::ResizingBottomLeft:
            setCursor( KCursor::sizeBDiagCursor() );
            break;

        case ImageSelectionWidgetPriv::ResizingBottomRight:
            setCursor( KCursor::sizeFDiagCursor() );
            break;
    }
}

void ImageSelectionWidget::placeSelection(QPoint pm, bool symmetric, QPoint center)
{
    // Set orientation
    if ( d->autoOrientation )
    {
        QPoint rel = pm - opposite();

        if ( abs(rel.x()) > abs(rel.y()) )
        {
            if ( d->currentOrientation == Portrait )
            {
                d->currentOrientation = Landscape;
                reverseRatioValues();
                emit signalSelectionOrientationChanged( d->currentOrientation );
            }
        }
        else
        {
            if ( d->currentOrientation == Landscape )
            {
                d->currentOrientation = Portrait;
                reverseRatioValues();
                emit signalSelectionOrientationChanged( d->currentOrientation );
            }
        }
    }

    // Place the corner at the mouse
    // If a symmetric selection is wanted, place opposite corner to
    // the center, double selection size and move it to old center after
    // computing aspect ratio.
    switch(d->currentResizing)
    {
        case ImageSelectionWidgetPriv::ResizingTopLeft:
            // Place corners to the proper position
            d->regionSelection.setTopLeft(pm);
            if ( symmetric )
                d->regionSelection.setBottomRight(center);
            break;

        case ImageSelectionWidgetPriv::ResizingTopRight:
            d->regionSelection.setTopRight(pm);
            if ( symmetric )
                d->regionSelection.setBottomLeft(center);
            break;

        case ImageSelectionWidgetPriv::ResizingBottomLeft:
            d->regionSelection.setBottomLeft(pm);
            if ( symmetric )
                d->regionSelection.setTopRight(center);
            break;

        case ImageSelectionWidgetPriv::ResizingBottomRight:
            d->regionSelection.setBottomRight(pm);
            if ( symmetric )
                d->regionSelection.setTopLeft(center);
            break;
    }

    if ( symmetric )
        d->regionSelection.setSize(d->regionSelection.size()*2);
    applyAspectRatio(d->currentOrientation == Portrait, false);
    if ( symmetric )
        d->regionSelection.moveCenter(center);

    // Repaint
    updatePixmap();
    repaint(false);
}

void ImageSelectionWidget::mousePressEvent ( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        QPoint pm = QPoint(e->x(), e->y());
        QPoint pmVirtual = convertPoint(pm);
        d->moving = false;

        if ( (e->state() & Qt::ShiftButton) == Qt::ShiftButton )
        {
            bool symmetric = (e->state() & Qt::ControlButton ) == Qt::ControlButton;
            QPoint center = d->regionSelection.center();

            // Find the closest corner

            QPoint points[] = { d->regionSelection.topLeft(),
                                d->regionSelection.topRight(),
                                d->regionSelection.bottomLeft(),
                                d->regionSelection.bottomRight() };
            int resizings[] = { ImageSelectionWidgetPriv::ResizingTopLeft,
                                ImageSelectionWidgetPriv::ResizingTopRight,
                                ImageSelectionWidgetPriv::ResizingBottomLeft,
                                ImageSelectionWidgetPriv::ResizingBottomRight };
            float dist = -1;
            for (int i = 0 ; i < 4 ; i++)
            {
                QPoint point = points[i];
                float dist2 = distance(pmVirtual, point);
                if (dist2 < dist || d->currentResizing == ImageSelectionWidgetPriv::ResizingNone)
                {
                    dist = dist2;
                    d->currentResizing = resizings[i];
                }
            }

            setCursorResizing();

            placeSelection(pmVirtual, symmetric, center);
        }
        else
        {
            if ( d->localTopLeftCorner.contains( pm ) )
                d->currentResizing = ImageSelectionWidgetPriv::ResizingTopLeft;
            else if ( d->localTopRightCorner.contains( pm ) )
                d->currentResizing = ImageSelectionWidgetPriv::ResizingTopRight;
            else if ( d->localBottomLeftCorner.contains( pm ) )
                d->currentResizing = ImageSelectionWidgetPriv::ResizingBottomLeft;
            else if ( d->localBottomRightCorner.contains( pm ) )
                d->currentResizing = ImageSelectionWidgetPriv::ResizingBottomRight;
            else
            {
                d->lastPos = pmVirtual;
                setCursor( KCursor::sizeAllCursor() );

                if (d->regionSelection.contains( pmVirtual ) )
                {
                    d->moving = true;
                }
                else
                {
                    d->regionSelection.moveCenter( pmVirtual );
                    normalizeRegion();
                    updatePixmap();
                    repaint(false);
                }
            }
        }
    }
}

void ImageSelectionWidget::mouseReleaseEvent ( QMouseEvent * )
{
    if ( d->currentResizing != ImageSelectionWidgetPriv::ResizingNone )
    {
        setCursor( KCursor::arrowCursor() );
        regionSelectionChanged();
        d->currentResizing = ImageSelectionWidgetPriv::ResizingNone;
    }
    else if ( d->regionSelection.contains( d->lastPos ) )
    {
        setCursor( KCursor::handCursor() );
        regionSelectionMoved();
    }
    else
    {
        setCursor( KCursor::arrowCursor() );
        regionSelectionMoved();
    }
}

void ImageSelectionWidget::mouseMoveEvent ( QMouseEvent * e )
{
    if ( ( e->state() & Qt::LeftButton ) == Qt::LeftButton )
    {
        if ( d->moving )
        {
            setCursor( KCursor::sizeAllCursor() );
            QPoint newPos = convertPoint(e->x(), e->y());

            d->regionSelection.moveBy( newPos.x() - d->lastPos.x(),
                                       newPos.y() - d->lastPos.y() );

            d->lastPos = newPos;

            normalizeRegion();

            updatePixmap();
            repaint(false);
        }
        else
        {
            QPoint pmVirtual = convertPoint(e->x(), e->y());

            if ( d->currentResizing == ImageSelectionWidgetPriv::ResizingNone )
            {
                d->regionSelection.setTopLeft( pmVirtual );
                d->regionSelection.setBottomRight( pmVirtual );
                d->currentResizing = ImageSelectionWidgetPriv::ResizingTopLeft; // set to anything
            }

            QPoint center = d->regionSelection.center();
            bool symmetric = (e->state() & Qt::ControlButton ) == Qt::ControlButton;

            // Change resizing mode

            QPoint opp = symmetric ? center : opposite();
            QPoint dir = pmVirtual - opp;

            if ( dir.x() > 0 && dir.y() > 0 && d->currentResizing != ImageSelectionWidgetPriv::ResizingBottomRight)
            {
                d->currentResizing = ImageSelectionWidgetPriv::ResizingBottomRight;
                d->regionSelection.setTopLeft( opp );
                setCursor( KCursor::sizeFDiagCursor() );
            }
            else if ( dir.x() > 0 && dir.y() < 0 && d->currentResizing != ImageSelectionWidgetPriv::ResizingTopRight)
            {
                d->currentResizing = ImageSelectionWidgetPriv::ResizingTopRight;
                d->regionSelection.setBottomLeft( opp );
                setCursor( KCursor::sizeBDiagCursor() );
            }
            else if ( dir.x() < 0 && dir.y() > 0 && d->currentResizing != ImageSelectionWidgetPriv::ResizingBottomLeft)
            {
                d->currentResizing = ImageSelectionWidgetPriv::ResizingBottomLeft;
                d->regionSelection.setTopRight( opp );
                setCursor( KCursor::sizeBDiagCursor() );
            }
            else if ( dir.x() < 0 && dir.y() < 0 && d->currentResizing != ImageSelectionWidgetPriv::ResizingTopLeft)
            {
                d->currentResizing = ImageSelectionWidgetPriv::ResizingTopLeft;
                d->regionSelection.setBottomRight( opp );
                setCursor( KCursor::sizeFDiagCursor() );
            }
            else
            {
                if ( dir.x() == 0 && dir.y() == 0 )
                    setCursor( KCursor::sizeAllCursor() );
                else if ( dir.x() == 0 )
                    setCursor( KCursor::sizeHorCursor() );
                else if ( dir.y() == 0 )
                    setCursor( KCursor::sizeVerCursor() );
            }

            placeSelection(pmVirtual, symmetric, center);
        }
    }
    else
    {
        if ( d->localTopLeftCorner.contains( e->x(), e->y() ) ||
             d->localBottomRightCorner.contains( e->x(), e->y() ) )
            setCursor( KCursor::sizeFDiagCursor() );
        else if ( d->localTopRightCorner.contains( e->x(), e->y() ) ||
                  d->localBottomLeftCorner.contains( e->x(), e->y() ) )
            setCursor( KCursor::sizeBDiagCursor() );
        else if ( d->localRegionSelection.contains( e->x(), e->y() ) )
            setCursor( KCursor::handCursor() );
        else
            setCursor( KCursor::arrowCursor() );
    }
}

}  // NameSpace DigikamImagesPluginCore
