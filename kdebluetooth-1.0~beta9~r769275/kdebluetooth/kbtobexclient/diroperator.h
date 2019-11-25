/***************************************************************************
                          diroperator.h  -  description
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


#ifndef DIROPERATOR_H
#define DIROPERATOR_H

#include <kdiroperator.h>

class QIconViewItem;
class QListViewItem;


/**
  *@author Sebastian Trueg
  */
class DirOperator : public KDirOperator
{
  Q_OBJECT

 public: 
  DirOperator( const KURL& urlName = KURL(), QWidget* parent = 0, const char* name = 0 );
  ~DirOperator();

 signals:
  void doubleClicked( KFileItem* item );

 protected:
  KFileView* createView( QWidget* parent, KFile::FileView view );

 protected slots:
  void slotIconViewItemDoubleClicked( QIconViewItem* );
  void slotListViewItemDoubleClicked( QListViewItem* );
};

#endif
