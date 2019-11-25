/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kconfigwin.h"
#include "kmixeritem.h"
#include "kpreset.h"
#include "kamconfig.h"
#include "kalsa.h"
#include "config.h"

#include <qlistbox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <kapplication.h>
#include <klocale.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qptrlist.h>
#include <qtabwidget.h>
#include <qvgroupbox.h>
#include <qmap.h>
#include <kfontdialog.h>
#include <qvbox.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <qlistview.h>
#include <qcursor.h>

KConfigWin::KConfigWin(QWidget*)
 : KDialogBase( Tabbed, i18n("Options"), Ok|Cancel, Ok, 0, 0, true, true )
{

    activeCard     = 0;
    activeLinkCard = 0;

    QFrame *page1 = addPage( i18n("General") );
    QFrame *page5 = addPage( i18n("Appearance") );
    QFrame *page2 = addPage( i18n("Cards") );
    QFrame *page3 = addPage( i18n("Presets") );
    QFrame *page4 = addPage( i18n("OSD") );
    QFrame *page6 = addPage( i18n("Links") );

    QGridLayout *gl = new QGridLayout ( page1 );
    QGridLayout *gl2= new QGridLayout ( page2 );
    QGridLayout *gl22=new QGridLayout ( page2 );
    QGridLayout *gl3= new QGridLayout ( page3 );
    QGridLayout *gl4= new QGridLayout ( page4 );
    QGridLayout *gl5= new QGridLayout ( page5 );
    QGridLayout *gl6= new QGridLayout ( page6 );

    /*
        General options
    */

    QVGroupBox *gbox = new QVGroupBox ( i18n("General options"), page1 );
    cLoad = new QCheckBox(i18n("Load levels on startup"), gbox );
    cSave = new QCheckBox(i18n("Save levels on exit"), gbox );
    cHideI= new QCheckBox(i18n("Hide inactive controls"), gbox );
    cPG   = new QCheckBox(i18n("Restore window geometry and position"), gbox );
    cAST  = new QCheckBox(i18n("Always restore to systray"), gbox );
    cDebug= new QCheckBox(i18n("Debug info when launched from console"), gbox );
    cMid  = new QComboBox(gbox);
    cMid->insertItem(i18n("Mid button on systray toggles mute"));
    cMid->insertItem(i18n("Mid button on systray opens short item"));

#ifdef VUMETER
    cVum  = new QCheckBox(i18n("Enable VU-meter based on aRts (you need to restart kalsamix)"), gbox );
#endif

    vu    = new KKeyButton( page1 );
    vd    = new KKeyButton( page1 );
    QLabel *lu = new QLabel(i18n("Master vol &up key"), page1 );
    QLabel *ld = new QLabel(i18n("Master vol &down key"), page1 );
    lu->setBuddy ( vu );
    ld->setBuddy ( vd );

    connect ( vu, SIGNAL(capturedShortcut(const KShortcut&)), 
    	this, SLOT(slotVolUpKey(const KShortcut&)) );
    connect ( vd, SIGNAL(capturedShortcut(const KShortcut&)), 
    	this, SLOT(slotVolDnKey(const KShortcut&)) );

    cSave->setChecked ( KAMConfig::self()->saveLevels );
    cLoad->setChecked ( KAMConfig::self()->loadLevels );
    cPG->setChecked   ( KAMConfig::self()->restoreWindow );
    cDebug->setChecked( KAMConfig::self()->debugInfo );
    cHideI->setChecked( KAMConfig::self()->hideInactive );
    cAST->setChecked  ( KAMConfig::self()->alwaysSystray );

    if ( KAMConfig::self()->midButtonMutes )
        cMid->setCurrentItem(0);
    else
        cMid->setCurrentItem(1);

#ifdef VUMETER
    cVum->setChecked ( KAMConfig::self()->useVumeter );
#endif

    vu->setShortcut ( KShortcut ( KAMConfig::self()->volUpShortcut ), false );
    vd->setShortcut ( KShortcut ( KAMConfig::self()->volDnShortcut ), false );

    gl->addMultiCellWidget ( gbox, 0, 0, 0, 1 );

    gl->addWidget ( lu, 1, 0 );
    gl->addWidget ( vu, 1, 1 );
    gl->addWidget ( ld, 2, 0 );
    gl->addWidget ( vd, 2, 1 );

    /*
        Appearance
    */
    cTick = new QCheckBox(i18n("Show tickmarks on sliders"), page5 ) ;
    cMute = new QCheckBox(i18n("Show mute buttons"), page5 );
    cBal  = new QCheckBox(i18n("Show balance buttons"), page5 );
    cFrames=new QCheckBox(i18n("Show frame around items"), page5 );
    cGFrames=new QCheckBox(i18n("Show frame around groups"), page5 );
    cVolumeMode = new QComboBox( page5 );
    cVolumeMode->insertItem ( i18n("Absolute value") );
    cVolumeMode->insertItem ( i18n("Percent") );
#ifdef HAS_SND_DECIBEL
    cVolumeMode->insertItem ( i18n("dB") );
#endif
    connect ( cVolumeMode, SIGNAL(activated(int)), this, SLOT(levelMethodChanged(int)) );

    QLabel *lblVM = new QLabel ( i18n("Show levels as"), page5 );
    QLabel *lblLFont = new QLabel ( i18n("Channels/cards font"), page5 );
    bLblFont = new QPushButton ( i18n("Choose"), page5 );

    cTick->setChecked ( KAMConfig::self()->showTicks );
    cMute->setChecked ( KAMConfig::self()->showMute );
    cBal->setChecked  ( KAMConfig::self()->showBal );
    cFrames->setChecked(KAMConfig::self()->itemsFrame );
    cGFrames->setChecked ( KAMConfig::self()->showFrameAroundGroups );
    cVolumeMode->setCurrentItem ( KAMConfig::self()->volumeMode );

    lblf = &KAMConfig::self()->lblFont;
    gl5->addMultiCellWidget ( cTick, 0, 0, 0, 1 );
    gl5->addMultiCellWidget ( cMute, 1, 1, 0, 1 );
    gl5->addMultiCellWidget ( cBal , 2, 2, 0, 1 );
    gl5->addMultiCellWidget ( cFrames, 3, 3, 0, 1 );
    gl5->addMultiCellWidget ( cGFrames, 4, 4, 0, 1 );
    gl5->addWidget ( lblVM, 5, 0 );
    gl5->addWidget ( cVolumeMode, 5, 1 );
    gl5->addWidget ( lblLFont, 6, 0 );
    gl5->addWidget ( bLblFont, 6, 1 );

    /*
        Per-card config
    */
    QLabel *sh = new QLabel ( i18n("Item to show in short view"), page2 );
    cShort= new QComboBox( page2 );
    sh->setBuddy ( cShort );

    itmdn = new QPushButton ( i18n("Move down"), page2 );
    itmup = new QPushButton ( i18n("Move up"), page2 );  
    itmdn->setEnabled(false);
    itmup->setEnabled(false);

    mCard  = new QCheckBox ( i18n("Default card"), page2 );
    cHideCard = new QCheckBox ( i18n("Hide this card"), page2 );
    cCards = new QComboBox ( page2 );

    newPgroup = new QPushButton ( i18n("New playback group"), page2 );
    newCgroup = new QPushButton ( i18n("New capture group"), page2 );
    delgroup = new QPushButton ( i18n("Delete group"), page2 );

    QString s;
    unsigned int i;
    for ( i=0; i<KAlsa::self()->cardsCount(); i++ ) {
        s.setNum( i );
	s+=". ";
	s+=KAlsa::self()->card(i)->name();
        cCards->insertItem( s );
    }

    lst = new QListView ( page2 );
    lst->addColumn ( i18n("Item name") );
    lst->addColumn ( i18n("Item ID") );
    lst->addColumn ( i18n("Order") );
    lst->addColumn ( i18n("Item ALSA names") );
    lst->setSortColumn ( -1 );

    gl2->addWidget ( cCards, 0, 0 );
    gl2->addWidget ( mCard, 1, 0 );
    gl2->addWidget ( cHideCard, 2, 0 );
    gl2->addWidget ( sh, 3, 0 );
    gl2->addWidget ( cShort, 4, 0 );
    gl2->addWidget ( lst, 5, 0 );
    gl2->addLayout ( gl22, 6, 0 );
    gl22->addWidget ( itmdn, 0, 0 );
    gl22->addWidget ( itmup, 0, 1 );
    gl22->addWidget ( newPgroup, 1, 0 );
    gl22->addWidget ( newCgroup, 1, 1 );
    gl22->addWidget ( delgroup, 1, 2 );

    activeCard = -1;
    cCards->setCurrentItem ( 0 );
    changeActiveCard();

    delgroup->setEnabled ( false );
    itmdn->setEnabled ( false );
    itmup->setEnabled ( false );

    connect ( itmdn, SIGNAL(clicked()), this, SLOT(itemMoveDown()) );
    connect ( itmup, SIGNAL(clicked()), this, SLOT(itemMoveUp()) );
    connect ( cCards, SIGNAL(activated(int)), this, SLOT(changeActiveCard()) );

    connect ( lst, SIGNAL(itemRenamed(QListViewItem*,int,const QString&)), 
             this, SLOT(itemRenamed(QListViewItem*,int,const QString&)) );
    connect ( lst, SIGNAL(currentChanged(QListViewItem*)),
             this, SLOT(itemClicked(QListViewItem*)) );

    /*
        Presets
    */
    QVBoxLayout *ly2 = new QVBoxLayout ( gl3 );
    pst = new QListView ( page3 );
    pst->addColumn ( i18n("Presets" ) );
    bSavePst = new QPushButton ( i18n("Save"), page3 );
    bDelPst = new QPushButton ( i18n("Delete"), page3 );
    bApplyPst = new QPushButton ( i18n("Apply"), page3 );

    QLabel *lname = new QLabel ( i18n("Save current volumes as preset:"), page3 );

    bSavePst->setEnabled ( false );

    pname = new QLineEdit ( page3 );

    connect ( pname, SIGNAL(textChanged(const QString&)), this, SLOT(pnameChanged(const QString&)) );
    connect ( bSavePst, SIGNAL(pressed()), this, SLOT(savePreset()) );
    connect ( bApplyPst, SIGNAL(pressed()), this, SLOT(applyPreset()) );
    connect ( bDelPst, SIGNAL(pressed()), this, SLOT(delPreset()) );
    connect ( pst, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(activePresetChanged(QListViewItem*) ) );
    connect ( pst, SIGNAL(doubleClicked ( QListViewItem *, const QPoint &, int )), this, SLOT(applyPreset()) );

    gl3->addWidget ( pst, 0, 0 );
    gl3->addWidget ( lname, 1, 0 );
    gl3->addWidget ( pname, 2, 0 );
    gl3->addWidget ( bSavePst, 3, 0 );

    ly2->addWidget ( bApplyPst );
    ly2->addWidget ( bDelPst );

    gl3->addMultiCell( ly2, 0, 0, 1, 1, Qt::AlignTop );

    fillPresets();

    /*
        OSD
    */
    QLabel *osd3 = new QLabel ( i18n("OSD position"), page4 );

    cOsd = new QCheckBox ( i18n("Show OSD messages"), page4 );
    ceOsd= new QCheckBox ( i18n("Show only when externally triggered"), page4 );
    cop = new QComboBox ( page4 );
    QLabel *fname = new QLabel ( i18n("OSD font"), page4 );
    bFont = new QPushButton ( i18n("Choose"), page4 );
    cbb = new KColorButton ( page4 );
    cbf = new KColorButton ( page4 );
    QLabel *l1 = new QLabel ( i18n("Background color"), page4 );
    QLabel *l2 = new QLabel ( i18n("Foreground color"), page4 );
    QLabel *l3 = new QLabel ( i18n("X position"), page4 );
    QLabel *l4 = new QLabel ( i18n("Y position"), page4 );
    osdX = new QSpinBox ( 10, QApplication::desktop()->screen(0)->size().width()-10, 1, page4 );
    osdY = new QSpinBox ( 10, QApplication::desktop()->screen(0)->size().height()-10, 1, page4 );

    osdf = &KAMConfig::self()->osdFont;

    cop->insertItem ( i18n("Top left") );
    cop->insertItem ( i18n("Top right") );
    cop->insertItem ( i18n("Bottom left") );
    cop->insertItem ( i18n("Bottom right") );
    cop->insertItem ( i18n("Custom") );

    gl4->addMultiCellWidget ( cOsd , 0, 0, 0, 1 );
    gl4->addMultiCellWidget ( ceOsd, 1, 1, 0, 1 );
    gl4->addWidget ( osd3,  2, 0 );
    gl4->addWidget ( cop ,  2, 1 );
    gl4->addWidget ( l3, 3, 0 );
    gl4->addWidget ( osdX, 3, 1 );
    gl4->addWidget ( l4, 4, 0 );
    gl4->addWidget ( osdY, 4, 1 );
    gl4->addWidget ( fname, 5, 0 );
    gl4->addWidget ( bFont, 5, 1 );
    gl4->addWidget ( l1, 6, 0 );
    gl4->addWidget ( cbb, 6, 1 );
    gl4->addWidget ( l2, 7, 0 );
    gl4->addWidget ( cbf, 7, 1 );

    cbb->setColor ( KAMConfig::self()->osdBackcolor );
    cbf->setColor ( KAMConfig::self()->osdForecolor );
    
    cop->setCurrentItem( KAMConfig::self()->osdPosition );
    cOsd->setChecked ( KAMConfig::self()->showOSD );
    ceOsd->setChecked ( KAMConfig::self()->showOSDExtern );
    osdX->setValue ( KAMConfig::self()->osdPositionX );
    osdY->setValue ( KAMConfig::self()->osdPositionY );
    
    connect ( bFont, SIGNAL(pressed()), this, SLOT(slotChooseOSDFont()) );
    connect ( bLblFont, SIGNAL(pressed()), this, SLOT(slotChooseLBLFont()) );
    

    /*
	Links
    */
    cLinkCards = new QComboBox ( page6 );
    links = new QListView ( page6 );
    cLinkSrc = new QComboBox ( page6 );
    cLinkDst = new QComboBox ( page6 );

    QPushButton *bAddLink = new QPushButton ( i18n("Add link"), page6 );
    bDelLink = new QPushButton ( i18n("Delete link"), page6 );
    bDelLink->setEnabled(false);

    links->addColumn ( i18n("Source" ) );
    links->addColumn ( i18n("Linked" ) );

    for ( i=0; i<KAlsa::self()->cardsCount(); i++ ) {
        s.setNum( i );
	s+=". ";
	s+=KAlsa::self()->card(i)->name();
        cLinkCards->insertItem( s );
    }

    gl6->addMultiCellWidget ( cLinkCards, 0, 0, 0, 1 );
    gl6->addMultiCellWidget ( links, 1, 1, 0, 1);
    gl6->addWidget ( new QLabel(i18n("Source"),page6), 2, 0 );
    gl6->addWidget ( new QLabel(i18n("Linked"),page6), 2, 1 );
    gl6->addWidget ( cLinkSrc, 3, 0 );
    gl6->addWidget ( cLinkDst, 3, 1 );
    gl6->addWidget ( bAddLink, 4, 0 );
    gl6->addWidget ( bDelLink, 4, 1 );

    activeLinkCard = -1;
    cLinkCards->setCurrentItem ( 0 );
    changeLinkCard();

    connect ( cLinkCards, SIGNAL(activated(int)), this, SLOT(changeLinkCard()) );
    connect ( bAddLink, SIGNAL(clicked()), this, SLOT(addLink()) );
    connect ( bDelLink, SIGNAL(clicked()), this, SLOT(delLink()) );
    connect ( links, SIGNAL(selectionChanged(QListViewItem*)), 
	this, SLOT(linkSelected(QListViewItem*)) );

    /*
       General connects
    */

    connect ( newPgroup, SIGNAL(clicked()), this, SLOT(newPlaybackGroup()) );
    connect ( newCgroup, SIGNAL(clicked()), this, SLOT(newCaptureGroup()) );
    connect ( delgroup, SIGNAL(clicked()), this, SLOT(delGroup()) );

    connect ( lst, SIGNAL(mouseButtonClicked(int,QListViewItem*,const QPoint&,int)),
             this, SLOT(itemMousePressed(int,QListViewItem*,const QPoint&,int)) );
}

