/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 * 
 * Date        : 2006-01-23
 * Description : setup image editor output files settings.
 * 
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETUPIOFILES_H
#define SETUPIOFILES_H

// Qt includes.

#include <qwidget.h>

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

class SetupIOFilesPriv;

class DIGIKAM_EXPORT SetupIOFiles : public QWidget
{
    Q_OBJECT
    
public:

    SetupIOFiles(QWidget* parent = 0);
    ~SetupIOFiles();

    void applySettings();

private:

    void readSettings();

private:

    SetupIOFilesPriv* d;    
};

}  // namespace Digikam

#endif // SETUPIOFILES_H 
