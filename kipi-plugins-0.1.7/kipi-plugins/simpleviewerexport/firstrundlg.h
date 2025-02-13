/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-01-06
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef FIRSTRUNDLG_H
#define FIRSTRUNDLG_H

// Include files for KDE

#include <kdialogbase.h>

// Include files for KIPI

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"

class QString;

class KURLRequester;

namespace KIPISimpleViewerExportPlugin
{

/**
 * To avoid licensing problems with some distributions, the SimpleViewer
 * Flash cannot be shipped with the plugin. During the first run of the
 * plugin, the user has to download SimpleViewer from its homepage and point
 * the plugin to that archive to install it. This is done by this dialog.
 */

class FirstRunDlg : public KDialogBase
{
    Q_OBJECT

public:
 
    FirstRunDlg(QWidget *parent=0);
    ~FirstRunDlg();

    /**
     * Returns the URL, where the SimpleViewer package is stored
     */
    QString getURL();

private slots:

    /**
     * Opens the browser with the SimpleViewer download page
     */
    void slotDownload(const QString &url);

    /**
     * Starts the installation of SimpleViewer
     */
    void slotURLSelected(const QString &url);

    void slotHelp();

private:

    QString                   m_url;

    KURLRequester            *m_urlRequester;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPISimpleViewerExportPlugin

#endif /* FIRSTRUNDLG_H */