void KConfigWin::levelMethodChanged(int mode) {
#ifdef HAS_SND_DECIBEL
    if ( mode!=2 )
        return;
#endif
}

void KConfigWin::linkSelected(QListViewItem* itm) {
    if ( itm )
        bDelLink->setEnabled(true);
    else
        bDelLink->setEnabled(false);
}

void KConfigWin::addLink() {
    if ( cLinkSrc->currentItem()==cLinkDst->currentItem() ) {
        KMessageBox::sorry( this, i18n("Can't connect an item to itself") );
        return;
    }
    QListViewItem *itm = new QListViewItem(links);
    itm->setText(0, cLinkSrc->currentText() );
    itm->setText(1, cLinkDst->currentText() );
}

void KConfigWin::delLink() {
    links->removeItem(links->currentItem());
}

void KConfigWin::delGroup()
{
    QListViewItem *oldgrp, *newgrp;
    QListViewItem *itm, *pitm;

    if ( KMessageBox::questionYesNo(this, i18n("Are you sure you want to delete this group?")) == No )
        return;

    // Reparent childern to Main group
    oldgrp = lst->currentItem();
    if ( oldgrp->parent() == iP ) {
        newgrp = lst->findItem ( "GP0", 2 );
        pitm = iP;
    } else {
        newgrp = lst->findItem ( "GC0", 2 );
        pitm = iC;
    }
    
    QListViewItemIterator lit( lst );
    while ( lit.current() ) {
        itm = (QCheckListItem*)lit.current();
        if ( itm->parent() == oldgrp ) {
            oldgrp->takeItem(itm);
            newgrp->insertItem(itm);
        }
        ++lit;
    }
    pitm->takeItem ( oldgrp );
    delete oldgrp;
    itemsReorder();
    
}

