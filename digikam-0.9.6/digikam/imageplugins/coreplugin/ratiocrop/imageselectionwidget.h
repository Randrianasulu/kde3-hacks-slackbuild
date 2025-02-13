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

#ifndef IMAGESELECTIONWIDGET_H
#define IMAGESELECTIONWIDGET_H

// Qt includes.

#include <qwidget.h>
#include <qrect.h>
#include <qcolor.h>

namespace Digikam
{
class ImageIface;
}

namespace DigikamImagesPluginCore
{

class ImageSelectionWidgetPriv;

class ImageSelectionWidget : public QWidget
{

Q_OBJECT

public:

    enum RatioAspect           // Constrained Aspect Ratio list.
    {
        RATIOCUSTOM=0,             // Custom aspect ratio.
        RATIO01X01,                // 1:1
        RATIO02x03,                // 2:3
        RATIO03X04,                // 3:4
        RATIO04X05,                // 4:5
        RATIO05x07,                // 5:7
        RATIO07x10,                // 7:10
        RATIOGOLDEN,               // Golden ratio : 1:1.618
        RATIONONE                  // No aspect ratio.
    };

    enum Orient
    {
        Landscape = 0,
        Portrait
    };

    enum CenterType
    {
        CenterWidth = 0,           // Center selection to the center of image width.
        CenterHeight,              // Center selection to the center of image height.
        CenterImage                // Center selection to the center of image.
    };

    // Proportion : Golden Ratio and Rule of Thirds. More information at this url:
    // http://photoinf.com/General/Robert_Berdan/Composition_and_the_Elements_of_Visual_Design.htm

    enum GuideLineType
    {
        RulesOfThirds = 0,         // Line guides position to 1/3 width and height.
        DiagonalMethod,            // Diagonal Method to improve composition.
        HarmoniousTriangles,       // Harmonious Triangle to improve composition.
        GoldenMean,                // Guides tools using Phi ratio (1.618).
        GuideNone                  // No guide line.
    };

public:

    ImageSelectionWidget(int width, int height, QWidget *parent=0,
                         int widthRatioValue=1, int heightRatioValue=1,
                         int aspectRatio=RATIO01X01, int orient=Landscape,
                         int guideLinesType=GuideNone);
    ~ImageSelectionWidget();

    void  setCenterSelection(int centerType=CenterImage);
    void  setSelectionX(int x);
    void  setSelectionY(int y);
    void  setSelectionWidth(int w);
    void  setSelectionHeight(int h);
    void  setSelectionOrientation(int orient);
    void  setPreciseCrop(bool precise);
    void  setAutoOrientation(bool orientation);
    void  setSelectionAspectRatioType(int aspectRatioType);
    void  setSelectionAspectRatioValue(int widthRatioValue, int heightRatioValue);
    void  setGoldenGuideTypes(bool drawGoldenSection,  bool drawGoldenSpiralSection,
                              bool drawGoldenSpiral,   bool drawGoldenTriangle,
                              bool flipHorGoldenGuide, bool flipVerGoldenGuide);

    int   getOriginalImageWidth();
    int   getOriginalImageHeight();
    QRect getRegionSelection();

    int   getMinWidthRange();
    int   getMinHeightRange();
    int   getMaxWidthRange();
    int   getMaxHeightRange();
    int   getWidthStep();
    int   getHeightStep();

    bool  preciseCropAvailable();

    void  resetSelection();
    void  maxAspectSelection();

    Digikam::ImageIface* imageIface();

public slots:

    void slotGuideLines(int guideLinesType);
    void slotChangeGuideColor(const QColor &color);
    void slotChangeGuideSize(int size);

signals:

    void signalSelectionMoved( QRect rect );
    void signalSelectionChanged( QRect rect );
    void signalSelectionOrientationChanged( int newOrientation );

protected:

    void paintEvent( QPaintEvent *e );
    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent ( QMouseEvent * e );
    void mouseMoveEvent ( QMouseEvent * e );
    void resizeEvent(QResizeEvent * e);

private:

    // Recalculate the target selection position and emit 'signalSelectionMoved'.
    void regionSelectionMoved();

    void regionSelectionChanged();
    QPoint convertPoint(const QPoint pm, bool localToReal=true);
    QPoint convertPoint(int x, int y, bool localToReal=true);
    void normalizeRegion();
    void reverseRatioValues();
    int computePreciseSize(int size, int step);
    void applyAspectRatio(bool useHeight, bool repaintWidget=true);
    void updatePixmap();
    QPoint opposite();
    float distance(QPoint a, QPoint b);
    void placeSelection(QPoint pm, bool symetric, QPoint center);
    void setCursorResizing();

private:

    ImageSelectionWidgetPriv* d;
};

}  // NameSpace DigikamImagesPluginCore

#endif /* IMAGESELECTIONWIDGET_H */
