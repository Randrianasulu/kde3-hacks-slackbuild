/*
 *
 *  Authorization dialog for kbluetooth
 *
 *  Copyright (C) 2006  Daniel Gollub <dgollub@suse.de>
 *
 *
 *  This file is part of kbluetooth.
 *
 *  kbluetooth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libkbluetooth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libkbluetooth; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "authorize.h"
#include "authdialog.h"

AuthorizeDialog::AuthorizeDialog(const QString &addr, const QString &devName, const QString &service, const QString& /*uuid*/, AuthAgent *agent) :
    KDialogBase(NULL, "authrequest", true, "Bluetooth Authorization Request", (User1|User3|User2), User2, false, KGuiItem(i18n("Accept"), "accept"), KGuiItem(i18n("Reject"), "reject"), KGuiItem(i18n("Always Accept"), "always") )
{

    kdDebug() << k_funcinfo << endl;	

    mAgent = agent;
    mService = service;
    mAddr = addr;

    QString action( i18n("wants to act as Input Device"));

    authDlg = new AuthDialog(this);
    authDlg->messageLabel->setText(authDlg->messageLabel->text().arg(devName).arg(addr).arg(action));

    connect(this, SIGNAL(user1Clicked()), SLOT(sendAuthGrant()));
    connect(this, SIGNAL(user2Clicked()), SLOT(sendAuthReject()));
    connect(this, SIGNAL(user3Clicked()), SLOT(sendAuthAlwaysGrant()));

    setMainWidget(authDlg);
}

AuthorizeDialog::~AuthorizeDialog()
{
	delete authDlg;
}

void AuthorizeDialog::sendAuthGrant()
{
	kdDebug() << k_funcinfo << endl;
	mAgent->sendAuth(true);
	close();
}

void AuthorizeDialog::sendAuthReject()
{
	kdDebug() << k_funcinfo << endl;
	mAgent->sendAuth(false);
	close();
}

void AuthorizeDialog::sendAuthAlwaysGrant()
{
	kdDebug() << k_funcinfo << endl;
	mAgent->sendAuth(true);
	mAgent->setAlwaysTrust(mAddr, mService);

	close();
}

#include "authorize.moc"
