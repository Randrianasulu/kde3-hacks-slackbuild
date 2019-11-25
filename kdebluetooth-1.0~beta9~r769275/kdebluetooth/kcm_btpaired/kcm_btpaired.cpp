/*
 *
 *  KDE Control Center Module for managing Bluetooth linkkeys 
 *
 *  Copyright (C) 2003  Fred Schaettgen <kdebluetooth@schaettgen.de>
 *  Copyright (C) 2006  Daniel Gollub <dgollub@suse.de>
 *
 *
 *  This file is part of kcm_btpaired.
 *
 *  libkbluetooth is free software; you can redistribute it and/or modify
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

#include <qlayout.h>

#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kjanuswidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <dcopclient.h>
#include "pairedtab.h"

#include "kcm_btpaired.h"

kcm_btpaired::kcm_btpaired(QWidget *parent, const char *name)
    : KCModule(parent, name)
{
	KLocale::setMainCatalogue("kdebluetooth");

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	tabContainer = new KJanusWidget(this, "tabcontainer", KJanusWidget::Tabbed);
	mainLayout->addWidget(tabContainer);

	QVBox* serviceBox = tabContainer->addVBoxPage(i18n("Paired/Trusted Devices"));
	pairedTab = new PairedTab(serviceBox, "pairedtab");

	KConfig* config = instance()->config();
	config->setGroup("kcm_btpaired_global");
	tabContainer->showPage(config->readNumEntry("currentTab", 0));

	this->setButtons(Ok|Help);
}

kcm_btpaired::~kcm_btpaired()
{
    KConfig* config = instance()->config();
    config->setGroup("kcm_btpaired_global");
    config->writeEntry("currentTab", tabContainer->activePageIndex());    
}


int kcm_btpaired::buttons()
{
    return KCModule::Help;
}

QString kcm_btpaired::quickHelp() const
{
    return i18n("<h1>BtPaired</h1>\
With this module you can manage the list of paired Bluetooth Devices.");
}


extern "C"
{
    KCModule *create_btpaired(QWidget *parent, const char* /*name*/)
    {
        KGlobal::locale()->insertCatalogue("btpaired");
        return new kcm_btpaired(parent, "btpaired");
    }
}

#include "kcm_btpaired.moc"
