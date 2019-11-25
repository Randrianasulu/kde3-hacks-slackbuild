/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KAMIXUAPP_H
#define KAMIXUAPP_H

#include <kuniqueapplication.h>

/**
KUniqueApplication subclass

@author Stefano Rivoir
*/
class KAMUApp : public KUniqueApplication
{
Q_OBJECT
public:
    KAMUApp() : KUniqueApplication() {};

    int newInstance();
};

#endif
