/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kmainwindow.h>
#include <klocale.h>
#include <kapplication.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kactionclasses.h>
#include <kaboutapplication.h>
#include <kmessagebox.h>
#include <dcopclient.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <ksplashscreen.h>
#include <kstandarddirs.h>

#include "kpreset.h"
#include "kmixer.h"
#include "kalsamix.h"
#include "kconfigwin.h"
#include "kmixeritem.h"
#include "kmasterview.h"
#include "kvumeter.h"
#include "kosd.h"
#include "kamconfig.h"
#include "kalsa.h"
#include "config.h"

#ifdef VUMETER
#include <arts/kartsdispatcher.h>
#include <arts/kartsserver.h>
#include <arts/artsflow.h>
#endif

kalsamix::kalsamix()
    : DCOPObject ( "KamixDCOPIface" ),
    KMainWindow( 0, "kalsamix" )
      
{

    setXMLFile("kalsamixui.rc");
    
    KStandardDirs stdd;
    KSplashScreen *spl = new KSplashScreen(
        QPixmap(stdd.findResource("data","kalsamix/kalsamix_splash.png")));
    spl->show();

    psmenu = new KPopupMenu();
    
    spl->message ( i18n("Reading config...") );

    KAMConfig::self()->readConfig();

    _doclose = false;

    QHBox *hb = new QHBox ( this );
    setCentralWidget(hb);

#ifdef VUMETER
    vu = new KVUMeter ( hb );
#endif

    vlayout = new QVBox ( hb );
    
    spl->message ( i18n("Initializing ALSA...") );
    int j;
    j = initAlsa();
    if ( !j ) {
        KMessageBox::error ( this,
            i18n("No ALSA card found on your system, KAlsaMix can't run!"),
	    i18n("Warning!"));
	exit(-1);
    }

    spl->message ( i18n("Initializing interface...") );
    KShortcut sh1, sh2;
    QString s1, s2;
    
    sh1.init( Qt::Key_PageUp );
    sh2.init( Qt::Key_PageDown );
    
    s1 = KAMConfig::self()->volUpShortcut;
    s2 = KAMConfig::self()->volDnShortcut;
    if ( s1!="" )
        sh1.init ( s1 );
    if ( s2!="" )
        sh2.init ( s2 );
    
    KAction *a1 = new KAction ( i18n("Quit"), "exit", 
        KStdAccel::shortcut(KStdAccel::Quit),
    	this, SLOT(quit()), actionCollection(), "exit" );
    KAction *a2 = new KAction ( i18n("Options"), "configure",
    	KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(showConfig()), actionCollection(), "options" );
    KAction *aUp = new KAction ( i18n("Volume up"), "up", 
        sh1,
        this, SLOT(volUp()), actionCollection(), "up" );
    KAction *aDown = new KAction ( i18n("Volume down"), "down", 
        sh2,
        this, SLOT(volDown()), actionCollection(), "down" );
    KAction *aAbout = new KAction ( i18n("About KAlsaMix"), "help",
        KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(showAbout()), actionCollection(), "help" );
	
#ifdef VUMETER
    KAction *aToggleScope = new KAction ( i18n("Show level meter"), "artsfftscope",
        KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(toggleMeter()), actionCollection(), "scope" );
    if ( !KAMConfig::self()->useVumeter ) 
        aToggleScope->setEnabled( false );
#endif

    KAction *aToggleViewV = new KAction ( i18n("Toggle view"), "view_top_bottom",
        KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(toggleViewTypeV()), actionCollection(), "toggleviewv" );
    KAction *aToggleViewH = new KAction ( i18n("Toggle view"), "view_left_right",
        KStdAccel::shortcut(KStdAccel::AccelNone),
        this, SLOT(toggleViewTypeH()), actionCollection(), "toggleviewh" );

    aPst = new KToolBarPopupAction ( i18n("Presets"), "down" );
    aPst->setIcon ( "project_open" );
    aPst->setDelayed ( false );
    connect ( aPst->popupMenu(), SIGNAL(activated(int)), this, SLOT(presetsPopup(int)));
    connect ( psmenu, SIGNAL(activated(int)), this, SLOT(presetsPopupMenu(int)));
    
    aUp->plug ( toolBar() );
    aDown->plug ( toolBar() );
    
    toolBar()->insertLineSeparator();
    
#ifdef VUMETER
    aToggleScope->plug ( toolBar() );
#endif

    toolBar()->insertLineSeparator();
    a2->plug ( toolBar() );
    aPst->plug ( toolBar() );
    aToggleViewV->plug ( toolBar() );
    aToggleViewH->plug ( toolBar() );
    
    toolBar()->insertLineSeparator();
    aAbout->plug ( toolBar() );
    a1->plug ( toolBar() );
    
    timer = new QTimer(this);
    
#ifdef VUMETER
    if ( KAMConfig::self()->useVumeter ) {
        adpt = new KArtsDispatcher();
        asvr = new KArtsServer();

        canShowMeter = true;
        if ( asvr->server().isNull() ) {
            timer2 = 0;
            aToggleScope->setEnabled(false);
	    showMeter = false;
	    canShowMeter = false;
        } else {
            vc = asvr->server().outVolume();
            timer2= new QTimer(this);
            timer2->changeInterval ( 20 );
            connect ( timer2,SIGNAL (timeout()), this, SLOT(readLevel()) );
        }
    }
#else
    canShowMeter = false;
#endif

    timer->changeInterval ( 250 );

    readConfig();

    if ( KAMConfig::self()->restoreWindow ) {
        setGeometry ( KAMConfig::self()->windowGeometry );
        move ( KAMConfig::self()->windowPosition );
    }

    initControls();

    tray = new KAMSystemTray ( this );
    tray->setPixmap ( tray->loadIcon ("kalsamix.png" ));
    psitem = tray->contextMenu()->insertItem ( i18n("Presets"), psmenu );
        
    connect ( tray, SIGNAL(quitSelected()), this, SLOT(quit()) );   
    connect ( tray, SIGNAL(midButtonPressed()), this, SLOT(midButtonPressed()) );
    connect ( tray, SIGNAL(wheelUp()), this, SLOT(volUp() ));
    connect ( tray, SIGNAL(wheelDown()), this, SLOT(volDown() ));
    tray->show();

    connect ( timer, SIGNAL (timeout()), this, SLOT(readMixer()) );
    updateTrayTip();

    spl->finish(this);
    delete spl;
}

