/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLUGIN_IPODEXPORT_H
#define PLUGIN_IPODEXPORT_H


#include <libkipi/plugin.h>

class KAction;

class Plugin_iPodExport : public KIPI::Plugin
{
    Q_OBJECT

public:
    Plugin_iPodExport( QObject *parent, const char* name, const QStringList &args);
    ~Plugin_iPodExport() { }

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private slots:
    void slotImageUpload();

private:
    KAction         *m_actionImageUpload;
    KIPI::Interface *m_interface;
};

#endif //PLUGIN_IPODEXPORT_H
