/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-01
 * Description : dialog to edit and create digiKam Tags
 * 
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TAGEDITDLG_H
#define TAGEDITDLG_H

// Qt includes.

#include <qmap.h>
#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

// Local includes.

#include "albummanager.h"

namespace Digikam
{
class TagEditDlgPriv;

class TagEditDlg : public KDialogBase
{
    Q_OBJECT

public:

    TagEditDlg(QWidget *parent, TAlbum* album, bool create=false);
    ~TagEditDlg();

    QString title() const;
    QString icon() const;

    static bool tagEdit(QWidget *parent, TAlbum* album, QString& title, QString& icon);
    static bool tagCreate(QWidget *parent, TAlbum* album, QString& title, QString& icon);

    /** Create a list of new Tag album using a list of tags hierarchies separated by ",".
        A hierarchy of tags is a string path of tags name separated by "/".
        If a hierarchy start by "/" or if mainRootAlbum is null, it will be created from 
        root tag album, else it will be created from mainRootAlbum as parent album.
        'errMap' is Map of TAlbum path and error message if tag creation failed.
        Return the list of created Albums.
    */
    static AlbumList createTAlbum(TAlbum *mainRootAlbum, const QString& tagStr, const QString& icon,
                                  QMap<QString, QString>& errMap);

    static void showtagsListCreationError(QWidget* parent, const QMap<QString, QString>& errMap);

private slots:

    void slotIconChanged();
    void slotIconResetClicked();
    void slotTitleChanged(const QString& newtitle);

private:

    TagEditDlgPriv *d;
};

}  // namespace Digikam

#endif /* TAGEDITDLG_H */
