/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : preview raw file widget used in single convert
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */


#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

// Qt includes.

#include <qcolor.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>

class QPaintEvent;
class QResizeEvent;

namespace KIPIRawConverterPlugin
{
class PreviewWidgetPriv;

class PreviewWidget : public QFrame
{
    Q_OBJECT
    
public:

    PreviewWidget(QWidget *parent);
    ~PreviewWidget();

    void load(const QString& file);
    void setInfo(const QString& text, const QColor& color=Qt::white, 
                 const QPixmap& preview=QPixmap());

protected:

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:

    void slotResize();
    
private:

    PreviewWidgetPriv* d;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* PREVIEWWIDGET_H */
