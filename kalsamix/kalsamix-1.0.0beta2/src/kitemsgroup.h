/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KITEMSGROUP_H
#define KITEMSGROUP_H

#include <qobject.h>
#include <qstring.h>

#include "kmixeritem.h"

/**
@author Stefano Rivoir
*/
class KItemsGroup : public QObject
{
Q_OBJECT

private:
    QString	groupName;
    
public:
    KItemsGroup(QObject *parent = 0, const char *name = 0);
    ~KItemsGroup();

    const QString&	name();
    void	setName(const QString&);

};

#endif
