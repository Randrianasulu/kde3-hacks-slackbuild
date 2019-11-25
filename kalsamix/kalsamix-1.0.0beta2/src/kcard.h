/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KCARD_H
#define KCARD_H

#include <qwidget.h>
#include <qstring.h>

#include "kmixer.h"

class KMixer;

/**
@author Stefano Rivoir
*/
class KCard : public QWidget
{
Q_OBJECT
public:
    KCard(QString &devname, QWidget *parent = 0, const char *name = 0);
    ~KCard();

    const QString& name();
    const QString& deviceName();
    const QString& configSection();
    
    KMixer*  mixer();
    
    void setName( QString & );
    bool openMixer();
    
    void writeConfig();

private:
    QString _configSection;
    KMixer *_mixer;
    QString _name;
    QString _devname;

};

#endif
