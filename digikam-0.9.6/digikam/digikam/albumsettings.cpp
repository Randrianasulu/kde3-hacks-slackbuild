/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-16-10
 * Description : albums settings interface
 *
 * Copyright (C) 2003-2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu> 
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2007 by Arnd Baecker <arnd dot baecker at web dot de>
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

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kconfig.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>

// LibKDcraw includes.

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000106
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes.

#include "thumbnailsize.h"
#include "mimefilter.h"
#include "albumlister.h"
#include "albumsettings.h"

namespace Digikam
{

class AlbumSettingsPrivate 
{

public:

    bool                                 showSplash;
    bool                                 useTrash;
    bool                                 showTrashDeleteDialog;
    bool                                 sidebarApplyDirectly;
    bool                                 scanAtStart;
    bool                                 recursiveAlbums;
    bool                                 recursiveTags;

    bool                                 iconShowName;
    bool                                 iconShowSize;
    bool                                 iconShowDate;
    bool                                 iconShowModDate;
    bool                                 iconShowComments;
    bool                                 iconShowResolution;
    bool                                 iconShowTags;
    bool                                 iconShowRating;

    bool                                 showToolTips;
    bool                                 tooltipShowFileName;
    bool                                 tooltipShowFileDate;
    bool                                 tooltipShowFileSize;
    bool                                 tooltipShowImageType;
    bool                                 tooltipShowImageDim;
    bool                                 tooltipShowPhotoMake;
    bool                                 tooltipShowPhotoDate;
    bool                                 tooltipShowPhotoFocal;
    bool                                 tooltipShowPhotoExpo;
    bool                                 tooltipShowPhotoMode;
    bool                                 tooltipShowPhotoFlash;
    bool                                 tooltipShowPhotoWb;
    bool                                 tooltipShowAlbumName;
    bool                                 tooltipShowComments;
    bool                                 tooltipShowTags;
    bool                                 tooltipShowRating;

    bool                                 exifRotate;
    bool                                 exifSetOrientation;

    bool                                 saveIptcTags;
    bool                                 saveIptcPhotographerId;
    bool                                 saveIptcCredits;

    bool                                 saveComments;
    bool                                 saveDateTime;
    bool                                 saveRating;

    bool                                 previewLoadFullImageSize;

    bool                                 showFolderTreeViewItemsCount;

    int                                  thumbnailSize;
    int                                  treeThumbnailSize;
    int                                  ratingFilterCond;

    QString                              currentTheme;
    QString                              albumLibraryPath;
    QString                              imageFilefilter;
    QString                              movieFilefilter;
    QString                              audioFilefilter;
    QString                              rawFilefilter;
    QString                              defaultImageFilefilter;
    QString                              defaultMovieFilefilter;
    QString                              defaultAudioFilefilter;
    QString                              defaultRawFilefilter;

    QString                              author;
    QString                              authorTitle;
    QString                              credit;
    QString                              source;
    QString                              copyright;

    QStringList                          albumCollectionNames;

    KConfig                             *config;

