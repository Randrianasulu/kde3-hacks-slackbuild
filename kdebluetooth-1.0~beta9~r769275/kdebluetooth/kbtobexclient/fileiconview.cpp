/***************************************************************************
                          fileiconview.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
    email                : simone.gotti@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>

#include "fileiconview.h"

#include <qfileinfo.h>
#include <kiconview.h>
#include <qiconview.h>
#include <kurldrag.h>
#include <kaction.h>
#include <klocale.h>
#include <kdeversion.h>
#include <kdebug.h>

using namespace std;

FileIconView::FileIconView(QWidget* parent, const char* name) :
       KFileIconView(parent, name)
{
    setAcceptDrops(true);
    createGui();
}

FileIconView::~FileIconView() {}

void FileIconView::createGui()
{
    KFileViewSignaler *sig = this->signaler();
    connect(sig, SIGNAL( activatedMenu(const KFileItem *, const QPoint& ) ),
            this, SLOT( activatedMenu(const KFileItem *, const QPoint& )));

// I don't exactly know from which version this new signal was introduced... So I've select the first alpha release.
#if KDE_IS_VERSION(3,1,90)
    QObject::connect(
        this, SIGNAL(dropped(QDropEvent*, KFileItem *)),
        this, SLOT(slotNewItem(QDropEvent*)));
#else
	QObject::connect(
        this, SIGNAL(dropped(QDropEvent*,const QValueList<QIconDragItem>&)),
        this, SLOT(slotNewItem(QDropEvent*)));
#endif
    KAction* actionCancelSelection = new KAction( i18n("&Cancel Selected Files"), Key_Delete,
                                     this, SLOT(slotCancelFilesFromList()),
                                     actionCollection(), "cancel_selected_files");

    // insert actions into fileview menu
    fileListMenu = new KActionMenu( i18n("&New Project"), "filenew", actionCollection(), "file_new" );
    fileListMenu->insert( actionCancelSelection, 0 );
}

void FileIconView::insertItem(KURL url)
{
    QFileInfo fileInfo(url.path());
	// I'll add the url only if it's a valid file
	if(fileInfo.isFile()) {
    KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, url);
    KFileIconView::insertItem(item);
	}
	kdDebug() << "insertItem" << endl;
}

void FileIconView::slotNewItem( QDropEvent *evt)
{
    KURL::List urlList;

    if ( KURLDrag::decode( evt, urlList ) ) {

        KURL::List::iterator it;
        for ( it = urlList.begin(); it != urlList.end(); ++it ) {
            insertItem(*it);
        }
    }
	qDebug("slotNewItem");
}

void FileIconView::activatedMenu( const KFileItem *, const QPoint& pos )
{
    const KFileItemList* m_selectedList = selectedItems();
    if (!m_selectedList->isEmpty())
        fileListMenu->popup( pos );
}

void FileIconView::slotCancelFilesFromList()
{
    const KFileItemList* m_selectedList = selectedItems();
    while(KFileItem* item = m_selectedList->current())
        removeItem(item);
}

#include "fileiconview.moc"
