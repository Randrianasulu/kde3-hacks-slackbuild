/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-04-07
 * Description : Raw camera list dialog
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef RAWCAMERADLG_H
#define RAWCAMERADLG_H

// KDE includes.

#include <kdialogbase.h>

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

class RawCameraDlgPriv;

class DIGIKAM_EXPORT RawCameraDlg : public KDialogBase
{
    Q_OBJECT

public:

    RawCameraDlg(QWidget* parent);
    ~RawCameraDlg();

private slots:

    void slotSearchTextChanged(const QString&);

private:

    RawCameraDlgPriv *d;
};

}  // NameSpace Digikam

#endif  // RAWCAMERADLG_H