void KConfigWin::newPlaybackGroup() {
    QString s = KInputDialog::getText(i18n("New group"),i18n("Insert new group name") );
    if ( s.contains(",") )
        KMessageBox::sorry( this, i18n("Group names cannot contain commas") );
    if ( s == "" )
        return;
    
    QListViewItem *itm = new QListViewItem ( iP, s );
    itm->setText (2, "GPXXX" );
    
    lst->setSortColumn ( 2 );
    lst->sort();
    itemsReorder();
}

void KConfigWin::newCaptureGroup() {
    QString s = KInputDialog::getText(i18n("New group"),i18n("Insert new group name") );
    if ( s.contains(",") )
        KMessageBox::sorry( this, i18n("Group names cannot contain commas") );
    if ( s == "" )
        return;

    QListViewItem *itm = new QListViewItem ( iC, s );
    itm->setText (2, "GCXXX" );

    lst->setSortColumn ( 2 );
    lst->sort();
    itemsReorder();
}

void KConfigWin::itemRenamed( QListViewItem *it, int, const QString& text ) {
    
    // Set the same name for capture and playback item
    QCheckListItem *itm;
    long id = it->text(1).toLong();
    
    if ( it->text(1).length() ) {
        // Item renamed
        QListViewItemIterator lit( lst );
        while ( lit.current() ) {
            itm = (QCheckListItem*)lit.current();
            if ( itm->text(1).length() &&
                 itm->text(1).toLong() == id && 
                 itm!=it )
                itm->setText(0,text);
            ++lit;
        }
    } else {
        // Group renamed   
    }
	
}

