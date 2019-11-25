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

#include "procinheritsock.h"
#include <unistd.h>

KProcessInheritSocket::KProcessInheritSocket(int socket) :
    socket(socket)
{ 
}

int KProcessInheritSocket::commSetupDoneP()
{
    return KProcess::commSetupDoneP();
}

int KProcessInheritSocket::commSetupDoneC()
{
    // We close all file descriptors except std[in|out|err] and the
    // rfcomm socket we're passing to the new process.
    for (int n = ::sysconf(_SC_OPEN_MAX)-1; n >= 0; --n) {
        if (n!=STDERR_FILENO && n!=STDOUT_FILENO && n!=STDIN_FILENO &&
            // See commSetupDoneC docs. We need to keep these
            // fds open so that communications wit the process works.
            n!=in[0] && n!=out[1] && n!=err[1] &&
            n!=this->socket) {
            ::close(n);
        }
    }
    return KProcess::commSetupDoneC();
}


