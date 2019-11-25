/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-11
 * Description : a digiKam image editor plugin to correct 
 *               image white balance 
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEPLUGIN_WHITEBALANCE_H
#define IMAGEPLUGIN_WHITEBALANCE_H

// Digikam includes.

#include "imageplugin.h"
#include "digikam_export.h"

class KAction;

class DIGIKAMIMAGEPLUGINS_EXPORT ImagePlugin_WhiteBalance : public Digikam::ImagePlugin
{
    Q_OBJECT

public:

    ImagePlugin_WhiteBalance(QObject *parent, const char* name,
                             const QStringList &args);
    ~ImagePlugin_WhiteBalance();

    void setEnabledActions(bool enable);

private slots:

    void slotWhiteBalance();

private:

    KAction *m_whitebalanceAction;
};

#endif /* IMAGEPLUGIN_WHITEBALANCE_H */