kalsamix::~kalsamix()
{
}

int kalsamix::initAlsa() {
    int res;

    res = KAlsa::self()->init();
    if ( res )
        return res;
    alsa_callback_setup ( this );
    return 0;
}

void kalsamix::toggleViewType ( KMixerView::viewType t ) {
    unsigned int i;
    KMixerView::viewType t2;
    
    t2 = views.at(0)->type;
    if ( t != t2 )
        t2 = t;
    else
        t2 = KMixerView::viewTabs;

    for ( i=0; i<views.count(); i++ ) {
        views.at(i)->type = t2;
        views.at(i)->initControls();
    }
}

void kalsamix::toggleViewTypeV() {
    toggleViewType ( KMixerView::viewTiledVert );
}

void kalsamix::toggleViewTypeH() {
    toggleViewType ( KMixerView::viewTiledHor );
}

void kalsamix::toggleMeter() {
#ifdef VUMETER
    if ( !showMeter ) {
        showMeter = true;
        if ( timer2 )
	    timer2->start( 20 );
	vu->show();
    } else {
        showMeter = false;
        if ( timer2 )
	    timer2->stop();
	vu->hide();
    }
#endif
}

void kalsamix::readConfig ()
{
    unsigned int i;
    
    if ( KPreset::count()>0 ) {
        aPst->setEnabled(true);
	aPst->popupMenu()->clear();
	psmenu->setEnabled(true);
	psmenu->clear();
	for ( i=0; i<KPreset::count(); i++ ) {
	    aPst->popupMenu()->insertItem(KPreset::preset(i));
	    psmenu->insertItem(KPreset::preset(i));
        }
    } else {
        psmenu->setEnabled(false);
	aPst->setEnabled(false);
    }
}

