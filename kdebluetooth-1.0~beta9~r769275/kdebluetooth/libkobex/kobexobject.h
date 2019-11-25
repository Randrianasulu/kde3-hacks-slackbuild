/***************************************************************************
                          obexobject.h  -  description
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

#ifndef KOBEXOBJECT_H
#define KOBEXOBJECT_H

#include "kobex.h"

class ObexHeader;

class ObexObject
{

public:
	ObexObject(Obex&, Obex::Command);
	~ObexObject();

	enum addHeaderFlag { None = 0,
						 FitOnePacket = OBEX_FL_FIT_ONE_PACKET,
						 StreamStart = OBEX_FL_STREAM_START,
						 StreamData = OBEX_FL_STREAM_DATA,
						 StreamEnd = OBEX_FL_STREAM_DATAEND };

	void addHeader(ObexHeader&, addHeaderFlag);
	ObexHeader* getNextHeader();
	int readStream(QByteArray&);
	int enableReadStream();
	void setResponse(Obex::Response, Obex::Response);

protected:
	ObexObject(obex_t*, obex_object_t*);

	obex_t *obex;
	obex_object_t *object;

friend class Obex;
};

#endif
