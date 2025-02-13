/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : Screenshot batch dialog
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) Richard J. Moore 1997-2002 from KSnapshot
 * Copyright (C) Matthias Ettrich 2000 from KSnapshot
 * Copyright (C) Aaron J. Seigo 2002 from KSnapshot
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef SCREENSHOOTDIALOG_H
#define SCREENSHOOTDIALOG_H

// Include files for Qt

#include <qimage.h>
#include <qpixmap.h>
#include <qtimer.h>

// Include files for KDE

#include <kdialogbase.h>

// Include files for X11

extern "C"
{
#include <X11/X.h>
#include <X11/Xlib.h>
}

// Include files for libKipi.

#include <libkipi/interface.h>

class QWidget;
class QCheckBox;
class QPushButton;

class KConfig;
class KIntNumInput;

namespace KIPIAcquireImagesPlugin
{

class AcquireImageDialog;

class ScreenGrabDialog : public KDialogBase
{
    Q_OBJECT

public:

    ScreenGrabDialog( KIPI::Interface* interface, 
                      QWidget *parent=0, const char *name=0);
    ~ScreenGrabDialog();

private slots:

    void slotHelp();
    void slotClose(void);
    void slotGrab(void);
    void slotPerformGrab(void);

private:
    
    bool eventFilter( QObject* o, QEvent* e);
    void endGrab(void);

private:

    KIPI::Interface        *m_interface;
    
    bool                    m_inSelect;
    
    QCheckBox              *m_desktopCB;
    QCheckBox              *m_hideCB;
    
    KIntNumInput           *m_delay;
    
    AcquireImageDialog     *m_acquireImageDialog;
    
    QImage                  m_screenshotImage;
    
    KConfig                *m_config;
    
    QWidget                *m_grabber;
    
    QTimer                  m_grabTimer;
    
    QPixmap                 m_snapshot;
    
    QPushButton            *m_helpButton;
    
    QValueList< QWidget* >  m_hiddenWindows;
};

}  // NameSpace KIPIAcquireImagesPlugin

#endif  // SCREENSHOOTDIALOG_H
