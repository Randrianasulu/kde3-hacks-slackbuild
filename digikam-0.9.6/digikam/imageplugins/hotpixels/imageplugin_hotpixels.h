/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : a digiKam image plugin for fixing dots produced by
 *               hot/stuck/dead pixels from a CCD.
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef IMAGEPLUGIN_HOTPIXELS_H
#define IMAGEPLUGIN_HOTPIXELS_H

// Digikam includes.

#include "imageplugin.h"
#include "digikam_export.h"

class DIGIKAMIMAGEPLUGINS_EXPORT ImagePlugin_HotPixels : public Digikam::ImagePlugin
{
    Q_OBJECT

public:

    ImagePlugin_HotPixels(QObject *parent, const char* name,
                          const QStringList &args);
    ~ImagePlugin_HotPixels();

    void setEnabledActions(bool enable);

private slots:

    void slotHotPixels();

private:

    KAction *m_hotpixelsAction;
};

#endif /* IMAGEPLUGIN_HOTPIXELS_H */
