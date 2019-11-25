/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KPRESET_H
#define KPRESET_H

#include <qstring.h>

/**
@author Stefano Rivoir
*/
class KPreset {

private:
    KPreset();

public:
    ~KPreset();

public:
    static unsigned int count();
    static const QString volumesToString();
    static void loadPreset ( const QString& );
    static const QString preset(int);
    
};

#endif
