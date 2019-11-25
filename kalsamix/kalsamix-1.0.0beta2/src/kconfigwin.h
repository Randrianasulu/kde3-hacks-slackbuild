/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KCONFIGWIN_H
#define KCONFIGWIN_H

#include <kdialogbase.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <kkeybutton.h>
#include <kconfigdialog.h>
#include <qlineedit.h>
#include <kcolorbutton.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <kpopupmenu.h>
#include <qcombobox.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kcard.h"

/**
@author Stefano Rivoir
*/
class KConfigWin : public KDialogBase
{
Q_OBJECT
public:
    KConfigWin(QWidget *parent);

    ~KConfigWin();

    QCheckBox	*cTick, *cBal, *cMute, *cHideI;
    QCheckBox   *cSave, *cLoad, *cPG, *cDebug, *cFrames, *cGFrames, *cHideCard;
    QCheckBox   *cOsd, *ceOsd, *cAST;
    QPushButton *bSavePst, *bDelPst, *bApplyPst, *bFont, *bLblFont, *bDelLink;
    QComboBox   *cMid;

    QSpinBox	*osdX, *osdY;
    QListBox	*ing, *ning;
    QPushButton *newCgroup, *newPgroup, *delgroup;
    QPushButton *itmdn, *itmup;

    KPopupMenu  *mnu;

#ifdef VUMETER
    QCheckBox   *cVum;
#endif

    KColorButton *cbf, *cbb;

    QComboBox   *cShort, *cCards, *cop, *cGroup, *cVolumeMode, *cLinkSrc, *cLinkDst;
    QComboBox	*cLinkCards;

    QCheckBox   *mCard;
    QLineEdit   *pname;
    QFont	*osdf;
    QFont	*lblf;

    int  activeCard;	// Current card for card setttings
    int  activeLinkCard;// Current card for linkings

    KKeyButton *vu, *vd;

    QString configSect, linkConfigSect;
    QListView *lst;
    QListViewItem *iP, *iC;

    QListView *pst, *links;
    QListViewItem *pitm;

    void fillCardItems();
    void fillLinkItems();
    void fillPresets();
    void fillLinks();
    void saveCardSettings();
    void saveCardLinks();
    void itemsReorder();

public slots:
    void slotOk();
    void slotChooseOSDFont();
    void slotChooseLBLFont();
    void slotVolUpKey ( const KShortcut& );
    void slotVolDnKey ( const KShortcut& );
    void pnameChanged( const QString& );
    void changeActiveCard();
    void changeLinkCard();
    void savePreset();
    void applyPreset();
    void delPreset();
    void activePresetChanged(QListViewItem*);
    void newPlaybackGroup();
    void newCaptureGroup();
    void delGroup();
    void itemRenamed( QListViewItem*,int,const QString& );
    void itemMousePressed ( int, QListViewItem *, const QPoint&, int );
    void itemMoveDown();
    void itemMoveUp();
    void itemGroupChanged(int);
    void itemClicked( QListViewItem* );
    void levelMethodChanged(int);
    void addLink();
    void delLink();
    void linkSelected(QListViewItem*);

};

#endif
