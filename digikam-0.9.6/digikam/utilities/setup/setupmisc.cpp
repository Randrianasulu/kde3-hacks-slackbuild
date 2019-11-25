/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-23
 * Description : mics configuration setup tab
 * 
 * Copyright (C) 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <qlayout.h>
#include <qvgroupbox.h>
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>

// Local includes.

#include "albumsettings.h"
#include "setupmisc.h"

namespace Digikam
{

class SetupMiscPriv
{
public:

    SetupMiscPriv()
    {
        showSplashCheck            = 0;
        showTrashDeleteDialogCheck = 0;
        sidebarApplyDirectlyCheck  = 0;
        scanAtStart                = 0;
    }

    QCheckBox* showSplashCheck;
    QCheckBox* showTrashDeleteDialogCheck;
    QCheckBox* sidebarApplyDirectlyCheck;
    QCheckBox* scanAtStart;
};

SetupMisc::SetupMisc(QWidget* parent)
         : QWidget( parent )
{
    d = new SetupMiscPriv;

    QVBoxLayout *mainLayout = new QVBoxLayout(parent);
    QVBoxLayout *layout = new QVBoxLayout( this, 0, KDialog::spacingHint() );

   // --------------------------------------------------------

   d->showTrashDeleteDialogCheck = new QCheckBox(i18n("Show confirmation dialog when moving items to the &trash"), this);
   layout->addWidget(d->showTrashDeleteDialogCheck);

   // --------------------------------------------------------

   d->sidebarApplyDirectlyCheck = new QCheckBox(i18n("Apply changes in the &right sidebar without confirmation"), this);
   layout->addWidget(d->sidebarApplyDirectlyCheck);

   // --------------------------------------------------------

   d->showSplashCheck = new QCheckBox(i18n("&Show splash screen at startup"), this);
   layout->addWidget(d->showSplashCheck);

   // --------------------------------------------------------

   d->scanAtStart = new QCheckBox(i18n("&Scan for new items on startup (slows down startup)"), this);
   layout->addWidget(d->scanAtStart);

   // --------------------------------------------------------

   layout->addStretch();
   readSettings();
   adjustSize();
   mainLayout->addWidget(this);
}

SetupMisc::~SetupMisc()
{
    delete d;
}

void SetupMisc::applySettings()
{
    AlbumSettings* settings = AlbumSettings::instance();

    settings->setShowSplashScreen(d->showSplashCheck->isChecked());
    settings->setShowTrashDeleteDialog(d->showTrashDeleteDialogCheck->isChecked());
    settings->setApplySidebarChangesDirectly(d->sidebarApplyDirectlyCheck->isChecked());
    settings->setScanAtStart(d->scanAtStart->isChecked());
    settings->saveSettings();
}

void SetupMisc::readSettings()
{
    AlbumSettings* settings = AlbumSettings::instance();

    d->showSplashCheck->setChecked(settings->getShowSplashScreen());
    d->showTrashDeleteDialogCheck->setChecked(settings->getShowTrashDeleteDialog());
    d->sidebarApplyDirectlyCheck->setChecked(settings->getApplySidebarChangesDirectly());
    d->scanAtStart->setChecked(settings->getScanAtStart());
}

}  // namespace Digikam
