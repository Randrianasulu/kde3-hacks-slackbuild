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

#ifndef PINDIALOG_H
#define PINDIALOG_H

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <kapplication.h>
#include <qlabel.h>
#include <stdlib.h>
#include <time.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kdialogbase.h>

#include "application.h"
#include "pindefdialog.h"

/**
@author Fred Schaettgen
*/

class PinDialog : public KDialogBase
{

Q_OBJECT

public:
    PinDialog(const QString&, const QString&, PasskeyAgent *agent);
    ~PinDialog();
    PinDefaultDialog *pinDlg;

private:
    PasskeyAgent *mAgent;
	    
private slots:
    void sendPin();	

};

#endif