void KConfigWin::changeActiveCard() {
    if ( activeCard != -1 )
        saveCardSettings();
    activeCard = cCards->currentItem();
    configSect = KAlsa::self()->card(activeCard)->configSection();
    fillCardItems();
}

void KConfigWin::changeLinkCard() {
    if ( activeLinkCard != -1 )
        saveCardLinks();
    activeLinkCard = cLinkCards->currentItem();
    linkConfigSect = KAlsa::self()->card(activeLinkCard)->configSection();
    fillLinkItems();
    fillLinks();
}

void KConfigWin::fillLinkItems() {
    unsigned int i;

    KMixer *mix = KAlsa::self()->card(activeLinkCard)->mixer();
    KMixerItem *itm;

    for ( i=0; i<mix->items.count(); i++ ) {
        itm = mix->items.at(i);
        if ( itm->hasPlaybackCapabilities() ) {
            cLinkSrc->insertItem(itm->name() );
            cLinkDst->insertItem(itm->name() );
        }
    }

}

void KConfigWin::fillLinks() {
    int i, src, dst;
    QString s;
    QStringList sl;

    QListViewItem *itm;
    long l = KAMConfig::self()->rawLong(linkConfigSect,"Links",-1);
    KMixer *mix = KAlsa::self()->card(activeLinkCard)->mixer();

    if ( l==-1 )
        return;

    links->clear();
    for ( i=0; i<l; i++ ) {
        s = KAMConfig::self()->rawString(linkConfigSect,"Link","",i);
        itm = new QListViewItem(links);

        sl = QStringList::split(",",s);
        QStringList::iterator it = sl.begin();

        src = ((QString)*it).toInt();
        it++;
        dst = ((QString)*it).toInt();
        itm->setText(0,mix->items.at(src)->name());
        itm->setText(1,mix->items.at(dst)->name());
    }
}

