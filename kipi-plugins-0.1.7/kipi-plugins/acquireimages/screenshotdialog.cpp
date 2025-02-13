/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : Screenshot batch dialog
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qlayout.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qwidgetlist.h>

// Include files for KDE

#include <klocale.h>
#include <knotifyclient.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

// Local includes

#include "pluginsversion.h"
#include "screenshotdialog.h"
#include "acquireimagedialog.h"
#include "screenshotdialog.moc"

namespace KIPIAcquireImagesPlugin
{

ScreenGrabDialog::ScreenGrabDialog( KIPI::Interface* interface, QWidget *parent, const char *name)
                : KDialogBase(parent, name, false, i18n("Screenshot"),
                              Help|User1|Close, Close, false, i18n("&New Snapshot")),
                  m_interface( interface )
{
    m_inSelect = false;
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *layout = new QVBoxLayout(box);

    //---------------------------------------------
   
    QLabel *label1 = new QLabel(i18n("This dialog will grab either your desktop or a single\n"
                                     "application window. If you grab a single window your mouse\n"
                                     "cursor will change into crosshairs; then, simply select the\n"
                                     "window with your mouse."), box);
    layout->addWidget(label1);

    //---------------------------------------------

    m_desktopCB = new QCheckBox(i18n("Grab the entire desktop"), box);
    QWhatsThis::add( m_desktopCB, i18n( "<p>If you enable this option, the entire desktop will be grabbed; "
                                        "otherwise, only the active windows." ) );
    layout->addWidget(m_desktopCB);

    //---------------------------------------------

    m_hideCB = new QCheckBox(i18n("Hide all host application windows"), box);
    QWhatsThis::add( m_hideCB, i18n( "<p>If you enable this option, all host application windows will be hidden "
                                     "during the grab operation." ) );
    layout->addWidget(m_hideCB);

    //---------------------------------------------

    QLabel *label2 = new QLabel(i18n("Delay:"), box);
    layout->addWidget(label2);
    m_delay = new KIntNumInput(box);
    QWhatsThis::add( m_delay, i18n( "<p>The delay in seconds before the grab operation is started.") );
    m_delay->setRange(0, 60);
    layout->addWidget(m_delay);
    layout->addStretch(1);

    //---------------------------------------------

    m_grabber = new QWidget( 0, 0, WStyle_Customize | WX11BypassWM );
    m_grabber->move( -4000, -4000 );
    m_grabber->installEventFilter( this );

    //---------------------------------------------

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotGrab()));
    
    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect( &m_grabTimer, SIGNAL(timeout()),
             this, SLOT(slotPerformGrab()));

    //---------------------------------------------

    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("ScreenshotImages Settings");

    if (m_config->readEntry("GrabDesktop", "true") == "true")
        m_desktopCB->setChecked( true );
    else
        m_desktopCB->setChecked( false );

    if (m_config->readEntry("HideHostWin", "true") == "true")
        m_hideCB->setChecked( true );
    else
        m_hideCB->setChecked( false );

    m_delay->setValue(m_config->readNumEntry("Delay", 1));

    delete m_config;
    
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Acquire images"), 
                                       kipiplugins_version,
                                       I18N_NOOP("A Kipi plugin to acquire images"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2008, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
}

ScreenGrabDialog::~ScreenGrabDialog()
{
}

void ScreenGrabDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("acquireimages", "kipi-plugins");
}    

void ScreenGrabDialog::slotClose( void )
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("ScreenshotImages Settings");
    m_config->writeEntry("GrabDesktop", m_desktopCB->isChecked());
    m_config->writeEntry("HideHostWin", m_hideCB->isChecked());
    m_config->writeEntry("Delay", m_delay->value());
    m_config->sync();
    delete m_config;

    close();
    delete this;
}

void ScreenGrabDialog::slotGrab()
{
    hide();

    // Hiding the Host windows
    m_hiddenWindows.clear();
    if (m_hideCB->isChecked())
    {
        QWidgetList  *list = QApplication::topLevelWidgets();
        QWidgetListIt it( *list );
        QWidget * w;
        while ( (w=it.current()) != 0 )
        {
            ++it;
            if ( w->isVisible())
            {
                m_hiddenWindows.append( w );
                w->hide();
            }
        }
        delete list;
    }

    kapp->processEvents();
    QApplication::syncX();

    if ( m_delay->value() != 0 )
        m_grabTimer.start( m_delay->value() * 1000, true );
    else
    {
        m_grabber->show();
        m_grabber->grabMouse( crossCursor );
    }
}

void ScreenGrabDialog::slotPerformGrab()
{
    m_grabber->releaseMouse();
    m_grabber->hide();
    m_grabTimer.stop();

    if ( m_desktopCB->isChecked() == false )
    {
        Window root;
        Window child;
        uint mask;
        int rootX, rootY, winX, winY;
        XQueryPointer( qt_xdisplay(), qt_xrootwin(), &root, &child,
                       &rootX, &rootY, &winX, &winY,
                       &mask);

        int x, y;
        unsigned int w, h;
        unsigned int border;
        unsigned int depth;
        XGetGeometry( qt_xdisplay(), child, &root, &x, &y,
                      &w, &h, &border, &depth );

        m_snapshot = QPixmap::grabWindow( qt_xrootwin(), x, y, w, h );
    }
    else
        m_snapshot = QPixmap::grabWindow( qt_xrootwin() );

    if (m_snapshot.isNull())
    {
       KMessageBox::sorry(this, i18n("Unable to take snapshot."),
                          i18n("Screenshot Error"));

       endGrab();
       return;
    }

    QApplication::restoreOverrideCursor();
    KNotifyClient::beep();

    m_screenshotImage = m_snapshot.convertToImage();
    m_acquireImageDialog = new AcquireImageDialog( m_interface, this, m_screenshotImage);
    m_acquireImageDialog->setMinimumWidth(400);
    m_acquireImageDialog->exec();

    endGrab();
}

void ScreenGrabDialog::endGrab(void)
{
    // Restore the Host windows

    if (m_hideCB->isChecked())
    {
       for( QValueList< QWidget* >::ConstIterator it = m_hiddenWindows.begin();
            it != m_hiddenWindows.end();
            ++it )
           (*it)->show();
       QApplication::syncX();
    }

    show();
}

bool ScreenGrabDialog::eventFilter( QObject* o, QEvent* e)
{
    if ( o == m_grabber && e->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* me = (QMouseEvent*) e;

        if ( QWidget::mouseGrabber() != m_grabber )
           return false;

        if ( me->button() == LeftButton )
           slotPerformGrab();
    }

    return false;
}

}  // NameSpace KIPIAcquireImagesPlugin
