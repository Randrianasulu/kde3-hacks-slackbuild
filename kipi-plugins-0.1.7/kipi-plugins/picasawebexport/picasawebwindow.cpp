/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qlistview.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qradiobutton.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qtextedit.h>

// KDE includes.

#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <krun.h>
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kwallet.h>
#endif

// Libkipi includes.

#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "uploadwidget.h"
#include "picasawebtalker.h"
#include "picasawebitem.h"
#include "picasawebviewitem.h"
#include "picasaweblogin.h"
#include "picasawebwidget.h"
#include "PicasawebNewAlbumDialog.h"
#include "picasaweblogin.h"
#include "picasawebwindow.h"
#include "picasawebwindow.moc"

namespace KIPIPicasawebExportPlugin
{

PicasawebWindow::PicasawebWindow(KIPI::Interface* interface, const QString &tmpFolder, QWidget* /*parent*/)
               : KDialogBase(0, 0, false, i18n("Export to Picasa Web Service"), Help|Close, Close, false), 
                 m_tmp(tmpFolder)
{
    m_interface              = interface;
    m_uploadCount            = 0;
    m_uploadTotal            = 0;
//  m_wallet                 = 0;
    m_urls                   = 0;
    m_widget                 = new PicasawebWidget(this);
    m_tagView                = m_widget->m_tagView;
    m_photoView              = m_widget->m_photoView;
    m_newAlbumButton         = m_widget->m_newAlbumButton;
    m_addPhotoButton         = m_widget->m_selectPhotosButton;
    m_albumsListComboBox     = m_widget->m_albumsListComboBox;
    m_dimensionSpinBox       = m_widget->m_dimensionSpinBox;
    m_imageQualitySpinBox    = m_widget->m_imageQualitySpinBox;
    m_resizeCheckBox         = m_widget->m_resizeCheckBox;
    m_tagsLineEdit           = m_widget->m_tagsLineEdit;
    m_exportApplicationTags  = m_widget->m_exportApplicationTags;
    m_startUploadButton      = m_widget->m_startUploadButton;
    m_changeUserButton       = m_widget->m_changeUserButton;
    m_userNameDisplayLabel   = m_widget->m_userNameDisplayLabel;
    m_reloadAlbumsListButton = m_widget->m_reloadAlbumsListButton;

    setMainWidget(m_widget);
    m_widget->setMinimumSize(620, 300);
    m_widget->m_currentSelectionButton->setChecked(true);

    if(!m_interface->hasFeature(KIPI::HostSupportsTags))
        m_exportApplicationTags->setEnabled(false);

    // ------------------------------------------------------------

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Picasaweb Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to export image collection to "
                                                     "Picasaweb web service."),
                                           "(c) 2007-2008, Vardhman Jain\n"
                                           "(c) 2008, Gilles Caulier");

    m_about->addAuthor("Vardhman Jain", I18N_NOOP("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Developer"),
                       "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup(helpMenu->menu());

    // ------------------------------------------------------------

    m_talker = new PicasawebTalker(this);

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy( bool)));

    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(m_talker, SIGNAL(signalGetAlbumsListSucceeded()),
            this, SLOT(slotGetAlbumsListSucceeded()));

    connect(m_talker, SIGNAL(signalGetAlbumsListFailed(const QString&)),
            this, SLOT(slotGetAlbumsListFailed(const QString&)));

    connect(m_talker, SIGNAL( signalAddPhotoFailed(const QString&)),
            this, SLOT(slotAddPhotoFailed(const QString&)));

//    connect(m_talker, SIGNAL( signalListPhotoSetsSucceeded( const QValueList<FPhotoSet>& ) ),
//            this, SLOT( slotListPhotoSetsResponse( const QValueList<FPhotoSet>& ) ) );

    // ------------------------------------------------------------

    m_progressDlg = new QProgressDialog(this, 0, true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL(canceled()),
            this, SLOT(slotAddPhotoCancel()));

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotUserChangeRequest()));

    connect(m_reloadAlbumsListButton, SIGNAL(clicked()),
            this, SLOT(slotUpdateAlbumsList()));

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotCreateNewAlbum()));

    connect(m_talker, SIGNAL(signalTokenObtained(const QString&)),
            this, SLOT(slotTokenObtained(const QString&)));

    connect(m_addPhotoButton, SIGNAL(clicked()),
            this, SLOT(slotAddPhotos()));

    connect(m_startUploadButton, SIGNAL(clicked()),
            this, SLOT(slotUploadImages()));

    connect(m_resizeCheckBox, SIGNAL(toggled(bool )),
            this, SLOT(slotRefreshSizeButtons(bool)));

    // ------------------------------------------------------------
    // read config

    KSimpleConfig config("kipirc");
    config.setGroup("PicasawebExport Settings");
    m_token = config.readEntry("token");
    QString username = config.readEntry("username");
    QString password = config.readEntry("password");

    //no saving password rt now
    if (config.readBoolEntry("Resize", false))
        m_resizeCheckBox->setChecked(true);

    m_dimensionSpinBox->setValue(config.readNumEntry("Maximum Width", 1600));
    m_imageQualitySpinBox->setValue(config.readNumEntry("Image Quality", 85));

    // ------------------------------------------------------------

    m_authProgressDlg = new QProgressDialog( this, 0, true );
    m_authProgressDlg->setAutoReset( true );
    m_authProgressDlg->setAutoClose( true );

    connect(m_authProgressDlg, SIGNAL(canceled()),
            this, SLOT(slotAuthCancel()));

    m_talker->authProgressDlg = m_authProgressDlg; 
    m_widget->setEnabled(false); 

    // All these three values can be null too.
    // If m_token is ot null, username would be not null too.
    // if (!(!m_token || m_token.length() < 1))
        //getToken(username, password);

    m_talker->authenticate(m_token, username, password);
}

