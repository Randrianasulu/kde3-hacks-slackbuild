//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MYFILEICONVIEW_H
#define MYFILEICONVIEW_H

#include <kfileiconview.h>

class MainDialog;

class MyFileIconView : public KFileIconView
{
Q_OBJECT
public:
    MyFileIconView(MainDialog* parent, QWidget* owner, const char* name);
    ~MyFileIconView();
protected:
    MainDialog* parent;
    virtual QDragObject* dragObject();
};

#endif
