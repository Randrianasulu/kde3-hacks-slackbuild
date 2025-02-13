    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

/*
 * BC - Status (2002-03-08): UnixConnection
 *
 * Not part of the public API. Do NOT use it in your apps. Can change, even
 * binary incompatible.
 */

#ifndef UNIXCONNECTION_H
#define UNIXCONNECTION_H

#include "socketconnection.h"

namespace Arts {

class UnixConnection :public SocketConnection {
public:
	UnixConnection(const std::string& url);
};

}
#endif /* UNIXCONNECTION_H */