void PicasawebWindow::slotRefreshSizeButtons(bool /*st*/)
{
    if(m_resizeCheckBox->isChecked())
    {
        m_dimensionSpinBox->setEnabled(true);
        m_imageQualitySpinBox->setEnabled(true);
    }
    else 
    {
        m_dimensionSpinBox->setEnabled(false);
        m_imageQualitySpinBox->setEnabled(false);
    }
}

void PicasawebWindow::slotUpdateAlbumsList()
{
    m_talker->listAllAlbums();
}

void PicasawebWindow::slotClose()
{
    //handle the close of the window that is calling the destructor.
    delete this;
}

PicasawebWindow::~PicasawebWindow()
{
#if KDE_IS_VERSION(3,2,0)
 //   if (m_wallet)
   //     delete m_wallet;
#endif

    // write config
    KSimpleConfig config("kipirc");
    config.setGroup("PicasawebExport Settings");
    config.writeEntry("token", m_token);
    config.writeEntry("username", m_username);
    config.writeEntry("Resize", m_resizeCheckBox->isChecked());
    config.writeEntry("Maximum Width",  m_dimensionSpinBox->value());
    config.writeEntry("Image Quality",  m_imageQualitySpinBox->value());

    if(m_urls!=NULL)
        delete m_urls;

    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;

    delete m_about;
}

void PicasawebWindow::getToken(QString& username, QString& password)
{
    PicasawebLogin *loginDialog = new PicasawebLogin(this, QString("LoginWindow"), username, password);
    if (!loginDialog)
    {
        return;
    }

    /*if (username!=NULL && username.length() > 0){
        kdDebug()<<"Showing stored username"<< username << endl;
        loginDialog->setUsername(username);
        if (password != NULL && password.length() > 0){
            kdDebug()<<"Showing stored password"<< password << endl;
            loginDialog->setPassword(password);
            kdDebug()<<"Showing stored password"<< password << endl;
        }
    }*/

    QString username_edit, password_edit;

    if (loginDialog->exec() == QDialog::Accepted)
    {
        username_edit = loginDialog->username();
        password_edit = loginDialog->password();
    }
    else 
    {
        //Return something which say authentication needed.
        return ;
    }
}

void PicasawebWindow::slotHelp()
{
    KApplication::kApplication()->invokeHelp("picasawebexport", "kipi-plugins");
}

void PicasawebWindow::slotGetAlbumsListSucceeded()
{
    if (m_talker && m_talker->m_albumsList)
    {
        QValueList <PicasaWebAlbum> *list = m_talker->m_albumsList;
        m_albumsListComboBox->clear();
        QValueList<PicasaWebAlbum>::iterator it = list->begin();

        while(it != list->end())
        {
            PicasaWebAlbum pwa=*it;
            QString name = pwa.title;
            m_albumsListComboBox->insertItem(name);
            it++;
        }
    }
}

void PicasawebWindow::slotDoLogin()
{
}

void PicasawebWindow::slotTokenObtained( const QString& token )
{
    m_token=token;
    m_username=m_talker->getUserName();
    m_userId=m_talker->getUserId();
    m_userNameDisplayLabel->setText(m_username);
    m_widget->setEnabled(true);
    m_talker->listAllAlbums();
}

void PicasawebWindow::slotBusy( bool val )
{
    if ( val )
    {
        setCursor(QCursor::WaitCursor);
    }
    else
    {
        setCursor(QCursor::ArrowCursor);
    }
}

void PicasawebWindow::slotError( const QString& msg )
{
    KMessageBox::error(this, msg);
}

void PicasawebWindow::slotUserChangeRequest()
{
    kdDebug()<<"Slot Change User Request "<<endl;
    m_talker->authenticate();
}

void PicasawebWindow::slotAuthCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

