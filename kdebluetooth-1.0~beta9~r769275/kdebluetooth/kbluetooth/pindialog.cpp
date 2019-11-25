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

#include "pindialog.h"
#include "pindefdialog.h"

//#include "dbuspasskeyagent.h"

PinDialog::PinDialog(const QString &addr, const QString &devName, PasskeyAgent *agent) :
    KDialogBase(NULL, "pinrequest", true, "Pin Request", (Ok|Cancel))
{

    kdDebug() << k_funcinfo << endl;	

    mAgent = agent;

    pinDlg = new PinDefaultDialog(this);
    pinDlg->messageLabel->setText(pinDlg->messageLabel->text().arg(devName).arg(addr));

    pinDlg->pinEdit->setFocus();

    connect(this, SIGNAL (okClicked()), SLOT(sendPin())); 

    setMainWidget(pinDlg);
}

PinDialog::~PinDialog()
{
	delete pinDlg;
}

void PinDialog::sendPin()
{
	QString pin = pinDlg->pinEdit->text();
	mAgent->sendPasskey(pin);
}

#include "pindialog.moc"
