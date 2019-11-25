//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROCINHERITSOCK_H
#define PROCINHERITSOCK_H

#include <kprocess.h>

/**
This class differs from kprocess only in
that it closes all open sockets except for 
stdin, stdout, stderr and one other socket
before executing in the new process.
@author Fred Schaettgen
*/
class KProcessInheritSocket : public KProcess
{
    int socket;
public:
    KProcessInheritSocket(int socket);
protected:
    virtual int commSetupDoneP();
    virtual int commSetupDoneC();
};


#endif
