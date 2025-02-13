/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef PICASAWEBWIDGET_H
#define PICASAWEBWIDGET_H

// Qt includes.

#include <qwidget.h>

// Local includes.

#include "uploadwidget.h"

class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QRadioButton;

class KHTMLPart;

namespace KIPIPicasawebExportPlugin
{

class UploadWidget;

class PicasawebWidget : public UploadWidget
{
    Q_OBJECT

public:

    PicasawebWidget(QWidget* parent=0, const char* name=0, WFlags fl= 0);
    ~PicasawebWidget();

private slots:

    void slotResizeChecked();
    void slotSelectionChecked();

private:

    QListView*    m_tagView;

    QButtonGroup* m_fileSrcButtonGroup;

//  QCheckBox*    m_resizeCheckBox;
//  QCheckBox*    m_exportApplicationTags;

    KHTMLPart*    m_photoView;

    friend class PicasawebWindow;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBWIDGET_H
