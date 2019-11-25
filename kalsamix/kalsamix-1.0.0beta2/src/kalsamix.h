/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _KAMIX_H_
#define _KAMIX_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <qptrlist.h>
#include <qtimer.h>

#ifdef VUMETER
#include <arts/kartsserver.h>
#include <arts/kartsdispatcher.h>
#include <arts/artsflow.h>
#endif

#include <qvbox.h>

#include "kamdcopiface.h"
#include "kmixer.h"
#include "kamsystemtray.h"
#include "kmixerview.h"
#include "kvumeter.h"
#include "kpopupmenu.h"
#include "kosd.h"

/**
 * @short Application Main Window
 * @author Stefano Rivoir <s.rivoir@gts.it>
 * @version 1.0.0beta1
 */
class kalsamix : public KMainWindow, virtual public KAMDCOPIface
{
    Q_OBJECT

private:

#ifdef VUMETER
    KArtsServer     *asvr;
    KArtsDispatcher *adpt;
    Arts::StereoVolumeControl vc;
#endif

    QVBox     *vlayout;
    
    bool canShowMeter;
    QTimer *timer, *timer2;

    KToolBarPopupAction *aPst;
    
    void initControls();
    int  initAlsa();
    bool showMeter;

    bool _doclose;
    long activeCard;
    KMixer* activeMixer();

public:
    
    kalsamix();
    virtual ~kalsamix();
    
#ifdef VUMETER
    KVUMeter *vu;
#endif

    QPtrList<KMixerView> views;
    KAMSystemTray *tray;
    KPopupMenu *psmenu;
    int	psitem;
    
    bool queryExit();
    void closeEvent( QCloseEvent* );
    
    // DCOP Iface functions
    void masterVolUp(int v=1);
    void masterVolDown(int v=1);
    void masterMute();
    void masterShow();
    void quitKAlsaMix();
        
public slots:

    void reinitApp();
    void readMixer();
    void readConfig();
    void showConfig();
    void quit();
    void volDown(int v=1);
    void volUp(int v=1);
    void reinitControls ( );
    void showAbout();
    void showMaster();
    void toggleViewTypeV();
    void toggleViewTypeH();
    void toggleViewType(KMixerView::viewType t);
    void presetsPopup(int);    
    void presetsPopupMenu(int);    
    void readLevel();
    void toggleMeter();
    void midButtonPressed();
    void updateTrayTip();
    
    void osdEnable();
    void osdDisable();
    
};

#endif // _KAMIX_H_