void KConfigWin::fillCardItems() {
    unsigned int i, j;
    QString s;
    QCheckListItem *it;
    int porder, corder, order;
    QListViewItem *iG;
    QStringList str;
    KMixerItem *mixitm;
    
    KMixer *mix = KAlsa::self()->card(activeCard)->mixer();
    
    if ( KAlsa::self()->cardsCount() == 1 ) {
        mCard->setChecked ( true );
	mCard->setEnabled ( false );
	cHideCard->setChecked ( false );
	cHideCard->setEnabled ( false );
    } else {
	mCard->setChecked ( KAMConfig::self()->defaultCard == activeCard );
	mCard->setEnabled ( true );
	cHideCard->setChecked ( KAMConfig::self()->cardHidden(configSect) );
	cHideCard->setEnabled ( true );
    }
    
    lst->clear();
    
    // Fill groups
    iP = new QListViewItem ( lst, i18n("Playback") );
    iP->setText ( 2, "0" );
    iP->setOpen ( true );
    iG = new QListViewItem ( iP, i18n("Main") );
    iG->setText ( 2, "GP0" );
    iG->setOpen ( true );
    
    iC = new QListViewItem ( lst, i18n("Capture") );
    iC->setText ( 2, "1" );
    iC->setOpen ( true );
    iG = new QListViewItem ( iC, i18n("Main") );
    iG->setText ( 2, "GC0" );
    iG->setOpen ( true );
    
    str = KAMConfig::self()->playbackGroups ( configSect );
    i = 1;
    for ( QStringList::Iterator it = str.begin(); it != str.end(); ++it ) {
        s.sprintf ( "GP%d", i++ );
    	iG = new QListViewItem ( iP, *it );
    	iG->setOpen ( true );
    	iG->setText ( 2, s );
    	iG->setRenameEnabled ( 0, true );
    }
    
    str = KAMConfig::self()->captureGroups ( configSect );
    i = 1;
    for ( QStringList::Iterator it = str.begin(); it != str.end(); ++it ) {
        s.sprintf ( "GC%d", i++ );
    	iG = new QListViewItem ( iC, *it );
    	iG->setOpen ( true );
    	iG->setText ( 2, s );
    	iG->setRenameEnabled ( 0, true );
    }
    
    corder = porder = 0;
    for ( i=0; i<mix->items.count(); i++ ) {
        it = 0;
        mixitm = mix->items.at(i);
        if ( mixitm->hasPlaybackCapabilities() ) {
	    order = KAMConfig::self()->playbackOrder(configSect,i);
	    
	    if ( order==-1 )
	        order = porder;
	    
	    j = mixitm->pbGroup;
	    s.sprintf ( "GP%d", j );
	    iG = lst->findItem( s, 2 );
	    if ( !iG )
	        iG = iP;
            it =  new QCheckListItem ( iG, mixitm->name(), QCheckListItem::CheckBox );
	    it->setText ( 1, s.sprintf("%d", i) );
	    it->setText ( 2, s.sprintf("%.2d", order) );
	    it->setText ( 3, mixitm->alsaName() );
	    it->setOn ( KAMConfig::self()->showPlayback( configSect, i ) );
	    cShort->insertItem ( mixitm->name() );
	    it->setRenameEnabled ( 0, true );
	    
	    porder++;
	}
        if ( mixitm->hasCaptureCapabilities() ) {
	    order = KAMConfig::self()->captureOrder(configSect,i);
	    
	    if ( order==-1 )
	        order = corder;
	    
	    j = mixitm->cpGroup;
	    s.sprintf ( "GC%d", j );
	    iG = lst->findItem( s, 2 );
	    if ( !iG )
	        iG = iC;
            it =  new QCheckListItem ( iG, mixitm->name(), QCheckListItem::CheckBox );
	    it->setText ( 1, s.sprintf("%d", i) );
	    it->setText ( 2, s.sprintf("%.2d", order) );
	    it->setText ( 3, mixitm->alsaName() );
	    it->setOn ( KAMConfig::self()->showCapture( configSect, i ) );
	    it->setRenameEnabled ( 0, true );
	    
	    corder++;
	}
    }
    
    if ( !KAMConfig::self()->rawLong( configSect, "ShortItem", 0 ) ) {
        cShort->setCurrentText ( mix->items.at(0)->name() );
    } else {
	cShort->setCurrentText ( mix->itemById( KAMConfig::self()->rawLong ( configSect, "ShortItem", 0 ) )->name() );
    }

    lst->setSortColumn(2);
    lst->sort();
    lst->setSortColumn(-1);
}

