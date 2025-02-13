/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : lens distortion algorithm.
 * 
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2001-2003 by David Hodson <hodsond@acm.org>
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

// C++ includes.

#include <cmath>
#include <cstdlib>

// Local includes.

#include "dimg.h"
#include "ddebug.h"
#include "pixelaccess.h"
#include "lensdistortion.h"

namespace DigikamLensDistortionImagesPlugin
{

LensDistortion::LensDistortion(Digikam::DImg *orgImage, QObject *parent, double main, 
                               double edge, double rescale, double brighten,
                               int center_x, int center_y)
              : Digikam::DImgThreadedFilter(orgImage, parent, "LensDistortion")
{ 
    m_main     = main;
    m_edge     = edge;
    m_rescale  = rescale;
    m_brighten = brighten;
    m_centre_x = center_x;
    m_centre_y = center_y;
    
    initFilter();
}

void LensDistortion::filterImage(void)
{
    int    Width      = m_orgImage.width();
    int    Height     = m_orgImage.height();
    int    bytesDepth = m_orgImage.bytesDepth();

    uchar *data       = m_destImage.bits();

    // initial copy

    m_destImage.bitBltImage(&m_orgImage, 0, 0);

    // initialize coefficients

    double normallise_radius_sq = 4.0 / (Width * Width + Height * Height);
    double center_x             = Width * (100.0 + m_centre_x) / 200.0;
    double center_y             = Height * (100.0 + m_centre_y) / 200.0;
    double mult_sq              = m_main / 200.0;
    double mult_qd              = m_edge / 200.0;
    double rescale              = pow(2.0, - m_rescale / 100.0);
    double brighten             = - m_brighten / 10.0;

    PixelAccess *pa = new PixelAccess(&m_orgImage);

    /*
     * start at image (i, j), increment by (step, step)
     * output goes to dst, which is w x h x d in size
     * NB: d <= image.bpp
     */

    // We are working on the full image.
    int    dstWidth = Width;
    int    dstHeight = Height;
    uchar* dst = (uchar*)data;
    int    step = 1, progress;

    int    iLimit, jLimit;
    double srcX, srcY, mag;

    iLimit = dstWidth * step;
    jLimit = dstHeight * step;

    for (int dstJ = 0 ; !m_cancel && (dstJ < jLimit) ; dstJ += step) 
    {
        for (int dstI = 0 ; !m_cancel && (dstI < iLimit) ; dstI += step) 
        {
            // Get source Coordinates.
            double radius_sq;
            double off_x;
            double off_y;
            double radius_mult;

            off_x       = dstI - center_x;
            off_y       = dstJ - center_y;
            radius_sq   = (off_x * off_x) + (off_y * off_y);

            radius_sq  *= normallise_radius_sq;

            radius_mult = radius_sq * mult_sq + radius_sq * radius_sq * mult_qd;
            mag         = radius_mult;
            radius_mult = rescale * (1.0 + radius_mult);

            srcX        = center_x + radius_mult * off_x;
            srcY        = center_y + radius_mult * off_y;

            brighten = 1.0 + mag * brighten;
            pa->pixelAccessGetCubic(srcX, srcY, brighten, dst);
            dst += bytesDepth;
        }

       // Update progress bar in dialog.

        progress = (int) (((double)dstJ * 100.0) / jLimit);
        if (m_parent && progress%5 == 0)
            postProgress(progress);
    }

    delete pa;
}

}  // NameSpace DigikamLensDistortionImagesPlugin
