/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-02-11
 * Description : a plugin to apply Distortion FX to an image.
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Original Distortion algorithms copyrighted 2004-2005 by
 * Pieter Z. Voloshyn <pieter dot voloshyn at gmail dot com>.
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

// KDE includes.

#include <klocale.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kaction.h>
#include <kcursor.h>

// Local includes.

#include "ddebug.h"
#include "distortionfxtool.h"
#include "imageplugin_distortionfx.h"
#include "imageplugin_distortionfx.moc"

using namespace DigikamDistortionFXImagesPlugin;

K_EXPORT_COMPONENT_FACTORY(digikamimageplugin_distortionfx,
                           KGenericFactory<ImagePlugin_DistortionFX>("digikamimageplugin_distortionfx"));

ImagePlugin_DistortionFX::ImagePlugin_DistortionFX(QObject *parent, const char*, const QStringList &)
                        : Digikam::ImagePlugin(parent, "ImagePlugin_DistortionFX")
{
    m_distortionfxAction = new KAction(i18n("Distortion Effects..."), "distortionfx", 0,
                               this, SLOT(slotDistortionFX()),
                               actionCollection(), "imageplugin_distortionfx");

    setXMLFile( "digikamimageplugin_distortionfx_ui.rc" );

    DDebug() << "ImagePlugin_DistortionFX plugin loaded" << endl;
}

ImagePlugin_DistortionFX::~ImagePlugin_DistortionFX()
{
}

void ImagePlugin_DistortionFX::setEnabledActions(bool enable)
{
    m_distortionfxAction->setEnabled(enable);
}

void ImagePlugin_DistortionFX::slotDistortionFX()
{
    DistortionFXTool *distortionfx = new DistortionFXTool(this);
    loadTool(distortionfx);
}