void KConfigWin::fillPresets() {
    pst->clear();
    unsigned int i;
    for ( i=0; i<KPreset::count(); i++ ) {
        pitm = new QListViewItem ( pst, KPreset::preset(i) );
    }
    if ( pst->currentItem()==0 ) {
        bApplyPst->setEnabled(false);
	bDelPst->setEnabled(false);
    } else {
        bApplyPst->setEnabled(true);
	bDelPst->setEnabled(true);
    }
    
    pname->setText("");
}

KConfigWin::~KConfigWin()
{
}

void KConfigWin::slotOk()
{
    saveCardSettings();
    saveCardLinks();

    KAMConfig::self()->volDnShortcut = vd->shortcut().toString();
    KAMConfig::self()->volUpShortcut = vu->shortcut().toString();
    KAMConfig::self()->showTicks = cTick->isChecked();
    KAMConfig::self()->showMute  = cMute->isChecked();
    KAMConfig::self()->showBal   = cBal->isChecked();
    KAMConfig::self()->loadLevels= cLoad->isChecked();
    KAMConfig::self()->saveLevels= cSave->isChecked();
    KAMConfig::self()->restoreWindow = cPG->isChecked();
    KAMConfig::self()->debugInfo = cDebug->isChecked();
    KAMConfig::self()->hideInactive = cHideI->isChecked();
    KAMConfig::self()->itemsFrame = cFrames->isChecked();
    KAMConfig::self()->showFrameAroundGroups = cGFrames->isChecked();
    KAMConfig::self()->alwaysSystray = cAST->isChecked();
    KAMConfig::self()->showOSD = cOsd->isChecked();
    KAMConfig::self()->showOSDExtern = ceOsd->isChecked();
    KAMConfig::self()->osdPosition = cop->currentItem();
    KAMConfig::self()->osdPositionX = osdX->value();
    KAMConfig::self()->osdPositionY = osdY->value();
    KAMConfig::self()->osdFont = *osdf;
    KAMConfig::self()->lblFont = *lblf;
    KAMConfig::self()->osdBackcolor = cbb->color();
    KAMConfig::self()->osdForecolor = cbf->color();
    KAMConfig::self()->midButtonMutes = (cMid->currentItem()==0);
    KAMConfig::self()->volumeMode = cVolumeMode->currentItem();
    
#ifdef VUMETER
    KAMConfig::self()->useVumeter = cVum->isChecked();
#endif

    KAMConfig::self()->writeConfig();
    
    KDialogBase::slotOk();
}

