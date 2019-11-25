/***************************************************************************
                          fileview.cpp  -  description
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

#include <unistd.h>

#include "fileview.h"

#include "mainwindow.h"
#include "diroperator.h"

#include <qwidget.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>

#include <kurldrag.h>
#include <kfilefiltercombo.h>
#include <klocale.h>
#include <kfileitem.h>
#include <kmessagebox.h>
#include <kcombobox.h>
#include <kcompletionbox.h>
#include <kurlcompletion.h>


FileView::FileView(QWidget *parent, const char *name )
        : QWidget(parent,name)
{
    setupGUI();
}


FileView::~FileView()
{}


KActionCollection* FileView::actionCollection() const
{
    return m_dirOp->actionCollection();
}


void FileView::setupGUI()
{
    this->setMinimumHeight(200);

    QVBoxLayout* layout = new QVBoxLayout( this );

	KToolBar* toolbar2 = new KToolBar( this , "FileView::toolbar2", true);

	m_comboBox = new KComboBox(TRUE, toolbar2 );
    urlCompletion = new KURLCompletion();
    m_comboBox->setCompletionObject(urlCompletion);
	connect( m_comboBox, SIGNAL(returnPressed(const QString&)), this, SLOT(slotGoUrl(const QString&)) );


	QLabel* m_comboLabel = new QLabel(i18n("Location: "), toolbar2, "kde toolbar widget"); // With this name the label is styled !!!

	int m_comboLabel_id = toolbar2->count();
	toolbar2->insertWidget(m_comboLabel_id,-1,m_comboLabel);
	int m_comboBox_id = toolbar2->count();
	toolbar2->insertWidget(m_comboBox_id,-1,m_comboBox);
	toolbar2->setItemAutoSized(m_comboBox_id);

	m_dirOp = new DirOperator( QDir::home().absPath(), this );
    m_dirOp->readConfig( MainApp()->config(), "file view" );
    m_dirOp->setMode( KFile::Files );
    m_dirOp->setView( KFile::Default );
	//connect( m_dirOp, SIGNAL(urlEntered(const KURL &)), this, SLOT(slotUrlEntered(const KURL &)));
	//connect( m_dirOp, SIGNAL(completion (const QString &)), m_comboBox, SLOT(setCompletedText(const QString &)));

	KToolBar* toolbar = new KToolBar( this , "FileView::toolbar", true);
    toolbar->setFlat(true);

    KActionCollection *coll = m_dirOp->actionCollection();

    // plug nav items into the toolbar
    coll->action( "up" )->plug( toolbar );
    coll->action( "up" )->setWhatsThis(i18n("<qt>Click this button to enter the parent folder.<p>"
                                               "For instance, if the current location is file:/home/%1 clicking this "
                                               "button will take you to file:/home.</qt>").arg(getlogin()));
    coll->action( "back" )->plug( toolbar );
    coll->action( "back" )->setWhatsThis(i18n("Click this button to move backwards one step in the browsing history."));
    coll->action( "forward" )->plug( toolbar );
    coll->action( "forward" )->setWhatsThis(i18n("Click this button to move forward one step in the browsing history."));
    coll->action( "reload" )->plug( toolbar );
    coll->action( "reload" )->setWhatsThis(i18n("Click this button to reload the contents of the current location."));
    coll->action( "mkdir" )->setShortcut(Key_F10);
    coll->action( "mkdir" )->plug( toolbar );
    coll->action( "mkdir" )->setWhatsThis(i18n("Click this button to create a new folder."));

    KActionMenu *menu = new KActionMenu( i18n("Configure"), "configure", this, "extra menu" );

    menu->setWhatsThis(i18n("<qt>This is the configuration menu for the file dialog. "
                            "Various options can be accessed from this menu including: <ul>"
                            "<li>how files are sorted in the list</li>"
                            "<li>types of view, including icon and list</li>"
                            "<li>showing of hidden files</li>"
                            "<li>the Quick Access navigation panel</li>"
                            "<li>file previews</li>"
                            "<li>separating folders from files</li></ul></qt>"));
    menu->insert( coll->action( "sorting menu" ));
    menu->insert( coll->action( "separator" ));
    coll->action( "short view" )->setShortcut(Key_F6);
    menu->insert( coll->action( "short view" ));
    coll->action( "detailed view" )->setShortcut(Key_F7);
    menu->insert( coll->action( "detailed view" ));
    menu->insert( coll->action( "separator" ));
    coll->action( "show hidden" )->setShortcut(Key_F8);
    menu->insert( coll->action( "show hidden" ));
    //  menu->insert( showSidebarAction );
    coll->action( "preview" )->setShortcut(Key_F11);
    menu->insert( coll->action( "preview" ));
    coll->action( "separate dirs" )->setShortcut(Key_F12);
    menu->insert( coll->action( "separate dirs" ));

    menu->setDelayed( false );
    menu->plug( toolbar );

    // create filter selection combobox
	QLabel* filterLabel = new QLabel(i18n("Filter:"), toolbar, "kde toolbar widget");  // With this name the label is styled !!!
    toolbar->insertWidget( toolbar->count(), 0, filterLabel );

	m_filterWidget = new KFileFilterCombo( toolbar, "filterwidget" );
    toolbar->insertWidget( toolbar->count(), 0, m_filterWidget );
    m_filterWidget->setEditable( true );
    QString filter = i18n("*|All Files");
    filter += "\n" + i18n("image/jpeg |JPEG Image Files");
    m_filterWidget->setFilter(filter);
    connect( m_filterWidget, SIGNAL(filterChanged()), SLOT(slotFilterChanged()) );

	layout->addWidget( toolbar);
    layout->addWidget( toolbar2 );
    layout->addWidget( m_dirOp );

    layout->setStretchFactor( m_dirOp, 1 );
}


void FileView::slotGoUrl(const QString& text)
{
    setUrl(text, FALSE);
}

void FileView::setUrl(const KURL& url, bool forward)
{
    m_dirOp->setURL( url, forward );
}

KURL FileView::Url() const
{
    return m_dirOp->url();
}

void FileView::setAutoUpdate(bool b)
{
    m_dirOp->dirLister()->setAutoUpdate( b );
}

void FileView::slotFilterChanged()
{
    QString filter = m_filterWidget->currentFilter();
    m_dirOp->clearFilter();

    if( filter.find( '/' ) > -1 ) {
        QStringList types = QStringList::split( " ", filter );
        types.prepend( "inode/directory" );
        m_dirOp->setMimeFilter( types );
    } else
        m_dirOp->setNameFilter( filter );

    m_dirOp->rereadDir();
    //  emit filterChanged( filter );
}

/*void FileView::slotUrlEntered(const KURL &url)
{
	m_comboBox->setCompletedText(url.path());
}*/

void FileView::reload()
{
    m_dirOp->actionCollection()->action("reload")->activate();
}

void FileView::saveConfig( KConfig* c )
{
    m_dirOp->writeConfig( c, "file view" );
}


#include "fileview.moc"
