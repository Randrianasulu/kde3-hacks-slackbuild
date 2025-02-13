/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef FLICKRTALKER_H
#define FLICKRTALKER_H

// Qt includes.

#include <qstring.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qprogressdialog.h>

// KDE includes.

#include <kurl.h>
#include <kio/job.h>

class QProgressDialog;

namespace KIPIFlickrExportPlugin
{

class GAlbum;
class GPhoto;
class FPhotoInfo;
class FPhotoSet;

class FlickrTalker : public QObject
{
    Q_OBJECT

public:

    enum State 
    {
        FE_LOGIN = 0,
        FE_LISTPHOTOSETS,
        FE_LISTPHOTOS,
        FE_GETPHOTOPROPERTY,
        FE_ADDPHOTO,
        FE_GETFROB,
        FE_CHECKTOKEN,
        FE_GETTOKEN,
        FE_GETAUTHORIZED
    };

public:

    FlickrTalker(QWidget* parent);
    ~FlickrTalker();

    QString getUserName();
    QString getUserId();
    void    getFrob();
    void    getToken();
    void    checkToken(const QString& token);
    void    getPhotoProperty(const QString& method, const QStringList& argList);
    void    cancel();

    void    listPhotoSets();
    void    listPhotos(const QString& albumName);
    void    createAlbum(const QString& parentAlbumName,
                        const QString& albumName,
                        const QString& albumTitle,
                        const QString& albumCaption);

    bool    addPhoto(const QString& photoPath, const FPhotoInfo& info,
                     bool rescale=false, int maxDim=600, int imageQuality=85);

public:

    QProgressDialog *m_authProgressDlg;

signals:

    void signalError(const QString& msg);
//  void signalLoginFailed( const QString& msg );
    void signalBusy(bool val);
    void signalAlbums(const QValueList<GAlbum>& albumList);
    void signalPhotos(const QValueList<GPhoto>& photoList);
    void signalAddPhotoSucceeded();
    void signalListPhotoSetsSucceeded(const QValueList <FPhotoSet>& photoSetList);
    void signalAddPhotoFailed(const QString& msg);
    void signalListPhotoSetsFailed(const QString& msg);
    void signalAuthenticate();
    void signalTokenObtained(const QString& token);

private:

//  void parseResponseLogin(const QByteArray& data);
    void parseResponseListPhotoSets(const QByteArray& data);
    void parseResponseListPhotos(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseGetFrob(const QByteArray& data);
    void parseResponseGetToken(const QByteArray& data);
    void parseResponseCheckToken(const QByteArray& data);
    void parseResponsePhotoProperty(const QByteArray& data);

    QString getApiSig(const QString& secret, const KURL& url);

private slots:

    void slotError(const QString& msg);
    void slotAuthenticate();
    void data(KIO::Job *job, const QByteArray& data);
    void slotResult(KIO::Job *job);

private:

    QWidget*   m_parent;

//  QString    m_cookie;
    QByteArray m_buffer;

    QString    m_apikey;
    QString    m_secret;
    QString    m_frob;
    QString    m_token;
    QString    m_username;
    QString    m_userId;

    KIO::Job*  m_job;

    State      m_state;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRTALKER_H */