#include <kdebug.h>
void KConfigWin::saveCardLinks() {
    kdDebug()<<"entering saveCardLinks"<<endl;
    QListViewItem *itm;
    QString s;
    int i = 0;

    KMixer *mix = KAlsa::self()->card(activeLinkCard)->mixer();

    QListViewItemIterator lit(links);
    while ( lit.current() ) {
        itm = (QListViewItem*)lit.current();
        s.sprintf ( "%d,%d",
            mix->itemByName(itm->text(0))->id, 
            mix->itemByName(itm->text(1))->id );
        KAMConfig::self()->setRawString( linkConfigSect, "Link", s, i++ );
        ++lit;
    }
    KAMConfig::self()->setRawLong ( linkConfigSect, "Links", i );
}

void KConfigWin::saveCardSettings() {
    kdDebug()<<"entering saveCardSettings, section "<<configSect<<endl;
    long l;
    QCheckListItem *itm;
    QStringList spg, scg;
    
    // Save items/groups hierarchy
    QListViewItemIterator lit( lst );
    while ( lit.current() ) {
        itm = (QCheckListItem*)lit.current();
        if ( itm->text(1).length() ) {
            // Item
            l = itm->text(1).toLong();
            KAMConfig::self()->setCustomName ( configSect, l, itm->text(0) );
            if ( itm->parent()->parent() == iP ) {
	        KAMConfig::self()->setShowPlayback ( configSect, l, itm->isOn() );
	        KAMConfig::self()->setPlaybackOrder ( configSect, l, itm->text(2).toLong() );
	        KAMConfig::self()->setPlaybackGroup ( configSect, l, 
	            itm->parent()->text(2).mid(2).toLong() );
	    }
            if ( itm->parent()->parent() == iC ) {
	        KAMConfig::self()->setShowCapture ( configSect, l, itm->isOn() );
	        KAMConfig::self()->setCaptureOrder ( configSect, l, itm->text(2).toLong() );
	        KAMConfig::self()->setCaptureGroup ( configSect, l, 
	            itm->parent()->text(2).mid(2).toLong() );
	    }
	}
	if ( itm->text(1).length()==0 && itm->parent() && 
	     itm->text(2)!="GP0" && itm->text(2)!="GC0" ) {
	    if ( itm->parent()==iP )
	        spg.append ( itm->text(0) );
	    if ( itm->parent()==iC )
	        scg.append ( itm->text(0) );
	}
        ++lit;
    }
    
    KAMConfig::self()->setPlaybackGroups ( configSect, spg );
    KAMConfig::self()->setCaptureGroups ( configSect, scg );
    
    KAMConfig::self()->setRawLong ( configSect, "ShortItem", 
        KAlsa::self()->card(activeCard)->mixer()->itemByName( cShort->currentText() )->id );
    
    if ( mCard->isChecked() )
	KAMConfig::self()->defaultCard = activeCard;

    KAMConfig::self()->setRawLong( configSect, "HideCard", (cHideCard->isChecked() ? 1 : 0), -1 );
    kdDebug()<<"done saveCardSettings, section "<<configSect<<endl;
}

void KConfigWin::savePreset() {
    QString s = KPreset::volumesToString();
    KAMConfig::self()->setPreset ( pname->text(), s );
    fillPresets();
}

void KConfigWin::delPreset() {
    KAMConfig::self()->delPreset ( pst->currentItem()->text(0) );
    fillPresets();
}

void KConfigWin::applyPreset() {
    KPreset::loadPreset( pst->currentItem()->text(0) );
}

void KConfigWin::pnameChanged(const QString& s) {
    bSavePst->setEnabled ( !s.isEmpty() );
}

void KConfigWin::slotVolUpKey( const KShortcut &sc ) {
    vu->setShortcut ( sc, false );
}

void KConfigWin::slotVolDnKey( const KShortcut &sc ) {
    vd->setShortcut ( sc, false );
}

