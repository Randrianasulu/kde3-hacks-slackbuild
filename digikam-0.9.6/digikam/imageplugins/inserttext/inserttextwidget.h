/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-02-14
 * Description : a widget to insert a text over an image.
 * 
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef INSERTTEXTWIDGET_H
#define INSERTTEXTWIDGET_H

// Qt includes.

#include <qwidget.h>
#include <qimage.h>
#include <qrect.h>
#include <qsize.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>

// KDE includes.

#include <kurl.h>

// Digikam includes.

#include "dimg.h"

class QPixmap;

namespace Digikam
{
class ImageIface;
}

namespace DigikamInsertTextImagesPlugin
{

enum Action
{
    ALIGN_LEFT=0,
    ALIGN_RIGHT,
    ALIGN_CENTER,
    ALIGN_BLOCK,
    BORDER_TEXT,
    TRANSPARENT_TEXT
};

enum TextRotation
{
    ROTATION_NONE=0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270
};

enum BorderMode
{
    BORDER_NONE,
    BORDER_SUPPORT,
    BORDER_NORMAL
};

class InsertTextWidget : public QWidget
{
Q_OBJECT

public:

    InsertTextWidget(int w, int h, QWidget *parent=0);
    ~InsertTextWidget();

    Digikam::ImageIface* imageIface();
    Digikam::DImg        makeInsertText(void);

    void   setText(QString text, QFont font, QColor color, int alignMode,
                   bool border, bool transparent, int rotation);
    void   resetEdit(void);

    void  setPositionHint(QRect hint);
    QRect getPositionHint();

protected:

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

    void makePixmap(void);
    QRect composeImage(Digikam::DImg *image, QPainter *destPainter,
                       int x, int y,
                       QFont font, float pointSize, int textRotation, QColor textColor,
                       int alignMode, const QString &textString,
                       bool transparentBackground, QColor backgroundColor,
                       BorderMode borderMode, int borderWidth, int spacing);

private:

    bool                 m_currentMoving;
    bool                 m_textBorder;
    bool                 m_textTransparent;

    int                  m_alignMode;
    int                  m_textRotation;

    uchar               *m_data;
    int                  m_w;
    int                  m_h;

    int                  m_xpos;
    int                  m_ypos;
    
    int                  m_transparency;

    QPixmap             *m_pixmap;

    QRect                m_rect;
    QRect                m_textRect;

    QString              m_textString;

    QFont                m_textFont;

    QColor               m_textColor;

    QColor               m_backgroundColor;

    QRect                m_positionHint;

    Digikam::ImageIface *m_iface;
};

}  // NameSpace DigikamInsertTextImagesPlugin

#endif /* INSERTTEXTWIDGET_H */
