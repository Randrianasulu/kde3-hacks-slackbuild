/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LISTIMAGESERRORDIALOG_included
#define LISTIMAGESERRORDIALOG_included

// KDElib includes

#include <kdialogbase.h>

class KListView;
class KURL::List;

namespace KIPISendimagesPlugin
{

class listImagesErrorDialog : public KDialogBase
{
    Q_OBJECT

public:

    listImagesErrorDialog(QWidget* parent, QString Caption,
                          const QString &Mess1, const QString &Mess2,
                          KURL::List ListOfiles);

    ~listImagesErrorDialog();

private:

    KListView *m_listFiles;
};

}  // NameSpace KIPISendimagesPlugin

#endif  // LISTIMAGESERRORDIALOG_included