void kalsamix::reinitApp() {
    KAMConfig::self()->allowLoadLevels = false;
    KAlsa::self()->init();    
    readConfig();
    initControls();
    setUpdatesEnabled( true );
    repaint();
    KAMConfig::self()->allowLoadLevels = true;
}

void kalsamix::initControls ()
{
    
#ifdef VUMETER
    showMeter = KAMConfig::self()->showMeter;
    
    if ( !canShowMeter || !showMeter || !KAMConfig::self()->useVumeter ) {
        if ( timer2 )
	    timer2->stop();
        vu->hide();
    }
#else
    showMeter = false;
#endif

    unsigned int i;
    for ( i=0; i<views.count(); i++ )
        delete views.at(i);

    KAMConfig::self()->config()->sync();
    
    views.clear();
    
    KMixerView *v;
    
    for ( i=0; i<KAlsa::self()->cardsCount(); i++ ) {
    
    	if ( !KAMConfig::self()->cardHidden( KAlsa::self()->card(i)->configSection() )) {
 	
	    switch ( KAMConfig::self()->viewType ) {
	    case 1:
                v = new KMixerView(KAlsa::self()->card(i)->mixer(), KMixerView::viewTiledVert, vlayout, "", this );
	        break;
	    case 2:
                v = new KMixerView(KAlsa::self()->card(i)->mixer(), KMixerView::viewTiledHor, vlayout, "", this );
	        break;
	    default:
	        v = new KMixerView(KAlsa::self()->card(i)->mixer(), KMixerView::viewTabs, vlayout, "", this );
	        break;
	    }
	    views.append(v);
	    v->initControls();
	    v->show();
	}
    }
    
}

KMixer* kalsamix::activeMixer() {
    return KAlsa::self()->card(activeCard)->mixer();
}

void kalsamix::showConfig()
{
    KConfigWin *cw = new KConfigWin( this );
    if ( cw->exec() )
        reinitControls();
}

void kalsamix::reinitControls()
{
    KAMConfig::self()->writeConfig();
    readConfig();
    initControls();
}

void kalsamix::readMixer()
{
    static int icon = -1;
    
    // Processes external ALSA events
    unsigned int i;
    for ( i=0; i<KAlsa::self()->cardsCount(); i++ )
        snd_mixer_handle_events(KAlsa::self()->card(i)->mixer()->amix);

    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    
    if ( itm && itm->muted(KMixConst::Playback, KAMIX_ALL_CHANNELS)==KMixConst::Muted && icon != 0 ) {
        tray->setPixmap ( tray->loadIcon ("kalsamix_muted.png" ));
	icon = 0;
    }
    if ( itm && itm->muted(KMixConst::Playback, KAMIX_ALL_CHANNELS)!=KMixConst::Muted && icon != 1 ) {
        tray->setPixmap ( tray->loadIcon ("kalsamix.png" ));
	icon = 1;
    }
}

void kalsamix::updateTrayTip() {
    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    long t = KAMConfig::self()->volumeMode;
    
    if ( itm ) {
        QString s2;
        QString s = itm->name();
	s += " "+i18n("at")+" ";
	if ( t == 0 )
		s += s2.sprintf ( "%ld", itm->volume(KMixConst::Playback) );
	if ( t == 1 )
		s += s2.sprintf ( "%ld%%", itm->percent(KMixConst::Playback) );
#ifdef HAS_SND_DECIBEL
	if ( t == 2 )
		s += s2.sprintf ( "%.2f dB", itm->dB(KMixConst::Playback) );
#endif
        switch ( itm->muted(KMixConst::Playback, KAMIX_ALL_CHANNELS )) {
        case KMixConst::Muted:
	    s += i18n(", muted" );
	    break;
	case KMixConst::Partial:
	    s += i18n(", partially muted" );
	}
	QToolTip::remove ( tray );
        QToolTip::add ( tray, s );
    }
    
}

