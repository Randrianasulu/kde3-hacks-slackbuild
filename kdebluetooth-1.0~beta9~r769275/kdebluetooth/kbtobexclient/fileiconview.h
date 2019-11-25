/***************************************************************************
                          fileiconview.h  -  description
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


#ifndef MYFILEICONVIEW_H
#define MYFILEICONVIEW_H

#include <kfileiconview.h>

class KActionMenu;

class FileIconView : public KFileIconView
{
    Q_OBJECT
public:
    FileIconView(QWidget* parent, const char* name);
    ~FileIconView();

    virtual void insertItem(KURL url);

public slots:
    void slotNewItem( QDropEvent *evt );
    void activatedMenu( const KFileItem *, const QPoint& pos );
    void slotCancelFilesFromList();

protected:

    KActionMenu* fileListMenu;

    void createGui();

};

#endif // MYFILEICONVIEW_H
