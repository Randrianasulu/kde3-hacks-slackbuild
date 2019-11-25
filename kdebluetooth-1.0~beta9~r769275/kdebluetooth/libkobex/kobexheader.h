/***************************************************************************
                          obexheader.h  -  description
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

#ifndef KOBEXHEADER_H
#define KOBEXHEADER_H

extern "C" {
	#include <openobex/obex.h>
}

class ObexObject;

class ObexHeader
{
public: 
	enum Header { None = 0,
				  Length = OBEX_HDR_LENGTH,
				  Name = OBEX_HDR_NAME,
				  Body = OBEX_HDR_BODY,
				  Who = OBEX_HDR_WHO,
				  MimeType = OBEX_HDR_TYPE};

	ObexHeader(Header, uint32_t, uint32_t);
	ObexHeader(Header, uint8_t, uint32_t);
	ObexHeader(Header, const uint8_t*, uint32_t);
	~ObexHeader();

    Header type();
    void data(uint32_t&, uint32_t&);
    void data(uint8_t&, uint32_t&);
    void data(const uint8_t*&, uint32_t&);

protected:
	ObexHeader(uint8_t, obex_headerdata_t, uint32_t);

	uint8_t hi;
	obex_headerdata_t hv;
	uint32_t hv_size;

friend class ObexObject;
};

#endif
