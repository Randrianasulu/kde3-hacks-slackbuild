/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a digiKam image editor plugin for add film 
 *               grain on an image.
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

// KDE includes.

#include <klocale.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kaction.h>
#include <kcursor.h>
#include <kapplication.h>

// Local includes.

#include "ddebug.h"
#include "filmgraintool.h"
#include "imageplugin_filmgrain.h"
#include "imageplugin_filmgrain.moc"

using namespace DigikamFilmGrainImagesPlugin;

K_EXPORT_COMPONENT_FACTORY(digikamimageplugin_filmgrain,
                           KGenericFactory<ImagePlugin_FilmGrain>("digikamimageplugin_filmgrain"));

ImagePlugin_FilmGrain::ImagePlugin_FilmGrain(QObject *parent, const char*, const QStringList &)
                     : Digikam::ImagePlugin(parent, "ImagePlugin_FilmGrain")
{
    m_filmgrainAction = new KAction(i18n("Add Film Grain..."), "filmgrain", 0, 
                            this, SLOT(slotFilmGrain()),
                            actionCollection(), "imageplugin_filmgrain");

    setXMLFile( "digikamimageplugin_filmgrain_ui.rc" );

    DDebug() << "ImagePlugin_FilmGrain plugin loaded" << endl;
}

ImagePlugin_FilmGrain::~ImagePlugin_FilmGrain()
{
}

void ImagePlugin_FilmGrain::setEnabledActions(bool enable)
{
    m_filmgrainAction->setEnabled(enable);
}

void ImagePlugin_FilmGrain::slotFilmGrain()
{
    FilmGrainTool *tool = new FilmGrainTool(this);
    loadTool(tool);
}
