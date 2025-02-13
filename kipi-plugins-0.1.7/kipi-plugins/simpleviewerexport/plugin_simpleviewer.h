/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
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

#ifndef PLUGIN_SIMPLEVIEWER_H
#define PLUGIN_SIMPLEVIEWER_H

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

class Plugin_SimpleViewer : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_SimpleViewer(QObject *parent, const char* name, const QStringList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private slots:

    void slotActivate();

private:

    KAction         *m_actionSimpleViewer;
    KIPI::Interface *m_interface;
};

#endif // PLUGIN_SIMPLEVIEWER_H

