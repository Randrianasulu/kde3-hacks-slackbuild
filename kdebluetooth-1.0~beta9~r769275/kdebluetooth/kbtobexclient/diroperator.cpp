/***************************************************************************
                          diroperator.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
		From the original version of Sebastian Trueg <trueg@k3b.org>
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


#include "diroperator.h"
#include "dndfileview.h"

#include <kcombiview.h>
#include <kfilepreview.h>
#include <kaction.h>


DirOperator::DirOperator(const KURL& url, QWidget* parent, const char* name )
  : KDirOperator( url, parent, name )
{
  // disable the del-key
  KAction* aDelete = actionCollection()->action("delete");

//disable the delete submenu option
   if( aDelete )
    aDelete->setEnabled( FALSE );
	
  KAction* aBack = actionCollection()->action("back");
  aBack->setEnabled( FALSE );
  KAction* aForward = actionCollection()->action("forward");
  aForward->setEnabled( FALSE );
}


DirOperator::~DirOperator()
{
}


KFileView* DirOperator::createView( QWidget* parent, KFile::FileView view )
{
  KFileView* new_view = 0L;
  bool separateDirs = KFile::isSeparateDirs( view );
  bool preview=( (view & KFile::PreviewInfo) == KFile::PreviewInfo ||
		 (view & KFile::PreviewContents) == KFile::PreviewContents );
  
  if( separateDirs ) {
    KCombiView *combi = new KCombiView( parent, "combi view" );
    combi->setOnlyDoubleClickSelectsFiles( onlyDoubleClickSelectsFiles() );
    KFileView* v = 0L;
    if ( (view & KFile::Simple) == KFile::Simple )
      v = createView( combi, KFile::Simple );
    else
      v = createView( combi, KFile::Detail );
    combi->setRight( v );
    new_view = combi;
  }
  else if( (view & KFile::Detail) == KFile::Detail && !preview ) {
    new_view = new DndFileDetailView( parent, "detail view");
    connect( (DndFileDetailView*)new_view, SIGNAL(doubleClicked(QListViewItem*)), 
	     this, SLOT(slotListViewItemDoubleClicked(QListViewItem*)) );
  }
  else if ((view & KFile::Simple) == KFile::Simple && !preview ) {
    new_view = new DndFileIconView( parent, "simple view");
    new_view->setViewName( i18n("Short View") );
    connect( (DndFileIconView*)new_view, SIGNAL(doubleClicked(QIconViewItem*)),
	     this, SLOT(slotIconViewItemDoubleClicked(QIconViewItem*)) );
  }
  else { // preview
    KFileView* v = 0L; // will get reparented by KFilePreview
    if ( (view & KFile::Simple ) == KFile::Simple )
      v = createView( 0L, KFile::Simple );
    else
      v = createView( 0L, KFile::Detail );
    
    KFilePreview* pView = new KFilePreview( v, parent, "preview" );
    pView->setOnlyDoubleClickSelectsFiles( onlyDoubleClickSelectsFiles() );
    new_view = pView;
  }

  return new_view;
}


void DirOperator::slotIconViewItemDoubleClicked( QIconViewItem* item )
{
  if( KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>( item ) )
    if( f->fileInfo()->isFile() )
      emit doubleClicked( f->fileInfo() );
}


void DirOperator::slotListViewItemDoubleClicked( QListViewItem* item )
{
  if( KFileListViewItem* f = dynamic_cast<KFileListViewItem*>( item ) )
    if( f->fileInfo()->isFile() )
      emit doubleClicked( f->fileInfo() );
}


#include "diroperator.moc"

