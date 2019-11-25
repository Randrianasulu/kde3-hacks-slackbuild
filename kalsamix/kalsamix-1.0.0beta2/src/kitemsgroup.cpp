/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kitemsgroup.h"

KItemsGroup::KItemsGroup(QObject *parent, const char *name)
 : QObject(parent, name)
{
    groupName = "";
}

KItemsGroup::~KItemsGroup()
{
}

void KItemsGroup::setName(const QString& n) {
    groupName = n;
}

const QString& KItemsGroup::name() {
    return groupName;
}

#include "kitemsgroup.moc"
