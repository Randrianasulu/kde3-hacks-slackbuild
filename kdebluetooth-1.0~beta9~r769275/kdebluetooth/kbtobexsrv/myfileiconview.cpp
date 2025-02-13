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

#include "myfileiconview.h"
#include "maindlg.h"


MyFileIconView::MyFileIconView(MainDialog* parent,
    QWidget* owner, const char* name) :
    KFileIconView(owner, name)
{
    this->parent = parent;
}


MyFileIconView::~MyFileIconView()
{
}

QDragObject* MyFileIconView::dragObject()
{
    return parent->createDragObject();
}


#include "myfileiconview.moc"
