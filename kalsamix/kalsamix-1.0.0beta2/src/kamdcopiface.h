/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KAMIXDCOPIFACE_H
#define KAMIXDCOPIFACE_H

#include <dcopobject.h>
#include "kamdcopiface.skel"

/**
@author Stefano Rivoir
*/
class KAMDCOPIface : virtual public DCOPObject {

K_DCOP
k_dcop:

	virtual void masterVolUp(int) = 0;
	virtual void masterVolDown(int) = 0;
	virtual void masterMute() = 0;
	virtual void masterShow() = 0;
	virtual void quitKAlsaMix() = 0;
};

#endif