void PicasawebWindow::slotCreateNewAlbum()
{
    NewAlbumDialog *dlg = new NewAlbumDialog(kapp->activeWindow());
    dlg->m_dateAndTimeEdit->setDateTime(QDateTime::currentDateTime());
    QString test;
    int t = dlg->exec();

    if(t == QDialog::Accepted)
    {
        if (dlg->m_isPublicRadioButton->isChecked())
            test = QString("public");
        else
            test = QString("unlisted");

        m_talker->createAlbum(dlg->m_titleLineEdit->text(), dlg->m_descriptionTextBox->text(),
                              dlg->m_locationLineEdit->text(), dlg->m_dateAndTimeEdit->dateTime().toTime_t(), 
                              test, QString(), true);
    }
    else 
    {
        if (t == QDialog::Rejected)
        {
            kdDebug()<<"Album Creation cancelled" <<endl;
        }
    }
}
/*
void PicasawebWindow::slotPhotos( const QValueList<GPhoto>& photoList)
{
    // TODO
}

void PicasawebWindow::slotTagSelected()
{
    // TODO
}

void PicasawebWindow::slotOpenPhoto( const KURL& url )
{
    new KRun(url);
}
*/

void PicasawebWindow::slotListPhotoSetsResponse(const QValueList <FPhotoSet>& /*photoSetList*/)
{
}

void PicasawebWindow::slotAddPhotos()
{
    //m_talker->listPhotoSets();
    m_urls = new KURL::List(KIPI::ImageDialog::getImageURLs( this, m_interface ));
}

void PicasawebWindow::slotUploadImages()
{
   if(m_widget->m_currentSelectionButton->isChecked())
   {
        if (m_urls!=NULL)
            delete m_urls;

        m_urls=new KURL::List(m_interface->currentSelection().images());
   }

   if (m_urls == NULL || m_urls->isEmpty())
        return;

    typedef QPair<QString,FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (KURL::List::iterator it = m_urls->begin(); it != m_urls->end(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        FPhotoInfo temp;

        temp.title=info.title();
        temp.description=info.description();

        QStringList allTags;

        QStringList tagsFromDialog = QStringList::split(" ", m_tagsLineEdit->text(), false);
        QStringList::Iterator itTags;

        //Tags from the interface	
        itTags= tagsFromDialog.begin();

        while( itTags != tagsFromDialog.end() ) 
        {
            allTags.append( *itTags );
            ++itTags;
        }

        //Tags from the database
        QMap <QString, QVariant> attribs = info.attributes();
        QStringList tagsFromDatabase;

        if(m_exportApplicationTags->isChecked())
        {
            // tagsFromDatabase=attribs["tags"].asStringList();	
        }

        itTags = tagsFromDatabase.begin();

        while( itTags != tagsFromDatabase.end() ) 
        {
            allTags.append( *itTags );
            ++itTags;
        }

        itTags = allTags.begin();

        while( itTags != allTags.end() ) 
        {
            ++itTags;
        }

        temp.tags=allTags; 
        m_uploadQueue.append( Pair( (*it).path(), temp) );
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
}


void PicasawebWindow::slotAddPhotoNext()
{
    if ( m_uploadQueue.isEmpty() )
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        //slotAlbumSelected();
        return;
    }

    typedef QPair<QString,FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info=pathComments.second;
    m_uploadQueue.pop_front();

/*    int upload_image_size;
    int upload_image_quality;*/

    // Get the albums' Id from the name.
    QString albumId = "";
    QString selectedAlbumName = m_albumsListComboBox->currentText();

    QValueList<PicasaWebAlbum>::iterator it = m_talker->m_albumsList->begin();
    while(it != m_talker->m_albumsList->end()) {
       PicasaWebAlbum pwa=*it;
       QString name = pwa.title;
       if (name == selectedAlbumName) {
           albumId = pwa.id;
           break;
       }
       it++;
    }
	
    bool res = m_talker->addPhoto(pathComments.first,          //the file path
                                  info, albumId,
                                  m_resizeCheckBox->isChecked(),
                                  m_dimensionSpinBox->value(), m_imageQualitySpinBox->value());
    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1 ").arg( KURL(pathComments.first).filename()));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void PicasawebWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setProgress( m_uploadCount, m_uploadTotal );
    slotAddPhotoNext();
}

void PicasawebWindow::slotAddPhotoFailed(const QString& msg)
{
    if ( KMessageBox::warningContinueCancel(this,
           i18n("Failed to upload photo into Picasaweb. %1\nDo you want to continue?")
                .arg( msg )) != KMessageBox::Continue)
    {
        m_uploadQueue.clear();
        m_progressDlg->reset();
        m_progressDlg->hide();
        // refresh the thumbnails
        //slotTagSelected();
    }
    else
    {
        m_uploadTotal--;
        m_progressDlg->setProgress(m_uploadCount, m_uploadTotal);
        slotAddPhotoNext();
    }
}

void PicasawebWindow::slotGetAlbumsListFailed(const QString& /*msg*/)
{
    // Raise some errors
}

void PicasawebWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();

    // refresh the thumbnails
    //slotTagSelected();
}

} // namespace KIPIPicasawebExportPlugin
