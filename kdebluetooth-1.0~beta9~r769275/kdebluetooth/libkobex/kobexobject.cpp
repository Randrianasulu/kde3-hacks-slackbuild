/***************************************************************************
                          obexobject.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
		From the original version of  James Stembridge <jstembridge@gmx.net>
    email                : simone.gotti@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kobexobject.h"

#include "kobexheader.h"

#include <qcstring.h>
#include <qmemarray.h>


ObexObject::ObexObject (Obex & o, Obex::Command cmd)
{
  obex = o.obex;
  object = OBEX_ObjectNew (obex, cmd);
}

ObexObject::ObexObject (obex_t * o, obex_object_t * obj)
{
  obex = o;
  object = obj;
}

ObexObject::~ObexObject ()
{
  // OBEX_ObjectDelete(obex, object);
}

void
ObexObject::addHeader (ObexHeader & hdr, addHeaderFlag f)
{
  OBEX_ObjectAddHeader (obex, object, hdr.hi, hdr.hv, hdr.hv_size, f);
}

ObexHeader *
ObexObject::getNextHeader ()
{
  uint8_t hi;
  obex_headerdata_t hv;
  uint32_t hv_size;

  if (OBEX_ObjectGetNextHeader (obex, object, &hi, &hv, &hv_size) != 0)
    {
      ObexHeader *hdr = new ObexHeader (hi, hv, hv_size);
      return hdr;
    }
  return 0;
}

int
ObexObject::readStream (QByteArray& buffer)
{

  const uint8_t *buf;
  QByteArray a;
	int size;
  size = OBEX_ObjectReadStream (obex, object, &buf);
	buffer.duplicate((char*)buf, size);
	return size;
}

int
ObexObject::enableReadStream ()
{

  if (OBEX_ObjectReadStream (obex, object, NULL) < 0);
  return FALSE;
  return TRUE;
}

void
ObexObject::setResponse (Obex::Response rsp, Obex::Response lastrsp)
{
  OBEX_ObjectSetRsp (object, int (rsp), int (lastrsp));
}

