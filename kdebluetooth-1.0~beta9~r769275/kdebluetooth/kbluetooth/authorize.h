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

#ifndef AUTHORIZE_H
#define AUTHORIZE_H

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

#include <libkbluetooth/service.h>

#include "application.h"
#include "authdialog.h"

class AuthorizeDialog : public KDialogBase
{

Q_OBJECT

public:
    AuthorizeDialog(const QString&, const QString&, const QString&, const QString &, AuthAgent *agent);
    ~AuthorizeDialog();
    AuthDialog *authDlg;

private:
    AuthAgent *mAgent;
    QString mService;
    QString mAddr;
	    
private slots:
    void sendAuthGrant();	
    void sendAuthReject();	
    void sendAuthAlwaysGrant();


};

#endif
