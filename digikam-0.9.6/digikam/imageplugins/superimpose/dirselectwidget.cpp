/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-01-04
 * Description : a Digikam image editor plugin for superimpose a 
 *               template to an image.
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// Qt includes.

#include <qlayout.h>
#include <qheader.h>
#include <qlistview.h>
#include <qdir.h>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "ddebug.h"
#include "dirselectwidget.h"
#include "dirselectwidget.moc"

namespace DigikamSuperImposeImagesPlugin
{

struct DirSelectWidget::Private
{
    KFileTreeBranch* m_item;
    QStringList      m_pendingPath;
    QString          m_handled;
    KURL             m_rootUrl;
};

DirSelectWidget::DirSelectWidget(QWidget* parent, const char* name, QString headerLabel)
               : KFileTreeView( parent, name)
{
    d = new Private;
    
    addColumn( headerLabel );
    
    if ( headerLabel.isNull() )
        header()->hide();
        
    setAlternateBackground(QColor());
}

DirSelectWidget::DirSelectWidget(KURL rootUrl, KURL currentUrl, 
                                 QWidget* parent, const char* name, QString headerLabel)
               : KFileTreeView( parent, name)
{
    d = new Private;
    
    addColumn( headerLabel );
    
    if ( headerLabel.isNull() )
        header()->hide();
        
    setAlternateBackground(QColor());
    setRootPath(rootUrl, currentUrl);
}

DirSelectWidget::~DirSelectWidget()
{
    delete d;
}

KURL DirSelectWidget::path() const
{
    return currentURL();
}

void DirSelectWidget::load()
{
    if ( d->m_pendingPath.isEmpty() ) 
    {
        disconnect( d->m_item, SIGNAL( populateFinished(KFileTreeViewItem *) ), 
                    this, SLOT( load() ) );
        
        emit folderItemSelected(currentURL());
        return;
    }

    QString item = d->m_pendingPath.front();
    d->m_pendingPath.pop_front();
    d->m_handled += item;    
    KFileTreeViewItem* branch = findItem( d->m_item, d->m_handled );
    
    if ( !branch ) 
    {
        DDebug() << "Unable to open " << d->m_handled << endl;
    }
    else
    {
        branch->setOpen( true );
        setSelected( branch, true );
        ensureItemVisible ( branch );
        d->m_handled += '/';
        
        if ( branch->alreadyListed() )
            load();
    }
}

void DirSelectWidget::setCurrentPath(KURL currentUrl)
{
    if ( !currentUrl.isValid() )
       return;
    
    QString currentPath = QDir::cleanDirPath(currentUrl.path());
    currentPath = currentPath.mid( d->m_rootUrl.path().length() );
    d->m_pendingPath.clear();    
    d->m_handled = QString("");
    d->m_pendingPath = QStringList::split( "/", currentPath, true );
    
    if ( !d->m_pendingPath[0].isEmpty() )
        d->m_pendingPath.prepend( "" ); // ensure we open the root first.        
        
    connect( d->m_item, SIGNAL( populateFinished(KFileTreeViewItem *) ),
             this, SLOT( load() ) );
    load();
}

void DirSelectWidget::setRootPath(KURL rootUrl, KURL currentUrl)
{
    d->m_rootUrl = rootUrl;
    clear();
    QString root = QDir::cleanDirPath(rootUrl.path());
    
    if ( !root.endsWith("/"))
       root.append("/");
    
    QString currentPath = QDir::cleanDirPath(currentUrl.isValid() ? currentUrl.path() : root);
    
    d->m_item = addBranch( rootUrl, rootUrl.fileName() );    
    setDirOnlyMode( d->m_item, true );
    currentPath = currentPath.mid( root.length() );
    d->m_pendingPath = QStringList::split( "/", currentPath, true );

    if ( !d->m_pendingPath[0].isEmpty() )
        d->m_pendingPath.prepend( "" ); // ensure we open the root first.
                    
    connect( d->m_item, SIGNAL( populateFinished(KFileTreeViewItem *) ),
             this, SLOT( load() ) );
    
    load();
    
    connect( this, SIGNAL( executed(QListViewItem *) ),
             this, SLOT( slotFolderSelected(QListViewItem *) ) );
}

KURL DirSelectWidget::rootPath(void)
{
    return d->m_rootUrl;
}

void DirSelectWidget::slotFolderSelected(QListViewItem *)
{
    emit folderItemSelected(currentURL());
}

}   // NameSpace DigikamSuperImposeImagesPlugin

