/***************************************************************************
                          fileview.h  -  description
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
/*
 *
 * $Id: fileview.h 310966 2004-05-13 09:49:46Z sgotti $
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef FILEVIEW_H
#define FILEVIEW_H


#include <qwidget.h>
#include <kcompletion.h>


class KDirOperator;
class QDragObject;
class KURL;
class KFileFilterCombo;
class KFileItem;
class KActionCollection;
class KConfig;
class QToolBox;
class KURLCompletion;
class KComboBox;


class FileView : public QWidget
{
  Q_OBJECT

 public:
  FileView(QWidget *parent=0, const char *name=0);
  ~FileView();
  void setUrl(const KURL &url, bool forward);
  KURL Url() const;
  void setAutoUpdate(bool);

  KActionCollection* actionCollection() const;

  void reload();

 signals:
  void urlEntered( const KURL& url );

 public slots:
	void saveConfig( KConfig* c );
	void slotGoUrl(const QString&);
	//void slotUrlEntered(const KURL &url);

 private:
  KDirOperator *m_dirOp;
  KFileFilterCombo* m_filterWidget;
	KComboBox *m_comboBox;
  KURLCompletion* urlCompletion;


  void setupGUI();

 private slots:
  void slotFilterChanged();

};


#endif