    AlbumSettings::AlbumSortOrder        albumSortOrder;
    AlbumSettings::ImageSortOrder        imageSortOrder;
    AlbumSettings::ItemRightClickAction  itemRightClickAction;
};


AlbumSettings* AlbumSettings::m_instance = 0;

AlbumSettings* AlbumSettings::instance()
{
    return m_instance;
}

AlbumSettings::AlbumSettings()
{
    d = new AlbumSettingsPrivate;
    d->config  = kapp->config();
    m_instance = this;
    init();
}

AlbumSettings::~AlbumSettings()
{
    delete d;
    m_instance = 0;
}

void AlbumSettings::init()
{
    d->albumCollectionNames.clear();
    d->albumCollectionNames.append(i18n("Family"));
    d->albumCollectionNames.append(i18n("Travel"));
    d->albumCollectionNames.append(i18n("Holidays"));
    d->albumCollectionNames.append(i18n("Friends"));
    d->albumCollectionNames.append(i18n("Nature"));
    d->albumCollectionNames.append(i18n("Party"));
    d->albumCollectionNames.append(i18n("Todo"));
    d->albumCollectionNames.append(i18n("Miscellaneous"));
    d->albumCollectionNames.sort();

    d->albumSortOrder               = AlbumSettings::ByFolder;
    d->imageSortOrder               = AlbumSettings::ByIName;
    d->itemRightClickAction         = AlbumSettings::ShowPreview;

    d->defaultImageFilefilter       = "*.jpg *.jpeg *.jpe "               // JPEG
                                      "*.jp2 *.jpx *.jpc *.pgx "          // JPEG-2000
                                      "*.tif *.tiff "                     // TIFF
                                      "*.png *.gif *.bmp *.xpm *.ppm *.pnm *.xcf *.pcx";

    d->defaultMovieFilefilter       = "*.mpeg *.mpg *.mpo *.mpe "         // MPEG
                                      "*.avi *.mov *.wmf *.asf *.mp4 *.3gp";

    d->defaultAudioFilefilter       = "*.ogg *.mp3 *.wma *.wav";

    // RAW files estentions supported by dcraw program and 
    // defines to digikam/libs/dcraw/rawfiles.h
#if KDCRAW_VERSION < 0x000106
    d->defaultRawFilefilter         = QString(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    d->defaultRawFilefilter         = QString(KDcrawIface::KDcraw::rawFiles());
#endif

    d->imageFilefilter              = d->defaultImageFilefilter;
    d->movieFilefilter              = d->defaultMovieFilefilter;
    d->audioFilefilter              = d->defaultAudioFilefilter;
    d->rawFilefilter                = d->defaultRawFilefilter;

    d->thumbnailSize                = ThumbnailSize::Medium;
    d->treeThumbnailSize            = 22;

    d->ratingFilterCond             = AlbumLister::GreaterEqualCondition;

    d->showToolTips                 = true;
    d->showSplash                   = true;
    d->useTrash                     = true;
    d->showTrashDeleteDialog        = true;
    d->sidebarApplyDirectly         = false;

    d->iconShowName                 = false;
    d->iconShowSize                 = false;
    d->iconShowDate                 = true;
    d->iconShowModDate              = true;
    d->iconShowComments             = true;
    d->iconShowResolution           = false;
    d->iconShowTags                 = true;
    d->iconShowRating               = true;

    d->tooltipShowFileName          = true;
    d->tooltipShowFileDate          = false;
    d->tooltipShowFileSize          = false;
    d->tooltipShowImageType         = false;
    d->tooltipShowImageDim          = true;
    d->tooltipShowPhotoMake         = true;
    d->tooltipShowPhotoDate         = true;
    d->tooltipShowPhotoFocal        = true;
    d->tooltipShowPhotoExpo         = true;
    d->tooltipShowPhotoMode         = true;
    d->tooltipShowPhotoFlash        = false;
    d->tooltipShowPhotoWb           = false;
    d->tooltipShowAlbumName         = false;
    d->tooltipShowComments          = true;
    d->tooltipShowTags              = true;
    d->tooltipShowRating            = true;

    d->exifRotate                   = true;
    d->exifSetOrientation           = true;

    d->saveIptcTags                 = false;
    d->saveIptcPhotographerId       = false;
    d->saveIptcCredits              = false;

    d->saveComments                 = false;
    d->saveDateTime                 = false;
    d->saveRating                   = false;

    d->previewLoadFullImageSize     = false;
    
    d->recursiveAlbums              = false;
    d->recursiveTags                = true;

    d->showFolderTreeViewItemsCount = false;
}

void AlbumSettings::readSettings()
{
    KConfig* config = d->config;

    // ---------------------------------------------------------------------

    config->setGroup("Album Settings");

    d->albumLibraryPath             = config->readPathEntry("Album Path", QString());

    QStringList collectionList      = config->readListEntry("Album Collections");
    if (!collectionList.isEmpty())
    {
        collectionList.sort();
        d->albumCollectionNames = collectionList;
    }

    d->albumSortOrder               = AlbumSettings::AlbumSortOrder(config->readNumEntry("Album Sort Order",
                                                                    (int)AlbumSettings::ByFolder));

    d->imageSortOrder               = AlbumSettings::ImageSortOrder(config->readNumEntry("Image Sort Order",
                                                                    (int)AlbumSettings::ByIName));

    d->itemRightClickAction         = AlbumSettings::ItemRightClickAction(config->readNumEntry(
                                                                          "Item Right Click Action",
                                                                          (int)AlbumSettings::ShowPreview));

    d->imageFilefilter              = config->readEntry("File Filter", d->imageFilefilter);
    d->movieFilefilter              = config->readEntry("Movie File Filter", d->movieFilefilter);
    d->audioFilefilter              = config->readEntry("Audio File Filter", d->audioFilefilter);
    d->rawFilefilter                = config->readEntry("Raw File Filter", d->rawFilefilter);
    d->thumbnailSize                = config->readNumEntry("Default Icon Size", ThumbnailSize::Medium);
    d->treeThumbnailSize            = config->readNumEntry("Default Tree Icon Size", 22);
    d->currentTheme                 = config->readEntry("Theme", i18n("Default"));

    d->ratingFilterCond             = config->readNumEntry("Rating Filter Condition",
                                                           AlbumLister::GreaterEqualCondition);

    d->iconShowName                 = config->readBoolEntry("Icon Show Name", false); 
    d->iconShowResolution           = config->readBoolEntry("Icon Show Resolution", false);
    d->iconShowSize                 = config->readBoolEntry("Icon Show Size", false);
    d->iconShowDate                 = config->readBoolEntry("Icon Show Date", true);
    d->iconShowModDate              = config->readBoolEntry("Icon Show Modification Date", true);
    d->iconShowComments             = config->readBoolEntry("Icon Show Comments", true);
    d->iconShowTags                 = config->readBoolEntry("Icon Show Tags", true);
    d->iconShowRating               = config->readBoolEntry("Icon Show Rating", true);

    d->showToolTips                 = config->readBoolEntry("Show ToolTips", false);
    d->tooltipShowFileName          = config->readBoolEntry("ToolTips Show File Name", true);
    d->tooltipShowFileDate          = config->readBoolEntry("ToolTips Show File Date", false);
    d->tooltipShowFileSize          = config->readBoolEntry("ToolTips Show File Size", false);
    d->tooltipShowImageType         = config->readBoolEntry("ToolTips Show Image Type", false);
    d->tooltipShowImageDim          = config->readBoolEntry("ToolTips Show Image Dim", true);
    d->tooltipShowPhotoMake         = config->readBoolEntry("ToolTips Show Photo Make", true);
    d->tooltipShowPhotoDate         = config->readBoolEntry("ToolTips Show Photo Date", true);
    d->tooltipShowPhotoFocal        = config->readBoolEntry("ToolTips Show Photo Focal", true);
    d->tooltipShowPhotoExpo         = config->readBoolEntry("ToolTips Show Photo Expo", true);
    d->tooltipShowPhotoMode         = config->readBoolEntry("ToolTips Show Photo Mode", true);
    d->tooltipShowPhotoFlash        = config->readBoolEntry("ToolTips Show Photo Flash", false);
    d->tooltipShowPhotoWb           = config->readBoolEntry("ToolTips Show Photo WB", false);
    d->tooltipShowAlbumName         = config->readBoolEntry("ToolTips Show Album Name", false);
    d->tooltipShowComments          = config->readBoolEntry("ToolTips Show Comments", true);
    d->tooltipShowTags              = config->readBoolEntry("ToolTips Show Tags", true);
    d->tooltipShowRating            = config->readBoolEntry("ToolTips Show Rating", true);

    d->previewLoadFullImageSize     = config->readBoolEntry("Preview Load Full Image Size", false);

    d->recursiveAlbums              = config->readBoolEntry("Recursive Albums", false);
    d->recursiveTags                = config->readBoolEntry("Recursive Tags", true);

    d->showFolderTreeViewItemsCount = config->readBoolEntry("Show Folder Tree View Items Count", false);

    // ---------------------------------------------------------------------

    config->setGroup("EXIF Settings");

    d->exifRotate         = config->readBoolEntry("EXIF Rotate", true);
    d->exifSetOrientation = config->readBoolEntry("EXIF Set Orientation", true);

    // ---------------------------------------------------------------------

    config->setGroup("Metadata Settings");

    d->saveIptcTags           = config->readBoolEntry("Save IPTC Tags", false);
    d->saveIptcPhotographerId = config->readBoolEntry("Save IPTC Photographer ID", false);
    d->saveIptcCredits        = config->readBoolEntry("Save IPTC Credits", false);

    d->saveComments           = config->readBoolEntry("Save EXIF Comments", false);
    d->saveDateTime           = config->readBoolEntry("Save Date Time", false);
    d->saveRating             = config->readBoolEntry("Save Rating", false);

    d->author                 = config->readEntry("IPTC Author", QString());
    d->authorTitle            = config->readEntry("IPTC Author Title", QString());
    d->credit                 = config->readEntry("IPTC Credit", QString());
    d->source                 = config->readEntry("IPTC Source", QString());
    d->copyright              = config->readEntry("IPTC Copyright", QString());

    // ---------------------------------------------------------------------

    config->setGroup("General Settings");

    d->showSplash            = config->readBoolEntry("Show Splash", true);
    d->useTrash              = config->readBoolEntry("Use Trash", true);
    d->showTrashDeleteDialog = config->readBoolEntry("Show Trash Delete Dialog", true);
    d->sidebarApplyDirectly  = config->readBoolEntry("Apply Sidebar Changes Directly", false);
    d->scanAtStart           = config->readBoolEntry("Scan At Start", true);
}

void AlbumSettings::saveSettings()
{
    KConfig* config = d->config;

    // ---------------------------------------------------------------------

    config->setGroup("Album Settings");

    config->writePathEntry("Album Path", d->albumLibraryPath);
    config->writeEntry("Album Collections", d->albumCollectionNames);
    config->writeEntry("Album Sort Order", (int)d->albumSortOrder);
    config->writeEntry("Image Sort Order", (int)d->imageSortOrder);
    config->writeEntry("Item Right Click Action", (int)d->itemRightClickAction);
    config->writeEntry("File Filter", d->imageFilefilter);
    config->writeEntry("Movie File Filter", d->movieFilefilter);
    config->writeEntry("Audio File Filter", d->audioFilefilter);
    config->writeEntry("Raw File Filter", d->rawFilefilter);
    config->writeEntry("Default Icon Size", QString::number(d->thumbnailSize));
    config->writeEntry("Default Tree Icon Size", QString::number(d->treeThumbnailSize));
    config->writeEntry("Rating Filter Condition", d->ratingFilterCond);
    config->writeEntry("Theme", d->currentTheme);

    config->writeEntry("Icon Show Name", d->iconShowName);
    config->writeEntry("Icon Show Resolution", d->iconShowResolution);
    config->writeEntry("Icon Show Size", d->iconShowSize);
    config->writeEntry("Icon Show Date", d->iconShowDate);
    config->writeEntry("Icon Show Modification Date", d->iconShowModDate);
    config->writeEntry("Icon Show Comments", d->iconShowComments);
    config->writeEntry("Icon Show Tags", d->iconShowTags);
    config->writeEntry("Icon Show Rating", d->iconShowRating);

    config->writeEntry("Show ToolTips", d->showToolTips);
    config->writeEntry("ToolTips Show File Name", d->tooltipShowFileName);
    config->writeEntry("ToolTips Show File Date", d->tooltipShowFileDate);
    config->writeEntry("ToolTips Show File Size", d->tooltipShowFileSize);
    config->writeEntry("ToolTips Show Image Type", d->tooltipShowImageType);
    config->writeEntry("ToolTips Show Image Dim", d->tooltipShowImageDim);
    config->writeEntry("ToolTips Show Photo Make", d->tooltipShowPhotoMake);
    config->writeEntry("ToolTips Show Photo Date", d->tooltipShowPhotoDate);
    config->writeEntry("ToolTips Show Photo Focal", d->tooltipShowPhotoFocal);
    config->writeEntry("ToolTips Show Photo Expo", d->tooltipShowPhotoExpo);
    config->writeEntry("ToolTips Show Photo Mode", d->tooltipShowPhotoMode);
    config->writeEntry("ToolTips Show Photo Flash", d->tooltipShowPhotoFlash);
    config->writeEntry("ToolTips Show Photo WB", d->tooltipShowPhotoWb);
    config->writeEntry("ToolTips Show Album Name", d->tooltipShowAlbumName);
    config->writeEntry("ToolTips Show Comments", d->tooltipShowComments);
    config->writeEntry("ToolTips Show Tags", d->tooltipShowTags);
    config->writeEntry("ToolTips Show Rating", d->tooltipShowRating);

    config->writeEntry("Preview Load Full Image Size", d->previewLoadFullImageSize);

    config->writeEntry("Recursive Albums", d->recursiveAlbums);
    config->writeEntry("Recursive Tags", d->recursiveTags);

    config->writeEntry("Show Folder Tree View Items Count", d->showFolderTreeViewItemsCount);

    // ---------------------------------------------------------------------

    config->setGroup("EXIF Settings");

    config->writeEntry("EXIF Rotate", d->exifRotate);
    config->writeEntry("EXIF Set Orientation", d->exifSetOrientation);

    // ---------------------------------------------------------------------

    config->setGroup("Metadata Settings");

    config->writeEntry("Save IPTC Tags", d->saveIptcTags);
    config->writeEntry("Save IPTC Photographer ID", d->saveIptcPhotographerId);
    config->writeEntry("Save IPTC Credits", d->saveIptcCredits);

    config->writeEntry("Save EXIF Comments", d->saveComments);
    config->writeEntry("Save Date Time", d->saveDateTime);
    config->writeEntry("Save Rating", d->saveRating);

    config->writeEntry("IPTC Author", d->author);
    config->writeEntry("IPTC Author Title", d->authorTitle);
    config->writeEntry("IPTC Credit", d->credit);
    config->writeEntry("IPTC Source", d->source);
    config->writeEntry("IPTC Copyright", d->copyright);

    // ---------------------------------------------------------------------

    config->setGroup("General Settings");

    config->writeEntry("Show Splash", d->showSplash);
    config->writeEntry("Use Trash", d->useTrash);
    config->writeEntry("Show Trash Delete Dialog", d->showTrashDeleteDialog);
    config->writeEntry("Apply Sidebar Changes Directly", d->sidebarApplyDirectly);
    config->writeEntry("Scan At Start", d->scanAtStart);

    config->sync();
}

void AlbumSettings::setAlbumLibraryPath(const QString& path)
{
    d->albumLibraryPath = path;
}

QString AlbumSettings::getAlbumLibraryPath() const
{
    return d->albumLibraryPath;
}

void AlbumSettings::setShowSplashScreen(bool val)
{
    d->showSplash = val;
}

bool AlbumSettings::getShowSplashScreen() const
{
    return d->showSplash;
}

void AlbumSettings::setScanAtStart(bool val)
{
    d->scanAtStart = val;
}

bool AlbumSettings::getScanAtStart() const
{
    return d->scanAtStart;
}

void AlbumSettings::setAlbumCollectionNames(const QStringList& list)
{
    d->albumCollectionNames = list;
}

QStringList AlbumSettings::getAlbumCollectionNames()
{
    return d->albumCollectionNames;
}

bool AlbumSettings::addAlbumCollectionName(const QString& name)
{
    if (d->albumCollectionNames.contains(name))
        return false;

    d->albumCollectionNames.append(name);
    return true;
}

bool AlbumSettings::delAlbumCollectionName(const QString& name)
{
    uint count = d->albumCollectionNames.remove(name);
    return (count > 0) ? true : false;
}

void AlbumSettings::setAlbumSortOrder(const AlbumSettings::AlbumSortOrder order)
{
    d->albumSortOrder = order;
}

AlbumSettings::AlbumSortOrder AlbumSettings::getAlbumSortOrder() const
{
    return d->albumSortOrder;
}

void AlbumSettings::setImageSortOrder(const ImageSortOrder order)
{
    d->imageSortOrder = order;
}

AlbumSettings::ImageSortOrder AlbumSettings::getImageSortOrder() const
{
    return d->imageSortOrder;
}

void AlbumSettings::setItemRightClickAction(const ItemRightClickAction action)
{
    d->itemRightClickAction = action;
}

AlbumSettings::ItemRightClickAction AlbumSettings::getItemRightClickAction() const
{
    return d->itemRightClickAction;
}

void AlbumSettings::setImageFileFilter(const QString& filter)
{
    d->imageFilefilter = filter;
}

QString AlbumSettings::getImageFileFilter() const
{
    return d->imageFilefilter;
}

void AlbumSettings::setMovieFileFilter(const QString& filter)
{
    d->movieFilefilter = filter;
}

QString AlbumSettings::getMovieFileFilter() const
{
    return d->movieFilefilter;
}

void AlbumSettings::setAudioFileFilter(const QString& filter)
{
    d->audioFilefilter = filter;
}

QString AlbumSettings::getAudioFileFilter() const
{
    return d->audioFilefilter;
}

void AlbumSettings::setRawFileFilter(const QString& filter)
{
    d->rawFilefilter = filter;
}

QString AlbumSettings::getRawFileFilter() const
{
    return d->rawFilefilter;
}

bool AlbumSettings::addImageFileExtension(const QString& newExt)
{
    if ( QStringList::split(" ", d->imageFilefilter).contains(newExt) ||
         QStringList::split(" ", d->movieFilefilter).contains(newExt) ||
         QStringList::split(" ", d->audioFilefilter).contains(newExt) ||
         QStringList::split(" ", d->rawFilefilter  ).contains(newExt) )
         return false; 

    d->imageFilefilter = d->imageFilefilter + ' ' + newExt;
    return true;
}

QString AlbumSettings::getAllFileFilter() const
{
    return d->imageFilefilter + ' '
        +  d->movieFilefilter + ' '
        +  d->audioFilefilter + ' '
        +  d->rawFilefilter;
}

void AlbumSettings::setDefaultIconSize(int val)
{
    d->thumbnailSize = val;
}

int AlbumSettings::getDefaultIconSize() const
{
    return d->thumbnailSize;
}

void AlbumSettings::setDefaultTreeIconSize(int val)
{
    d->treeThumbnailSize = val;
}

int AlbumSettings::getDefaultTreeIconSize() const
{
    return ((d->treeThumbnailSize < 8) || (d->treeThumbnailSize > 48)) ? 48 : d->treeThumbnailSize;
}

void AlbumSettings::setRatingFilterCond(int val)
{
    d->ratingFilterCond = val;
}

int AlbumSettings::getRatingFilterCond() const
{
    return d->ratingFilterCond;
}

void AlbumSettings::setIconShowName(bool val)
{
    d->iconShowName = val;
}

bool AlbumSettings::getIconShowName() const
{
    return d->iconShowName;
}

void AlbumSettings::setIconShowSize(bool val)
{
    d->iconShowSize = val;
}

bool AlbumSettings::getIconShowSize() const
{
    return d->iconShowSize;
}

void AlbumSettings::setIconShowComments(bool val)
{
    d->iconShowComments = val;
}

bool AlbumSettings::getIconShowComments() const
{
    return d->iconShowComments;
}

void AlbumSettings::setIconShowResolution(bool val)
{
    d->iconShowResolution = val;
}

bool AlbumSettings::getIconShowResolution() const
{
    return d->iconShowResolution;
}

void AlbumSettings::setIconShowTags(bool val)
{
    d->iconShowTags = val;
}

bool AlbumSettings::getIconShowTags() const
{
    return d->iconShowTags;
}

void AlbumSettings::setIconShowDate(bool val)
{
    d->iconShowDate = val;
}

bool AlbumSettings::getIconShowDate() const
{
    return d->iconShowDate;
}

void AlbumSettings::setIconShowModDate(bool val)
{
    d->iconShowModDate = val;
}

bool AlbumSettings::getIconShowModDate() const
{
    return d->iconShowModDate;
}

void AlbumSettings::setIconShowRating(bool val)
{
    d->iconShowRating = val;
}

bool AlbumSettings::getIconShowRating() const
{
    return d->iconShowRating;
}

void AlbumSettings::setExifRotate(bool val)
{
    d->exifRotate = val;
}

bool AlbumSettings::getExifRotate() const
{
    return d->exifRotate;
}

void AlbumSettings::setExifSetOrientation(bool val)
{
    d->exifSetOrientation = val;
}

bool AlbumSettings::getExifSetOrientation() const
{
    return d->exifSetOrientation;
}

void AlbumSettings::setSaveIptcTags(bool val)
{
    d->saveIptcTags = val;
}

bool AlbumSettings::getSaveIptcTags() const
{
    return d->saveIptcTags;
}

void AlbumSettings::setSaveIptcPhotographerId(bool val)
{
    d->saveIptcPhotographerId = val;
}

bool AlbumSettings::getSaveIptcPhotographerId() const
{
    return d->saveIptcPhotographerId;
}

void AlbumSettings::setSaveIptcCredits(bool val)
{
    d->saveIptcCredits = val;
}

bool AlbumSettings::getSaveIptcCredits() const
{
    return d->saveIptcCredits;
}

void AlbumSettings::setIptcAuthor(const QString& author)
{
    d->author = author;
}

QString AlbumSettings::getIptcAuthor() const
{
    return d->author;
}

void AlbumSettings::setIptcAuthorTitle(const QString& authorTitle)
{
    d->authorTitle = authorTitle;
}

QString AlbumSettings::getIptcAuthorTitle() const
{
    return d->authorTitle;
}

void AlbumSettings::setIptcCredit(const QString& credit)
{
    d->credit = credit;
}

QString AlbumSettings::getIptcCredit() const
{
    return d->credit;
}

void AlbumSettings::setIptcSource(const QString& source)
{
    d->source = source;
}

QString AlbumSettings::getIptcSource() const
{
    return d->source;
}

void AlbumSettings::setIptcCopyright(const QString& copyright)
{
    d->copyright = copyright;
}

QString AlbumSettings::getIptcCopyright() const
{
    return d->copyright;
}

void AlbumSettings::setSaveComments(bool val)
{
    d->saveComments = val;
}

bool AlbumSettings::getSaveComments() const
{
    return d->saveComments;
}

void AlbumSettings::setSaveDateTime(bool val)
{
    d->saveDateTime = val;
}

bool AlbumSettings::getSaveDateTime() const
{
    return d->saveDateTime;
}

bool AlbumSettings::getSaveRating() const
{
    return d->saveRating;
}

void AlbumSettings::setSaveRating(bool val)
{
    d->saveRating = val;
}

void AlbumSettings::setShowToolTips(bool val)
{
    d->showToolTips = val;
}

bool AlbumSettings::getShowToolTips() const
{
    return d->showToolTips;
}

void AlbumSettings::setToolTipsShowFileName(bool val)
{
    d->tooltipShowFileName = val;
}

bool AlbumSettings::getToolTipsShowFileName() const
{
    return d->tooltipShowFileName;
}

void AlbumSettings::setToolTipsShowFileDate(bool val)
{
    d->tooltipShowFileDate = val;
}

bool AlbumSettings::getToolTipsShowFileDate() const
{
    return d->tooltipShowFileDate;
}

void AlbumSettings::setToolTipsShowFileSize(bool val)
{
    d->tooltipShowFileSize = val;
}

bool AlbumSettings::getToolTipsShowFileSize() const
{
    return d->tooltipShowFileSize;
}

void AlbumSettings::setToolTipsShowImageType(bool val)
{
    d->tooltipShowImageType = val;
}

bool AlbumSettings::getToolTipsShowImageType() const
{
    return d->tooltipShowImageType;
}

void AlbumSettings::setToolTipsShowImageDim(bool val)
{
    d->tooltipShowImageDim = val;
}

bool AlbumSettings::getToolTipsShowImageDim() const
{
    return d->tooltipShowImageDim;
}

void AlbumSettings::setToolTipsShowPhotoMake(bool val)
{
    d->tooltipShowPhotoMake = val;
}

bool AlbumSettings::getToolTipsShowPhotoMake() const
{
    return d->tooltipShowPhotoMake;
}

void AlbumSettings::setToolTipsShowPhotoDate(bool val)
{
    d->tooltipShowPhotoDate = val;
}

bool AlbumSettings::getToolTipsShowPhotoDate() const
{
    return d->tooltipShowPhotoDate;
}

void AlbumSettings::setToolTipsShowPhotoFocal(bool val)
{
    d->tooltipShowPhotoFocal = val;
}

bool AlbumSettings::getToolTipsShowPhotoFocal() const
{
    return d->tooltipShowPhotoFocal;
}

void AlbumSettings::setToolTipsShowPhotoExpo(bool val)
{
    d->tooltipShowPhotoExpo = val;
}

bool AlbumSettings::getToolTipsShowPhotoExpo() const
{
    return d->tooltipShowPhotoExpo;
}

void AlbumSettings::setToolTipsShowPhotoMode(bool val)
{
    d->tooltipShowPhotoMode = val;
}

bool AlbumSettings::getToolTipsShowPhotoMode() const
{
    return d->tooltipShowPhotoMode;
}

void AlbumSettings::setToolTipsShowPhotoFlash(bool val)
{
    d->tooltipShowPhotoFlash = val;
}

bool AlbumSettings::getToolTipsShowPhotoFlash() const
{
    return d->tooltipShowPhotoFlash;
}

void AlbumSettings::setToolTipsShowPhotoWB(bool val)
{
    d->tooltipShowPhotoWb = val;
}

bool AlbumSettings::getToolTipsShowPhotoWB() const
{
    return d->tooltipShowPhotoWb;
}

void AlbumSettings::setToolTipsShowAlbumName(bool val)
{
    d->tooltipShowAlbumName = val;
}

bool AlbumSettings::getToolTipsShowAlbumName() const
{
    return d->tooltipShowAlbumName;
}

void AlbumSettings::setToolTipsShowComments(bool val)
{
    d->tooltipShowComments = val;
}

bool AlbumSettings::getToolTipsShowComments() const
{
    return d->tooltipShowComments;
}

void AlbumSettings::setToolTipsShowTags(bool val)
{
    d->tooltipShowTags = val;
}

bool AlbumSettings::getToolTipsShowTags() const
{
    return d->tooltipShowTags;
}

void AlbumSettings::setToolTipsShowRating(bool val)
{
    d->tooltipShowRating = val;
}

bool AlbumSettings::getToolTipsShowRating() const
{
    return d->tooltipShowRating;
}

void AlbumSettings::setCurrentTheme(const QString& theme)
{
    d->currentTheme = theme;
}

QString AlbumSettings::getCurrentTheme() const
{
    return d->currentTheme;
}

void AlbumSettings::setUseTrash(bool val)
{
    d->useTrash = val;
}

bool AlbumSettings::getUseTrash() const
{
    return d->useTrash;
}

void AlbumSettings::setShowTrashDeleteDialog(bool val)
{
    d->showTrashDeleteDialog = val;
}

bool AlbumSettings::getShowTrashDeleteDialog() const
{
    return d->showTrashDeleteDialog;
}

void AlbumSettings::setApplySidebarChangesDirectly(bool val)
{
    d->sidebarApplyDirectly= val;
}

bool AlbumSettings::getApplySidebarChangesDirectly() const
{
    return d->sidebarApplyDirectly;
}

bool AlbumSettings::showToolTipsIsValid() const
{
    if (d->showToolTips)
    {
        if (d->tooltipShowFileName   ||
            d->tooltipShowFileDate   ||
            d->tooltipShowFileSize   ||
            d->tooltipShowImageType  ||
            d->tooltipShowImageDim   ||
            d->tooltipShowPhotoMake  ||
            d->tooltipShowPhotoDate  ||
            d->tooltipShowPhotoFocal ||
            d->tooltipShowPhotoExpo  ||
            d->tooltipShowPhotoMode  ||
            d->tooltipShowPhotoFlash ||
            d->tooltipShowPhotoWb    ||
            d->tooltipShowAlbumName  ||
            d->tooltipShowComments   ||
            d->tooltipShowTags       ||
            d->tooltipShowRating)
           return true;
    }

    return false;
}

QString AlbumSettings::getDefaultImageFileFilter() const
{
    return d->defaultImageFilefilter;
}

QString AlbumSettings::getDefaultMovieFileFilter() const
{
    return d->defaultMovieFilefilter;
}

QString AlbumSettings::getDefaultAudioFileFilter() const
{
    return d->defaultAudioFilefilter;
}

QString AlbumSettings::getDefaultRawFileFilter() const
{
    return d->defaultRawFilefilter;
}

void AlbumSettings::setPreviewLoadFullImageSize(bool val)
{
    d->previewLoadFullImageSize = val;
}

bool AlbumSettings::getPreviewLoadFullImageSize() const
{
    return d->previewLoadFullImageSize;
}

void AlbumSettings::setRecurseAlbums(bool val)
{
    d->recursiveAlbums = val;
}

bool AlbumSettings::getRecurseAlbums() const
{
    return d->recursiveAlbums;
}

void AlbumSettings::setRecurseTags(bool val)
{
    d->recursiveTags = val;
}

bool AlbumSettings::getRecurseTags() const
{
    return d->recursiveTags;
}

void AlbumSettings::setShowFolderTreeViewItemsCount(bool val)
{
    d->showFolderTreeViewItemsCount = val;
}

bool AlbumSettings::getShowFolderTreeViewItemsCount() const
{
    return d->showFolderTreeViewItemsCount;
}

}  // namespace Digikam
