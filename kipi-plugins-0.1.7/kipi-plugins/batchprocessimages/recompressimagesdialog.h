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

#ifndef RECOMPRESSIMAGESDIALOG_H
#define RECOMPRESSIMAGESDIALOG_H

// Local includes

#include "kpaboutdata.h"
#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class RecompressImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
 
   RecompressImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0 );
   ~RecompressImagesDialog();

 private slots:
 
   void slotOptionsClicked(void);
   void slotHelp(void);

 protected:
 
   int     m_JPEGCompression;
   int     m_PNGCompression;
   bool    m_compressLossLess;
   QString m_TIFFCompressionAlgo;
   QString m_TGACompressionAlgo;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       const QString& albumDest, bool previewMode);

   void readSettings(void);
   void saveSettings(void);

   bool prepareStartProcess(BatchProcessImagesItem *item,
                            const QString& albumDest);

 private:

   KIPIPlugins::KPAboutData    *m_about;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // RECOMPRESSIMAGESDIALOG_H
