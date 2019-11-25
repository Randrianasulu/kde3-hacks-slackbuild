/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-26
 * Description : a digiKam image editor plugin to emboss 
 *               an image.
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
#include "embosstool.h"
#include "imageplugin_emboss.h"
#include "imageplugin_emboss.moc"

using namespace DigikamEmbossImagesPlugin;

K_EXPORT_COMPONENT_FACTORY(digikamimageplugin_emboss,
                           KGenericFactory<ImagePlugin_Emboss>("digikamimageplugin_emboss"));

ImagePlugin_Emboss::ImagePlugin_Emboss(QObject *parent, const char*,
                                       const QStringList &)
                  : Digikam::ImagePlugin(parent, "ImagePlugin_Emboss")
{
    m_embossAction = new KAction(i18n("Emboss..."), "embosstool", 0, 
                         this, SLOT(slotEmboss()),
                         actionCollection(), "imageplugin_emboss");

    setXMLFile( "digikamimageplugin_emboss_ui.rc" );

    DDebug() << "ImagePlugin_Emboss plugin loaded" << endl;
}

ImagePlugin_Emboss::~ImagePlugin_Emboss()
{
}

void ImagePlugin_Emboss::setEnabledActions(bool enable)
{
    m_embossAction->setEnabled(enable);
}

void ImagePlugin_Emboss::slotEmboss()
{
    EmbossTool *tool = new EmbossTool(this);
    loadTool(tool);
}
