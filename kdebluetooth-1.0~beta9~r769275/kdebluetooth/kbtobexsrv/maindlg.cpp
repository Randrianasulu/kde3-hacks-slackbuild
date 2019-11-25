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

#include "maindlg.h"
#include "myfileiconview.h"
#include <qlabel.h>
#include <kglobal.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qdir.h>
#include <qfile.h>
#include <qlayout.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kurl.h>
#include <krun.h>
#include <kfileitem.h>
#include <kmimetype.h>
#include <qframe.h>
#include <qfile.h>
#include <ktempfile.h>
#include <kio/netaccess.h>
#include <qdragobject.h>
#include <kprogress.h>
#include <ksqueezedtextlabel.h>
#include <kmdcodec.h>
#include <kapplication.h>

MainDialog::MainDialog(QWidget *parent, int s, QString name) :
        MainDialogBase(parent)
{
    closing = false;
    KGlobal::config()->setGroup("main");
    QString defaultPath = KGlobalSettings::desktopPath();
    targetPath = KGlobal::config()->readPathEntry("targetPath",
                 defaultPath);
    if (!QDir(targetPath).exists()) {
        targetPath = defaultPath;
    }
    targetURLRequester->setURL(targetPath);
    openFolderCheckbox->setChecked(KGlobal::config()->readBoolEntry(
                                       "openfolder", true));

    targetURLRequester->setMode(KFile::Directory | KFile::LocalOnly);
    messageLabel->setText(messageLabel->text().arg(name));
    rejectButton->setIconSet(KGlobal::iconLoader()->loadIcon(
                                 "button_cancel", KIcon::Toolbar));
    acceptButton->setIconSet(KGlobal::iconLoader()->loadIcon(
                                 "button_ok", KIcon::Toolbar));
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(slotAccept()));
    connect(rejectButton, SIGNAL(clicked()), this, SLOT(slotClose()));
    acceptButton->setEnabled(false);

    server = new ObexServer(s, name, this, "obexServer");
    connect(server, SIGNAL(putRequest(QString&, long, QString&)),
            this, SLOT(slotPutRequest(QString&, long, QString&)));
    connect(server, SIGNAL(received(QString&, QByteArray)),
            this, SLOT(slotReceived(QString&, QByteArray)));
    connect(server, SIGNAL(error(const QString&)),
            this, SLOT(slotError(const QString&)));
    connect(server, SIGNAL(finished()),
            this, SLOT(slotFinished()));
    connect(server, SIGNAL(progress(long)),
            this, SLOT(slotProgress(long)));
    server->start();

    fileView = new MyFileIconView(this, fileViewFrame, "fileView");
    connect(fileView, SIGNAL(clicked(QIconViewItem*)),
            this, SLOT(slotIconClicked(QIconViewItem*)));
    fileView->setSelectionMode(KFile::Extended);
    fileView->showPreviews();
    QHBoxLayout *fileViewLayout = new QHBoxLayout(fileViewFrame);
    fileViewLayout->addWidget(fileView);

    fileItemList.setAutoDelete(true);
    /*item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
        "file:/home/fred/test.cpp");
    fileView->insertItem(item);
    delete item;*/

    mainlogo =  KGlobal::iconLoader()->loadIcon("kdebluetooth", KIcon::Small, 16);
    setIcon(mainlogo);

    this->show();
    createTempDir();
}

QDragObject* MainDialog::createDragObject()
{
    //KMessageBox::information(NULL, "bla");
    QUriDrag *ret = new QUriDrag(this);
    QStringList fileList;
    KFileItem *item = fileView->firstFileItem();
    if (item) do {
            if (fileView->isSelected(item)) {
                fileList.append(item->url().path());
            }
            item = fileView->nextItem(item);
        } while (item);
    ret->setFileNames(fileList);
    return ret;
}


void MainDialog::createTempDir()
{
    // Let KTempFile suggest a name for a temporary directory
    QString tempfilename;
    KTempFile tempFile;
    tempfilename = tempFile.name();
    tempFile.close();
    tempFile.unlink();
    // Create the temporary directory
    QDir dir;
    if (dir.mkdir(tempfilename)) {
        tempDir = tempfilename;
    }
}


void MainDialog::removeTempDir()
{
    for (unsigned int n=0; n<tempFiles.count(); ++n) {
        QFile::remove(tempFiles[n]);
    }
    QDir d(tempDir);
    d.rmdir(tempDir);
}

void MainDialog::slotIconClicked(QIconViewItem *i)
{
   if (!i) return;
   KURL file( "file:" + tempDir + "/" +  i->text() );
   KFileItem f( file, "", 0 );
   f.refresh();
   f.refreshMimeType();
   KRun::runURL( file, f.mimetype() );
}

void MainDialog::slotPutRequest(QString& filename, long length, QString&/*mimeType */)
{
    server->acceptFile(true);
    currentFilename = filename;
    currentLength = length;
    progress->setEnabled(true);
    progress->setTotalSteps(length);
    progress->setProgress(currentLength/2);
    statusText->setText(i18n("Receiving %1").arg(filename));
}