void kalsamix::readLevel()
{
#ifdef VUMETER
    if ( KAMConfig::self()->useVumeter ) {
        vu->setValueLeft ( vc.currentVolumeLeft() );
        vu->setValueRight( vc.currentVolumeRight() );
    }
#endif
}

void kalsamix::quit()
{
    _doclose = true;
    timer->stop();
    close();
    exit(0);
}

void kalsamix::volDown(int v) {
    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    if ( itm ) {
        long l = itm->volume(KMixConst::Playback);
        itm->setVolume( KMixConst::Playback, l-v );
        updateTrayTip();
    }
}

void kalsamix::volUp(int v) {
    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    if ( itm ) {
        long l = itm->volume(KMixConst::Playback);
        itm->setVolume( KMixConst::Playback, l+v );
        updateTrayTip();
    }
}

bool kalsamix::queryExit() {
    QString s;
    unsigned int i;
    for ( i=0; i<KAlsa::self()->cardsCount(); i++ )
        KAlsa::self()->card(i)->writeConfig();
    
    return true;
}

void kalsamix::showAbout() {
    KAboutApplication *dlg = new KAboutApplication(this);
    dlg->show(this);
}

void kalsamix::closeEvent ( QCloseEvent *ev ) {
   
    if ( !_doclose && !ev->spontaneous() ) {

	if ( !KAMConfig::self()->trayWarning ) {
           KMessageBox::information ( this,
	   	i18n("Closing this window will NOT quit the application, it will be put "
		"the systray instead. If you really want to quit, press the Quit "
		"button on the toolbar, or select Quit on the menu of the systray"),
		i18n("Warning!"),
		"TrayWarning");
        }
       
        ev->ignore();
        this->hide();
    } else {
    
        // Forces views delete to store splitted values
        unsigned int i;
        for ( i=0; i<views.count(); i++ )
            delete views.at(i);
	
        KAMConfig::self()->showMeter = showMeter;
        KAMConfig::self()->windowGeometry = geometry();
        KAMConfig::self()->windowPosition = pos();
	KAMConfig::self()->restoreToSystray = isHidden();
        KAMConfig::self()->writeConfig();
        KAMConfig::self()->config()->sync();
	
        ev->accept();
    }
   
}

void kalsamix::showMaster() {
    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    if ( itm ) {
        osdDisable();
    	KMasterView *v = new KMasterView ( itm, this );
    	v->popup ( QCursor::pos() );
    }
}

void kalsamix::midButtonPressed() {
    if ( KAMConfig::self()->midButtonMutes )
        masterMute();
    else
        showMaster();
}

void kalsamix::presetsPopup(int i) {
    KPreset::loadPreset(aPst->popupMenu()->text(i));
}

void kalsamix::presetsPopupMenu(int i) {
    KPreset::loadPreset(psmenu->text(i));
}

void kalsamix::osdEnable() {
    unsigned int i;
    for ( i=0; i<views.count(); i++ )
        views.at(i)->osdEnabled = true;
}

void kalsamix::osdDisable() {
    unsigned int i;
    for ( i=0; i<views.count(); i++ )
        views.at(i)->osdEnabled = false;
}

// DCOP INTERFACE FUNCTIONS
void kalsamix::masterVolDown(int v) {
    volDown(v);
}

void kalsamix::masterVolUp(int v) {
    volUp(v);
}

void kalsamix::masterMute() {
    KMixerItem *itm = KAlsa::self()->shortItem(KAlsa::self()->defaultCard());
    if ( itm )
        itm->toggleMuted( KMixConst::Playback );
}

void kalsamix::masterShow() {
    showMaster();
}

void kalsamix::quitKAlsaMix() {
    quit();
}
