/***************************************************************************
                          obexheader.cpp  -  description
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

#include "kobexheader.h"

ObexHeader::ObexHeader (Header h, uint32_t bq4, uint32_t size)
{
  hi = h;
  hv.bq4 = bq4;
  hv_size = size;
}

ObexHeader::ObexHeader (Header h, uint8_t bq1, uint32_t size)
{
  hi = h;
  hv.bq1 = bq1;
  hv_size = size;
}

ObexHeader::ObexHeader (Header h, const uint8_t * bs, uint32_t size)
{
  hi = h;
  hv.bs = bs;
  hv_size = size;
}

ObexHeader::ObexHeader (uint8_t h, obex_headerdata_t hd, uint32_t size)
{
  hi = h;
  hv = hd;
  hv_size = size;
}

ObexHeader::Header ObexHeader::type ()
{
  return Header (hi);
}

void
ObexHeader::data (uint32_t & bq4, uint32_t & size)
{
  bq4 = hv.bq4;
  size = hv_size;
}

void
ObexHeader::data (uint8_t & bq1, uint32_t & size)
{
  bq1 = hv.bq1;
  size = hv_size;
}

void
ObexHeader::data (const uint8_t * &bs, uint32_t & size)
{
  bs = hv.bs;
  size = hv_size;
}

ObexHeader::~ObexHeader ()
{
}


