/***************************************************************************
                          dndfileview.h  -  description
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


#ifndef DNDFILEVIEW_H
#define DNDFILEVIEW_H

#include <kfiledetailview.h>
#include <kfileiconview.h>

class QListViewItem;
class QIconViewItem;


/**
 * adds Drag'n'Drop support
  * @author Sebastian Trueg
  */
class DndFileDetailView : public KFileDetailView
{
  Q_OBJECT

 public: 
  DndFileDetailView( QWidget* parent, const char* name );
  ~DndFileDetailView();

 protected slots:
   void slotSelected( QListViewItem* item );

 protected:
  QDragObject* dragObject();
};


class DndFileIconView : public KFileIconView
{
  Q_OBJECT

 public:
  DndFileIconView( QWidget* parent = 0, const char* name = 0);
  ~DndFileIconView();

 protected slots:
   void slotSelected( QIconViewItem* item );

 protected:
  QDragObject* dragObject();
};

#endif
