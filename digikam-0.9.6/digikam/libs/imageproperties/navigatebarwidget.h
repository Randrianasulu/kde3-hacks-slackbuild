/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a navigate bar with text 
 *
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef NAVIGATEBARWIDGET_H
#define NAVIGATEBARWIDGET_H

// Qt includes.

#include <qwidget.h>
#include <qstring.h>

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

class NavigateBarWidgetPriv;

class DIGIKAM_EXPORT NavigateBarWidget : public QWidget
{
Q_OBJECT

public:

    NavigateBarWidget(QWidget *parent=0, bool show=true);
    ~NavigateBarWidget();
    
    void    setFileName(QString filename=QString());
    QString getFileName();
    void    setButtonsState(int itemType);
    int     getButtonsState();

signals:
    
    void signalFirstItem(void);    
    void signalPrevItem(void);    
    void signalNextItem(void);    
    void signalLastItem(void);    
        
private :

    NavigateBarWidgetPriv* d;    
};

}  // namespace Digikam

#endif /* NAVIGATEBARWIDGET_H */
