//////////////////////////////////////////////////////////////////////////////
//
//    OPTIONSDIALOG.H
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at gmail dot com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef OptionsDialog_H
#define OptionsDialog_H

// Qt includes.

#include <qpixmap.h>

// KDE includes.

#include <kdialogbase.h>

class KLineEdit;
class KIconLoader;

namespace KIPIMPEGEncoderPlugin
{

class OptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   OptionsDialog(QWidget *parent=0);
   ~OptionsDialog();

   KLineEdit* IMBinFolderEditFilename;
   KLineEdit* MJBinFolderEditFilename;

 private slots:
   void slotIMBinFolderFilenameDialog(void);
   void slotMJBinFolderFilenameDialog(void);

 private:
   KIconLoader* Icons;
   QPixmap LoadIcon( QString Name, int Group );
};

}  // NameSpace KIPIMPEGEncoderPlugin

#endif  // OptionsDialog.H
