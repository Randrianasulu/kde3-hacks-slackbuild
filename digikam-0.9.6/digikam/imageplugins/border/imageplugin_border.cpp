/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-01-20
 * Description : a digiKam image plugin to add a border
 *               around an image.
 *
 * Copyright 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "bordertool.h"
#include "imageplugin_border.h"
#include "imageplugin_border.moc"

using namespace DigikamBorderImagesPlugin;

K_EXPORT_COMPONENT_FACTORY(digikamimageplugin_border,
                           KGenericFactory<ImagePlugin_Border>("digikamimageplugin_border"));

ImagePlugin_Border::ImagePlugin_Border(QObject *parent, const char*, const QStringList &)
                  : Digikam::ImagePlugin(parent, "ImagePlugin_Border")
{
    m_borderAction = new KAction(i18n("Add Border..."), "bordertool", 
                         0, 
                         this, SLOT(slotBorder()),
                         actionCollection(), "imageplugin_border");

    setXMLFile("digikamimageplugin_border_ui.rc");

    DDebug() << "ImagePlugin_Border plugin loaded" << endl;
}

ImagePlugin_Border::~ImagePlugin_Border()
{
}

void ImagePlugin_Border::setEnabledActions(bool enable)
{
    m_borderAction->setEnabled(enable);
}

void ImagePlugin_Border::slotBorder()
{
    BorderTool *tool = new BorderTool(this);
    loadTool(tool);
}
