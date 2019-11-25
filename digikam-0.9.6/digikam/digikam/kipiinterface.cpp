/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-02
 * Description : digiKam kipi library interface.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2005 by Ralf Holzer <ralf at well.com>
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// C Ansi includes

extern "C"
{
#include <sys/types.h>
#include <utime.h>
}

// Qt includes.

#include <qdir.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qregexp.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kfilemetainfo.h>
#include <kio/netaccess.h>

// libKipi includes.

#include <libkipi/version.h>

// Local includes.

#include "constants.h"
#include "ddebug.h"
#include "albummanager.h"
#include "albumitemhandler.h"
#include "album.h"
#include "albumdb.h"
#include "albumsettings.h"
#include "dmetadata.h"
#include "imageattributeswatch.h"
#include "kipiinterface.h"
#include "kipiinterface.moc"

namespace Digikam
{

//-- Image Info ------------------------------------------------------------------

DigikamImageInfo::DigikamImageInfo( KIPI::Interface* interface, const KURL& url )
                : KIPI::ImageInfoShared( interface, url ), palbum_(0)
{
}

DigikamImageInfo::~DigikamImageInfo()
{
}

PAlbum* DigikamImageInfo::parentAlbum()
{
    if (!palbum_)
    {
        KURL u(_url.directory());
        palbum_ = AlbumManager::instance()->findPAlbum(u);
    }
    return palbum_;
}

QString DigikamImageInfo::title()
{
    return _url.fileName();
}

QString DigikamImageInfo::description()
{
    PAlbum* p = parentAlbum();

    if (p)
    {
        AlbumDB* db = AlbumManager::instance()->albumDB();
        return db->getItemCaption(p->id(), _url.fileName());
    }

    return QString();
}

void DigikamImageInfo::setTitle( const QString& newName )
{
    // Here we get informed that a plugin has renamed the file 
    
    PAlbum* p = parentAlbum();

    if ( p && !newName.isEmpty() )
    {
        AlbumDB* db = AlbumManager::instance()->albumDB();
        db->moveItem(p->id(), _url.fileName(), p->id(), newName);
        _url = _url.upURL();
        _url.addPath(newName);
    }
}

void DigikamImageInfo::setDescription( const QString& description )
{
    PAlbum* p = parentAlbum();

    if ( p  )
    {
        AlbumDB* db = AlbumManager::instance()->albumDB();
        Q_LLONG imageId = db->getImageId(p->id(), _url.filename());
        db->setItemCaption(imageId, description);
        ImageAttributesWatch::instance()->imageCaptionChanged(imageId);

        // See B.K.O #140133. Do not set here metadata comments of picture.
        // Only the database comments must be set.
    }
}

QDateTime DigikamImageInfo::time( KIPI::TimeSpec /*spec*/ )
{
    PAlbum* p = parentAlbum();

    if (p)
    {
        AlbumDB* db = AlbumManager::instance()->albumDB();
        return db->getItemDate(p->id(), _url.fileName());
    }

    return QDateTime();
}

void DigikamImageInfo::setTime(const QDateTime& time, KIPI::TimeSpec)
{
    if ( !time.isValid() )
    {
        DWarning() << k_funcinfo << "Invalid datetime specified" << endl;
        return;
    }

    PAlbum* p = parentAlbum();

    if ( p )
    {
        AlbumDB* db = AlbumManager::instance()->albumDB();
        Q_LLONG imageId = db->getImageId(p->id(), _url.filename());
        db->setItemDate(imageId, time);
        ImageAttributesWatch::instance()->imageDateChanged(imageId);
        AlbumManager::instance()->refreshItemHandler( _url );
    }
}

void DigikamImageInfo::cloneData( ImageInfoShared* other )
{
    setDescription( other->description() );
    setTime( other->time(KIPI::FromInfo), KIPI::FromInfo );
    addAttributes( other->attributes() );
}

QMap<QString, QVariant> DigikamImageInfo::attributes()
{
    QMap<QString, QVariant> res;

    PAlbum* p = parentAlbum();
    if (p)
    {
        AlbumDB* db      = AlbumManager::instance()->albumDB();
        Q_LLONG imageId  = db->getImageId(p->id(), _url.filename());
        
        // Get digiKam Tags list of picture.
        QStringList tags         = db->getItemTagNames(imageId); 
        res["tags"]              = tags;

        // Get digiKam Rating of picture.
        int rating               = db->getItemRating(imageId); 
        res["rating"]            = rating;

        // TODO: add here future picture attributes stored by digiKam database
    }
    return res;
}

void DigikamImageInfo::addAttributes(const QMap<QString, QVariant>& res)
{
    PAlbum* p = parentAlbum();
    if (p)
    {
        AlbumDB* db                        = AlbumManager::instance()->albumDB();
        Q_LLONG imageId                    = db->getImageId(p->id(), _url.filename());
        QMap<QString, QVariant> attributes = res;
        
        // Set digiKam Tags list of picture.
        if (attributes.find("tags") != attributes.end())
        {
            QStringList tags = attributes["tags"].asStringList();
            //TODO
        }

        // Set digiKam Rating of picture.
        if (attributes.find("rating") != attributes.end())
        {
            int rating = attributes["rating"].asInt();
	    if (rating >= RatingMin && rating <= RatingMax)
                db->setItemRating(imageId, rating); 
        }

        // TODO: add here future picture attributes stored by digiKam database
    }

    // To update sidebar content. Some kipi-plugins use this way to refresh sidebar 
    // using an empty QMap(). 
    ImageAttributesWatch::instance()->fileMetadataChanged(_url);
}

void DigikamImageInfo::clearAttributes()
{
    // TODO ! This will used for the futures tags digiKam features.
}

int DigikamImageInfo::angle()
{
    AlbumSettings *settings = AlbumSettings::instance();
    if (settings->getExifRotate())
    {
        DMetadata metadata(_url.path());
        DMetadata::ImageOrientation orientation = metadata.getImageOrientation();
        
        switch (orientation) 
        {
            case DMetadata::ORIENTATION_ROT_180:
                return 180;
            case DMetadata::ORIENTATION_ROT_90:
            case DMetadata::ORIENTATION_ROT_90_HFLIP:
            case DMetadata::ORIENTATION_ROT_90_VFLIP:
                return 90;
            case DMetadata::ORIENTATION_ROT_270:
                return 270;
            default:
                return 0;
        }
    }
    
    return 0;
}

void DigikamImageInfo::setAngle( int )
{
    // TODO : add here a DMetadata call (thru Exiv2) to set Exif orientation tag.
}

//-- Image Collection ------------------------------------------------------------

DigikamImageCollection::DigikamImageCollection( Type tp, Album* album, const QString& filter )
                      : tp_( tp ), album_(album), imgFilter_(filter)
{
    if (!album)
    {
        DWarning() << k_funcinfo << "This should not happen. No album specified" << endl;
    }
}

DigikamImageCollection::~DigikamImageCollection()
{
}

QString DigikamImageCollection::name()
{
    if ( album_->type() == Album::TAG )
    {
        return i18n("Tag: %1").arg(album_->title());
    }
    else
        return album_->title();
}

QString DigikamImageCollection::category()
{
    if ( album_->type() == Album::PHYSICAL )
    {
        PAlbum *p = dynamic_cast<PAlbum*>(album_);
        return p->collection();
    }
    else if ( album_->type() == Album::TAG )
    {
        TAlbum *p = dynamic_cast<TAlbum*>(album_);
        return i18n("Tag: %1").arg(p->tagPath());
    }
    else
        return QString();
}

QDate DigikamImageCollection::date()
{
    if ( album_->type() == Album::PHYSICAL )
    {
        PAlbum *p = dynamic_cast<PAlbum*>(album_);
        return p->date();
    }
    else
        return QDate();
}

QString DigikamImageCollection::comment()
{
    if ( album_->type() == Album::PHYSICAL )
    {
        PAlbum *p = dynamic_cast<PAlbum*>(album_);
        return p->caption();
    }
    else
        return QString();
}

static QValueList<QRegExp> makeFilterList( const QString &filter )
{
    QValueList<QRegExp> regExps;
    if ( filter.isEmpty() )
        return regExps;

    QChar sep( ';' );
    int i = filter.find( sep, 0 );

    if ( i == -1 && filter.find( ' ', 0 ) != -1 )
        sep = QChar( ' ' );

    QStringList list = QStringList::split( sep, filter );
    QStringList::Iterator it = list.begin();
    
    while ( it != list.end() ) 
    {
        regExps << QRegExp( (*it).stripWhiteSpace(), false, true );
        ++it;
    }
    
    return regExps;
}

static bool matchFilterList( const QValueList<QRegExp>& filters, const QString &fileName )
{
    QValueList<QRegExp>::ConstIterator rit = filters.begin();
    
    while ( rit != filters.end() ) 
    {
        if ( (*rit).exactMatch(fileName) )
            return true;
        ++rit;
    }

    return false;
}

KURL::List DigikamImageCollection::images()
{
    switch ( tp_ )
    {
        case AllItems:
        {
            if (album_->type() == Album::PHYSICAL)
            {
                return imagesFromPAlbum(dynamic_cast<PAlbum*>(album_));
            }
            else if (album_->type() == Album::TAG)
            {
                return imagesFromTAlbum(dynamic_cast<TAlbum*>(album_));
            }
            else if (album_->type() == Album::DATE || 
                    album_->type() == Album::SEARCH)
            {
                AlbumItemHandler* handler = AlbumManager::instance()->getItemHandler();
    
                if (handler)
                {
                    return handler->allItems();
                }
    
                return KURL::List();
        }
            else
            {
                DWarning() << k_funcinfo << "Unknown album type" << endl;
                return KURL::List();
            }
    
            break;
        }
        case SelectedItems:
        {
            AlbumItemHandler* handler = AlbumManager::instance()->getItemHandler();
    
            if (handler)
            {
                return handler->selectedItems();
            }
    
            return KURL::List();
    
            break;
        }
        default:
            break;
    }

    // We should never reach here
    return KURL::List();
}

/** get the images from the Physical album in database and return the items found */

KURL::List DigikamImageCollection::imagesFromPAlbum(PAlbum* album) const
{
    // get the images from the database and return the items found

    AlbumDB* db = AlbumManager::instance()->albumDB();

    db->beginTransaction();

    QStringList urls = db->getItemURLsInAlbum(album->id());

    db->commitTransaction();

    KURL::List urlList;

    QValueList<QRegExp> regex = makeFilterList(imgFilter_);
    
    for (QStringList::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        if (matchFilterList(regex, *it))
            urlList.append(*it);
    }

    return urlList;
}

/** get the images from the Tags album in database and return the items found */

KURL::List DigikamImageCollection::imagesFromTAlbum(TAlbum* album) const
{
    AlbumDB* db = AlbumManager::instance()->albumDB();

    db->beginTransaction();

    QStringList urls = db->getItemURLsInTag(album->id());

    db->commitTransaction();

    KURL::List urlList;

    QValueList<QRegExp> regex = makeFilterList(imgFilter_);
    
    for (QStringList::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        if (matchFilterList(regex, *it))
            urlList.append(*it);
    }

    return urlList;
}

KURL DigikamImageCollection::path()
{
    if (album_->type() == Album::PHYSICAL)
    {
        PAlbum *p = dynamic_cast<PAlbum*>(album_);
        KURL url;
        url.setPath(p->folderPath());
        return url;
    }
    else
    {
        DWarning() << k_funcinfo << "Requesting kurl from a virtual album" << endl;
        return QString();
    }
}

KURL DigikamImageCollection::uploadPath()
{
    if (album_->type() == Album::PHYSICAL)
    {
        PAlbum *p = dynamic_cast<PAlbum*>(album_);
        KURL url;
        url.setPath(p->folderPath());
        return url;
    }
    else
    {
        DWarning() << k_funcinfo << "Requesting kurl from a virtual album" << endl;
        return KURL();
    }
}

KURL DigikamImageCollection::uploadRoot()
{
    return KURL(AlbumManager::instance()->getLibraryPath() + '/');
}

QString DigikamImageCollection::uploadRootName()
{
    return i18n("My Albums");
}

bool DigikamImageCollection::isDirectory()
{
    if (album_->type() == Album::PHYSICAL)
        return true;
    else
        return false;
}

bool DigikamImageCollection::operator==(ImageCollectionShared& imgCollection)
{
    DigikamImageCollection* thatCollection = static_cast<DigikamImageCollection*>(&imgCollection);
    return (album_ == thatCollection->album_);
}

//-- LibKipi interface -----------------------------------------------------------

DigikamKipiInterface::DigikamKipiInterface( QObject *parent, const char *name)
                    : KIPI::Interface( parent, name )
{
    albumManager_ = AlbumManager::instance();
    albumDB_      = albumManager_->albumDB();

    connect( albumManager_, SIGNAL( signalAlbumItemsSelected( bool ) ),
             this, SLOT( slotSelectionChanged( bool ) ) );

    connect( albumManager_, SIGNAL( signalAlbumCurrentChanged(Album*) ),
             this, SLOT( slotCurrentAlbumChanged(Album*) ) );
}

DigikamKipiInterface::~DigikamKipiInterface()
{
}

KIPI::ImageCollection DigikamKipiInterface::currentAlbum()
{
    Album* currAlbum = albumManager_->currentAlbum();
    if ( currAlbum )
    {
        return KIPI::ImageCollection(
            new DigikamImageCollection( DigikamImageCollection::AllItems,
                                        currAlbum, fileExtensions() ) );
    }
    else
    {
        return KIPI::ImageCollection(0);
    }
}

KIPI::ImageCollection DigikamKipiInterface::currentSelection()
{
    Album* currAlbum = albumManager_->currentAlbum();
    if ( currAlbum )
    {
        return KIPI::ImageCollection(
            new DigikamImageCollection( DigikamImageCollection::SelectedItems,
                                        currAlbum, fileExtensions() ) );
    }
    else
    {
        return KIPI::ImageCollection(0);
    }
}

QValueList<KIPI::ImageCollection> DigikamKipiInterface::allAlbums()
{
    QValueList<KIPI::ImageCollection> result;

    QString fileFilter(fileExtensions());

    AlbumList palbumList = albumManager_->allPAlbums();
    for ( AlbumList::Iterator it = palbumList.begin();
          it != palbumList.end(); ++it )
    {
        // don't add the root album
        if ((*it)->isRoot())
            continue;

        DigikamImageCollection* col = new DigikamImageCollection( DigikamImageCollection::AllItems,
                                                                  *it, fileFilter );
        result.append( KIPI::ImageCollection( col ) );
    }

    AlbumList talbumList = albumManager_->allTAlbums();
    for ( AlbumList::Iterator it = talbumList.begin();
          it != talbumList.end(); ++it )
    {
        // don't add the root album
        if ((*it)->isRoot())
            continue;

        DigikamImageCollection* col = new DigikamImageCollection( DigikamImageCollection::AllItems,
                                                                  *it, fileFilter );   
        result.append( KIPI::ImageCollection( col ) );
    }

    return result;
}

KIPI::ImageInfo DigikamKipiInterface::info( const KURL& url )
{
    return KIPI::ImageInfo( new DigikamImageInfo( this, url ) );
}

void DigikamKipiInterface::refreshImages( const KURL::List& urls )
{
    KURL::List ulist = urls;

    // Re-scan metadata from pictures. This way will update Metadata sidebar and database.
    for ( KURL::List::Iterator it = ulist.begin() ; it != ulist.end() ; ++it )
        ImageAttributesWatch::instance()->fileMetadataChanged(*it);
    
    // Refresh preview.
    albumManager_->refreshItemHandler(urls);
}

int DigikamKipiInterface::features() const
{
    return (

// HostSupportsTags feature require libkipi version > 1.4.0
#if KIPI_VERSION>0x000104
           KIPI::HostSupportsTags |
#endif
           KIPI::ImagesHasComments          | KIPI::AcceptNewImages            |
           KIPI::AlbumsHaveComments         | KIPI::ImageTitlesWritable        |
           KIPI::ImagesHasTime              | KIPI::AlbumsHaveCategory         |
           KIPI::AlbumsHaveCreationDate     | KIPI::AlbumsUseFirstImagePreview 
           );
}

bool DigikamKipiInterface::addImage( const KURL& url, QString& errmsg )
{
    // Nota : All copy/move operations are processed by the plugins.

    if ( url.isValid() == false )
    {
        errmsg = i18n("Target URL %1 is not valid.").arg(url.path());
        return false;
    }

    PAlbum *targetAlbum = albumManager_->findPAlbum(url.directory());

    if ( !targetAlbum )
    {
        errmsg = i18n("Target album is not in the album library.");
        return false;
    }

    albumManager_->refreshItemHandler( url );

    return true;
}

void DigikamKipiInterface::delImage( const KURL& url )
{
    KURL rootURL(albumManager_->getLibraryPath());
    if ( !rootURL.isParentOf(url) )
    {
        DWarning() << k_funcinfo << "URL not in the album library" << endl;
    }

    // Is there a PAlbum for this url

    PAlbum *palbum = albumManager_->findPAlbum( KURL(url.directory()) );

    if ( palbum )
    {
        // delete the item from the database
        albumDB_->deleteItem( palbum->id(), url.fileName() );
    }
    else
    {
        DWarning() << k_funcinfo << "Cannot find Parent album in album library" << endl;
    }
}

void DigikamKipiInterface::slotSelectionChanged( bool b )
{
    emit selectionChanged( b );
}

void DigikamKipiInterface::slotCurrentAlbumChanged( Album *album )
{
    emit currentAlbumChanged( album != 0 );
}

QString DigikamKipiInterface::fileExtensions()
{
    // do not save this into a local variable, as this
    // might change in the main app

    AlbumSettings* s = AlbumSettings::instance();
    return (s->getImageFileFilter() + ' ' +
            s->getMovieFileFilter() + ' ' +
            s->getAudioFileFilter() + ' ' +
            s->getRawFileFilter());
}

}  // namespace Digikam

