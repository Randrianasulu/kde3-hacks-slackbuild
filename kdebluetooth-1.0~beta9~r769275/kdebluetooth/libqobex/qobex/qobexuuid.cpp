/*
    This file is part of libqobex.

    Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>

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
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <qcstring.h>

#include "qobexuuid.h"

namespace {

  /* Folder Browsing service UUID */
  /* F9EC7BC4-953C-11D2-984E-525400DC9E09 */
  QByteArray initUuidFbs() {
    QByteArray ret;
    return ret.duplicate( "\xF9\xEC\x7B\xC4\x95\x3C\x11\xD2\x98\x4E\x52\x54\x00\xDC\x9E\x09", 16 );
  }

  /* UUID for the Telecom/IrMC Syncronisation service. */
  QByteArray initUuidIrMCSync() {
    QByteArray ret;
    return ret.duplicate( "IRMC-SYNC", 9 );
  }

  /* SyncML service UUID */
  /* 00000002-0000-1000-8000-0002EE000002 */
  QByteArray initUuidSyncML() {
    QByteArray ret;
    return ret.duplicate( "\x00\x00\x00\x02\x00\x00\x10\x00\x80\x00\x00\x02\xEE\x00\x00\x02", 16 );
  }

  /* SyncMLSync service UUID */
  /* SYNCML-SYNC */
  QByteArray initUuidSyncMLSync() {
    QByteArray ret;
    return ret.duplicate( "SYNCML-SYNC", 11 );
  }

  /* SyncMLDM service UUID */
  /* SYNCML-DM */
  QByteArray initUuidSyncMLDM() {
    QByteArray ret;
    return ret.duplicate( "SYNCML-DM", 9 );
  }

  /* UUID for Siemens S45 and maybe others too */
  /* binary representation of 6B01CB31-4106-11D4-9A77-0050DA3F471F */
  QByteArray initUuidSiemensMobile() {
    QByteArray ret;
    return ret.duplicate( "\x6B\x01\xCB\x31\x41\x06\x11\xD4\x9A\x77\x00\x50\xDA\x3F\x47\x1F", 16 );
  }

}

const QByteArray QObexUuidFBS = initUuidFbs();
const QByteArray QObexUuidIrMCSync = initUuidIrMCSync();
const QByteArray QObexUuidSyncML = initUuidSyncML();
const QByteArray QObexUuidSyncMLSync = initUuidSyncMLSync();
const QByteArray QObexUuidSyncMLDM = initUuidSyncMLDM();
const QByteArray QObexUuidSiemensMobile = initUuidSiemensMobile();
