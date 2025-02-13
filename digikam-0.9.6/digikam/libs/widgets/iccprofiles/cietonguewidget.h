/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-10
 * Description : a widget to display CIE tongue from
 * an ICC profile.
 * 
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Any source code are inspired from lprof project and
 * Copyright (C) 1998-2001 Marti Maria
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

#ifndef CIETONGUEWIDGET_H
#define CIETONGUEWIDGET_H

#include <config.h>

// Qt includes.

#include <qwidget.h>
#include <qcolor.h>

// KDE includes.

#include <kurl.h>

// lcms includes.

#include LCMS_HEADER
#if LCMS_VERSION < 114
#define cmsTakeCopyright(profile) "Unknown"
#endif // LCMS_VERSION < 114

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

class CIETongueWidgetPriv;

class DIGIKAM_EXPORT CIETongueWidget : public QWidget
{
Q_OBJECT

public:

    CIETongueWidget(int w, int h, QWidget *parent=0, cmsHPROFILE hMonitor=0);
    ~CIETongueWidget();

    bool setProfileData(const QByteArray& profileData=QByteArray());
    bool setProfileFromFile(const KURL& file=KURL());
    
    void loadingStarted();
    void loadingFailed();

protected:

    int  grids(double val) const;

    void outlineTongue();
    void fillTongue();
    void drawTongueAxis();
    void drawTongueGrid();
    void drawLabels();

    QRgb colorByCoord(double x, double y);  
    void drawSmallElipse(LPcmsCIExyY xyY, BYTE r, BYTE g, BYTE b, int sz);

    void paintEvent( QPaintEvent * );

private:

    void drawColorantTriangle();
    void drawWhitePoint();
    void drawPatches();
    
    void mapPoint(int& icx, int& icy, LPcmsCIExyY xyY);
    void biasedLine(int x1, int y1, int x2, int y2);
    void biasedText(int x, int y, QString Txt);

    void sweep_sRGB();

    void setProfile(cmsHPROFILE hProfile);

private slots:

    void slotBlinkTimerDone();

private :

    CIETongueWidgetPriv* d;

};

}  // namespace Digikam

#endif /* CIETONGUEWIDGET_H */
