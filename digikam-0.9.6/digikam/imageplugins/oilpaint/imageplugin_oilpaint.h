/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-25
 * Description : a plugin to simulate Oil Painting
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEPLUGIN_OILPAINT_H
#define IMAGEPLUGIN_OILPAINT_H

// Digikam includes.

#include "imageplugin.h"
#include "digikam_export.h"

class KAction;

class DIGIKAMIMAGEPLUGINS_EXPORT ImagePlugin_OilPaint : public Digikam::ImagePlugin
{
    Q_OBJECT

public:

    ImagePlugin_OilPaint(QObject *parent, const char* name,
                         const QStringList &args);
    ~ImagePlugin_OilPaint();

    void setEnabledActions(bool enable);

private slots:

    void slotOilPaint();

private:

    KAction *m_oilpaintAction;
};

#endif /* IMAGEPLUGIN_OILPAINT_H */