void KConfigWin::activePresetChanged(QListViewItem* itm ) {
    pname->setText( itm->text(0) );
}

void KConfigWin::slotChooseOSDFont() {
    KFontDialog::getFont( *osdf, false, this );
}

void KConfigWin::slotChooseLBLFont() {
    KFontDialog::getFont( *lblf, false, this );
}

void KConfigWin::itemMoveDown() {
    int order;
    QString s;
    
    QListViewItem *it = lst->currentItem();
    if ( !it )
        return;
    
    QListViewItem *itbelow = it->itemBelow();
    
    order = it->text(2).toLong();
    s.sprintf ( "%.2d", order );
    itbelow->setText(2,s);
    
    order++;
    s.sprintf ( "%.2d", order );
    it->setText(2,s);
    
    lst->setSortColumn ( 2 );
    lst->sort();
    itemsReorder();
    
    itemClicked ( it );
}

void KConfigWin::itemsReorder() {
    int order = 0;
    int ordergp= 0;
    int ordergc= 0;
    QString s;
    QListViewItemIterator lit( lst );
    while ( lit.current() ) {
        if ( lit.current()->text(1).length() ) {
            // Items
            s.sprintf ( "%.2d", order++ );
            lit.current()->setText(2,s);
        }
        if ( lit.current()->text(1).length()==0 && lit.current()->parent() ){
            // Groups
            if ( lit.current()->parent() == iP )
                s.sprintf ( "GP%d", ordergp++ );
            else
                s.sprintf ( "GC%d", ordergc++ );
            lit.current()->setText(2,s);
        }
        ++lit;
    }
    
    lst->setSorting ( 2 );
    lst->sort();
    lst->setSortColumn ( -1 );
}

void KConfigWin::itemMoveUp() {
    int order;
    QString s;
    
    QListViewItem *it = lst->currentItem();
    if ( !it )
        return;
    
    QListViewItem *itabove = it->itemAbove();
    
    order = it->text(2).toLong();
    s.sprintf ( "%.2d", order );
    itabove->setText(2,s);
    
    order--;
    s.sprintf ( "%.2d", order );
    it->setText(2,s);
    
    lst->setSortColumn ( 2 );
    lst->sort();
    itemsReorder();

    itemClicked ( it );
}

void KConfigWin::itemClicked(QListViewItem* itm) {
    if ( itm==iP || itm==iC ) {
        itmdn->setEnabled(false);
        itmup->setEnabled(false);
        delgroup->setEnabled ( false );
	return;
    };
    QListViewItem *p = itm->parent();
    itmup->setEnabled ( itm != p->firstChild() && itm->text(1).length() );
    itmdn->setEnabled ( itm->nextSibling() && itm->text(1).length() );
    
    delgroup->setEnabled ( itm->text(2)!="GP0" && itm->text(2)!="GC0" &&
        (!itm->text(1).length()) );
}

void KConfigWin::itemMousePressed(int btn, QListViewItem* itm, const QPoint &, int ) {
    if ( btn != 2 || itm==0 )
        return;
    
    if ( !itm->text(1).length() )
        return;
        
    int i, j;
    QStringList str;
    QListViewItem *itmp = itm->parent()->parent(); // Playback or capture root item
    
    lst->setCurrentItem ( itm );
    mnu = new KPopupMenu ( );
    mnu->insertTitle ( i18n("Move item to...") );
    
    QListViewItemIterator lit( lst );
    while ( lit.current() ) {
        if ( (QListViewItem*)(lit.current()->parent()) == itmp )
            str.append ( lit.current()->text(0) );
        ++lit;
    }
    
    j = 0;
    for ( QStringList::Iterator it = str.begin(); it != str.end(); ++it ) {
        i = mnu->insertItem ( *it, this, SLOT(itemGroupChanged(int)) );
        mnu->setItemParameter ( i, j++ );
    }
    
    mnu->exec(QCursor::pos());
}

void KConfigWin::itemGroupChanged(int m) {
    
    QCheckListItem *olditm = (QCheckListItem*)lst->currentItem();
    QListViewItem *newgrp;
    QListViewItem *oldgrp;
    QString s;
    
    oldgrp = olditm->parent();
    
    if ( olditm->parent()->parent()==iP )
        s.sprintf ( "GP%d", mnu->itemParameter ( m ) );
    else
        s.sprintf ( "GC%d", mnu->itemParameter ( m ) );
    
    newgrp = lst->findItem ( s, 2 );
    
    if ( newgrp ) {
    
        oldgrp->takeItem ( olditm );
        newgrp->insertItem ( olditm );
        
        itemsReorder();
    }
    
}

#include "kconfigwin.moc"
