/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-28
 * Description : a dialog to display camera information.
 * 
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef CAMERAINFODIALOG_H
#define CAMERAINFODIALOG_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

namespace Digikam
{

class CameraInfoDialog : public KDialogBase
{
public:

    CameraInfoDialog(QWidget *parent, const QString& summary, const QString& manual,
                     const QString& about);
    ~CameraInfoDialog();
};

} // namespace Digikam

#endif /* CAMERAINFODIALOG_H */