void MainDialog::slotProgress(long pos)
{
    double dPos = pos;
    QString unit = "B";
    if (dPos >= 1024) {
        dPos /= 1024;
        unit = "kB";
    }
    if (dPos >= 1024) {
        dPos /= 1024;
        unit = "MB";
    }
    progress->setFormat(QString("%1 %2").arg(dPos, 0, 'f', 1).arg(unit));
    progress->setProgress(pos);
}

void MainDialog::slotReceived(QString& filename, QByteArray data)
{
    QDir dir(tempDir);
    QString path = dir.path()+"/"+filename;
    QFile outFile(path);
    if (outFile.open(IO_WriteOnly)) {
        outFile.writeBlock(data);
        outFile.close();
        tempFiles.append(outFile.name());

        KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                                        QString("file:")+outFile.name());
        fileView->insertItem(item);
        acceptButton->setEnabled(true);
    }
    statusText->setText("done.");
    progress->setProgress(0);
    progress->setEnabled(false);
}

void MainDialog::slotFinished()
{
}

void MainDialog::slotClose()
{
    this->close();
}

void MainDialog::slotError(const QString& /*errStr*/)
{
    statusText->setText(i18n("Disconnected."));
    progress->setProgress(0);
    progress->setEnabled(false);
}

void MainDialog::slotAccept()
{
    QString targetPath = targetURLRequester->url();
    if (!QDir(targetPath).exists()) {
        KMessageBox::error(this, i18n("The selected folder does not exist. \
                           Please select another folder."), i18n("Obex Server"));
        return;
    }

    if (!targetPath.endsWith("/")) {
        targetPath += "/";
    }

    // Move each file to the target directory
    for (unsigned int n=0; n<tempFiles.count(); ++n) {
        QString filename = tempFiles[n];
        if (filename == QString::null) continue;

        QString outfilename = QFileInfo(filename).fileName();
        QFileInfo outfileInfo(outfilename);
        QString newfilename = outfilename;
        QFile outFile(targetPath+outfilename);
        if (outFile.exists()) {
            if (filesEqual(filename, targetPath+outfilename) == false) {
                // Find an alternative name for the file
                int n = 0;
                do {
                    newfilename = outfileInfo.baseName() + "_" +
                                QString::number(n) + "." + outfileInfo.extension();
                } while (QFile(targetPath+newfilename).exists());
            }
            else {
                existingFileList.append(outfilename);
                continue;
            }
        }

        // Ask what to do if the file exists already in the target dir
        //TODO: Use a dialog where the user can edit the suggested name. Standarddialog..?
        if (outfilename != newfilename) {
            int ret = KMessageBox::warningYesNoCancel(this,
                i18n("%1 already exists. Do you want to use %2 as \
filename instead?").arg(outfilename).arg(newfilename),QString::null,i18n("Rename"),i18n("Overwrite"));
            if (ret == KMessageBox::Yes) {
                outFile.setName(targetPath+newfilename);
            } else if (ret == KMessageBox::No) {
                outFile.setName(targetPath+outfilename);
            } else
                return;
        }

        // Move to file to the target directory
        if (KIO::NetAccess::copy(KURL(filename), KURL(outFile.name())
#if (KDE_VERSION >= 320)
          , this
#endif
          ) == false) {
            KMessageBox::error(this,
                i18n("Error writing file %1.").arg(outFile.name()));
        }
    }

    // let konqueror show the target directory
    if (openFolderCheckbox->isChecked()) {
        KMimeType::Ptr mime = KMimeType::findByPath(targetPath);
        if (mime) {
            KRun::runURL(KURL(targetPath), mime->name(), false);
        }
    }

    if (existingFileList.count() > 0) {
        KMessageBox::informationList(this,
        i18n("The following files already existed in the target folder and were not \
saved again with a different name:"), existingFileList);
    }
    KApplication::kApplication()->quit();
}

bool MainDialog::filesEqual(QString filename1, QString filename2)
{
    QFile f1(filename1);
    QFile f2(filename2);
    if (f1.open(IO_ReadOnly) == false) return false;
    if (f2.open(IO_ReadOnly) == false) return false;

    KMD5 digest1;
    KMD5 digest2;
    digest1.update(f1);
    digest2.update(f2);

    return (QString(digest1.hexDigest()) == QString(digest2.hexDigest()));
}

MainDialog::~MainDialog()
{
    removeTempDir();
    KGlobal::config()->setGroup("main");
    QString targetPath = targetURLRequester->url();
    if (QDir(targetPath).exists()) {
        KGlobal::config()->writePathEntry(
            "targetPath",targetPath);
    }
    KGlobal::config()->writeEntry("openfolder",
                                  openFolderCheckbox->isChecked());
    if (server) {
        delete server;
    }
}

#include "maindlg.moc"
