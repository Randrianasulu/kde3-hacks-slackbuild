//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAINDLG_H
#define MAINDLG_H

#include "maindlgbase.h"
#include <kurl.h>
#include <kurlrequester.h>
#include <kicondialog.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <libkobex/kobexserver.h>
#include <qcstring.h>
#include <qguardedptr.h>
#include <qdir.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kpixmap.h>

class MyFileIconView;
class KFileItem;

/**
@author Fred Schaettgen
*/
class MainDialog : public MainDialogBase
{
    Q_OBJECT
public:
    MainDialog(QWidget *parent, int s, QString name);
    ~MainDialog();
    QDragObject* createDragObject();
public slots:
    void slotAccept();
private:
    void createTempDir();
    void removeTempDir();
    bool filesEqual(QString f1, QString f2);
    QString tempDir;
    QStringList tempFiles;
    ObexServer *server;
    QString currentFilename;
    long currentLength;
    QString targetPath;
    QGuardedPtr<MyFileIconView> fileView;
    QPtrList<KFileItem> fileItemList;
    QStringList existingFileList;

    bool closing;
    KPixmap mainlogo;

private slots:
    void slotPutRequest(QString&, long, QString&);
    void slotReceived(QString&, QByteArray);
    void slotError(const QString& errStr);
    void slotProgress(long pos);
    void slotFinished();
    void slotIconClicked(QIconViewItem*);
	void slotClose();
};

#endif
