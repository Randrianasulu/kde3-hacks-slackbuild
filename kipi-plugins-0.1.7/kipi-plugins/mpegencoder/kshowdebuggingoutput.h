//////////////////////////////////////////////////////////////////////////////
//
//    KSHOWDEBUGGINGOUTPUT.H
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


#ifndef KShowDebuggingOutput_included
#define KShowDebuggingOutput_included

// Qt includes

#include <qpushbutton.h>
#include <qtextview.h>
#include <qlayout.h>

// KDElib includes

#include <kdialog.h>
#include <klocale.h>

namespace KIPIMPEGEncoderPlugin
{

class KShowDebuggingOutput : public KDialog
{
Q_OBJECT

public:
  KShowDebuggingOutput( QString Messages, QString Header, QString Foot, QWidget* parent );
  ~KShowDebuggingOutput();

public slots:
  void slotCopyToCliboard( void );

private:
  QPushButton*  okButton;
  QPushButton*  ClipBoardCopy;
  QTextView*    debugView;
  QGridLayout*  grid;
};

}  // NameSpace KIPIMPEGEncoderPlugin

#endif  // KShowDebuggingOutput_included

