/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-12-02
 * Description : 8-16 bits color container.
 *
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DCOLOR_H
#define DCOLOR_H

// Qt includes.

#include <qcolor.h>

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DColor
{
public:

    /** Initialize with default value, fully transparent eight bit black */
    DColor()
        : m_red(0), m_green(0), m_blue(0), m_alpha(0), m_sixteenBit(false)
        {};

    /** Read value from data. Equivalent to setColor() */
    DColor(const uchar *data, bool sixteenBit = false)
        { setColor(data, sixteenBit); }

    /** Initialize with given RGBA values */
    DColor(int red, int green, int blue, int alpha, bool sixteenBit)
        : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha), m_sixteenBit(sixteenBit)
        {};

    /** Read values from QColor, convert to sixteenBit of sixteenBit is true */
    DColor(const QColor& color, bool sixteenBit=false);

    // Use default copy constructor, assignment operator and destructor

    /** Read color values as RGBA from the given memory location.
        If sixteenBit is false, 4 bytes are read.
        If sixteenBit is true, 8 bytes are read.
        Inline method.
     */
    void setColor(const uchar *data, bool sixteenBit = false);

    /** Write the values of this color to the given memory location.
        If sixteenBit is false, 4 bytes are written.
        If sixteenBit is true, 8 bytes are written.
        Inline method.
    */
    void setPixel(uchar *data) const;

    int  red  () const { return m_red; }
    int  green() const { return m_green; }
    int  blue () const { return m_blue; }
    int  alpha() const { return m_alpha; }
    bool sixteenBit() const { return m_sixteenBit; }

    void setRed  (int red)   { m_red = red; }
    void setGreen(int green) { m_green = green; }
    void setBlue (int blue)  { m_blue = blue; }
    void setAlpha(int alpha) { m_alpha = alpha; }
    void setSixteenBit(bool sixteenBit) { m_sixteenBit = sixteenBit; }

    QColor getQColor() const;

    /** Convert the color values of this color to and from sixteen bit
        and set the sixteenBit value accordingly
    */
    void convertToSixteenBit();
    void convertToEightBit();

    /** Premultiply and demultiply this color.
        DImg stores the color non-premultiplied.
        Inline methods.
    */
    void premultiply();
    void demultiply();

    /** Return the current RGB color values of this color
        in the HSL color space.
        Alpha is ignored for the conversion.
    */
    void getHSL(int* h, int* s, int* l) const;

    /** Set the RGB color values of this color
        to the given HSL values converted to RGB.
        Alpha is set to be fully opaque.
        sixteenBit determines both how the HSL values are interpreted
        and the sixteenBit value of this color after this operation.
    */
    void setRGB(int h, int s, int l, bool sixteenBit);

private:

    int  m_red;
    int  m_green;
    int  m_blue;
    int  m_alpha;

    bool m_sixteenBit;

public:

    // Inline alpha blending helper functions.
    // These functions are used by DColorComposer.
    // Look at that code to learn how to use them for
    // composition if you want to use them in optimized code.
    void blendZero();
    void blendAlpha8(int alpha);
    void blendInvAlpha8(int alpha);
    void blendAlpha16(int alpha);
    void blendInvAlpha16(int alpha);
    void premultiply16(int alpha);
    void premultiply8(int alpha);
    void demultiply16(int alpha);
    void demultiply8(int alpha);
    void blendAdd(const DColor &src);
    void blendClamp8();
    void blendClamp16();
};

}  // NameSpace Digikam


// Inline methods
#include "dcolorpixelaccess.h"
#include "dcolorblend.h"

#endif /* DCOLOR_H */
