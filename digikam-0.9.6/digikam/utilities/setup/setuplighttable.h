/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 * 
 * Date        : 2007-05-11
 * Description : setup Light Table tab.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETUPLIGHTTABLE_H
#define SETUPLIGHTTABLE_H

// Qt includes.

#include <qwidget.h>

namespace Digikam
{

class SetupLightTablePriv;

class SetupLightTable : public QWidget
{
    Q_OBJECT

public:

    SetupLightTable(QWidget* parent = 0);
    ~SetupLightTable();

    void applySettings();

private:

    void readSettings();

private:

    SetupLightTablePriv* d;
};

}  // namespace Digikam

#endif // SETUPLIGHTTABLE_H
