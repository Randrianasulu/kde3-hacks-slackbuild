/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef RECOMPRESSOPTIONSDIALOG_H
#define RECOMPRESSOPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QLabel;
class QCheckBox;
class QComboBox;

class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class RecompressOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   RecompressOptionsDialog(QWidget *parent=0);
   ~RecompressOptionsDialog();

   QLabel          *m_label_JPEGimageCompression;
   QLabel          *m_label_PNGimageCompression;
   QLabel          *m_label_TIFFimageCompression;
   QLabel          *m_label_TGAimageCompression;
   KIntNumInput    *m_JPEGCompression;
   KIntNumInput    *m_PNGCompression;
   QCheckBox       *m_compressLossLess;
   QComboBox       *m_TIFFCompressionAlgo;
   QComboBox       *m_TGACompressionAlgo;

 public slots:
   void slotCompressLossLessEnabled(bool val);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // RECOMPRESSOPTIONSDIALOG_H
