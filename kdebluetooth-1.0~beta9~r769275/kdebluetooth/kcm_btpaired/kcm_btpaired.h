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

#ifndef _KCM_BTPAIRED_H_
#define _KCM_BTPAIRED_H_

#include <kcmodule.h>
#include <kaboutdata.h>
#include <qguardedptr.h>

class KJanusWidget;
class ServiceTab;
class ScanTab;

class kcm_btpaired: public KCModule
{
    Q_OBJECT

public:
    kcm_btpaired( QWidget *parent=0, const char *name=0 );
    ~kcm_btpaired();

    virtual int buttons();
    virtual QString quickHelp() const;
    virtual const KAboutData *aboutData()
    { return myAboutData; };

private:
      KAboutData *myAboutData;
      QGuardedPtr<KJanusWidget> tabContainer;
      QGuardedPtr<PairedTab> pairedTab;
};

#endif
