/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef SVEDIALOG_H
#define SVEDIALOG_H

// Include files for KDE

#include <kdialogbase.h>

// Include files for KIPI

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"

class QFrame;
class QCheckBox;
class QComboBox;

class KLineEdit;
class KIntNumInput;
class KColorButton;
class KURLRequester;
class KConfig;

namespace KIPI
{
    class ImageCollectionSelector;
}

namespace KIPISimpleViewerExportPlugin
{

class SVEDialog : public KDialogBase
{
    Q_OBJECT

public:

    SVEDialog( KIPI::Interface* interface, QWidget *parent=0);
    ~SVEDialog();

    void readConfig();
    void writeConfig();

    void slotOk();
    QValueList<KIPI::ImageCollection> getSelectedAlbums() const;

    QString title() const;
    void setTitle(const QString &title);

    bool resizeExportImages() const;
    void setResizeExportImages(bool resize);

    int imagesExportSize() const;
    void setImagesExportSize(int size);

    int maxImageDimension() const;
    void setMaxImagesDimension(int size);

    bool showExifComments() const;
    void setShowExifComments(bool show);

    int thumbnailRows() const;
    void setThumbnailRows(int rows);

    int thumbnailColumns() const;
    void setThumbnailColumns(int columns);

    int frameWidth() const;
    void setFrameWidth(int width);

    int stagePadding() const;
    void setStagePadding(int stagePadding);

    QColor textColor() const;
    void setTextColor(const QColor &color);

    QColor frameColor() const;
    void setFrameColor(const QColor &color);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    QString navDirection() const;
    void setNavDirection(const QString &direction);

    QString navPosition() const;
    void setNavPosition(const QString &pos);

    QString exportURL() const;

private:

    void selectionPage();
    void generalPage();
    void lookPage();

private slots:

    void slotHelp();

private:

    QFrame                              *m_selectionPage;
    QFrame                              *m_generalPage;
    QFrame                              *m_lookPage;

    QCheckBox                           *m_resizeExportImages;
    QCheckBox                           *m_showExifComments;
    QComboBox                           *m_navPosition;
    QComboBox                           *m_navDirection;

    KLineEdit                           *m_title;
    KIntNumInput                        *m_imagesExportSize;
    KIntNumInput                        *m_maxImageDimension;
    KIntNumInput                        *m_thumbnailRows;
    KIntNumInput                        *m_thumbnailColumns;
    KColorButton                        *m_textColor;
    KColorButton                        *m_backgroundColor;
    KColorButton                        *m_frameColor;
    KIntNumInput                        *m_frameWidth;
    KIntNumInput                        *m_stagePadding;
    KURLRequester                       *m_exportURL;

    KIPI::Interface                     *m_interface;
    KIPI::ImageCollectionSelector       *m_imageCollectionSelector;
    KIPIPlugins::KPAboutData            *m_about;
    QValueList<KIPI::ImageCollection>    m_selectedAlbums;
};

} // namespace KIPISimpleViewerExportPlugin

#endif /* SVEDIALOG_H */
