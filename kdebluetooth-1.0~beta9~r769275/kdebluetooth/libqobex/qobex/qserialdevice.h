/*
    This file is part of libqobex.

    Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef QSERIALDEVICE_H
#define QSERIALDEVICE_H

#include <termios.h>

#include <qiodevice.h>

#include "qbytebuffer.h"
#include "qttylock.h"

class QSerialDevice
  : public QIODevice {
public:
  enum {
    // Parity values.
    NoParity             = 'N',
    EvenParity           = 'E',
    OddParity            = 'O',
    MarkParity           = 'M',
    SpaceParity          = 'S',

    // Default values
    DefaultParity        = NoParity,
    DefaultDataBits      = 8,
    DefaultStopBits      = 1,
    DefaultSpeed         = 57600,
    DefaultHWFlowControl = 0,
    DefaultSWFlowControl = 0
  };

  QSerialDevice();
  QSerialDevice( const QString&, bool = true );
  ~QSerialDevice();

  // the functions required for a QIODevice ...
  bool isOpen() const;

  bool open( int );
  void close();
  void flush();
  
  Q_ULONG size() const;
  
  Q_LONG readBlock( char *, Q_ULONG );
  Q_LONG writeBlock( const char *, Q_ULONG );
  
  int getch();
  int putch( int );
  int ungetch( int );

  // Implementation specific functions
  // Sets the serial device
  void setDevice( const QString& );
  QString device() const;

  // Return the filedescriptor in case somone wants to select on it.
  int getFd() const { return fd; }

  // Sets the speed
  void setSpeed( int = DefaultSpeed );
  int speed() const;

  // Set and get parity value.
  void setParity( int );
  int parity() const;

  // Drop and raise DTR line. For modem initialization ...
  void dropDTR();
  void raiseDTR();

  // Set the device blocking or nonblocking. Default is blocking
  bool blocking() const;
  void setBlocking( bool enable );

  // sets and gets if the device should do locking of the device file
  bool locking() const { return mDoLocking; }
  void setLocking( bool enable ) { mDoLocking = enable; }

  int waitForMore( int ) const;

  // Modem handling
  // Send the given string to the modem
  bool sendModemCommand( const char* );
  bool lineAvailable();
  QString readLine();

  // Discard any buffered input or output. do the same on the serial line.
  void clear();

private:
  // The filedescriptor of that device
  int fd;

  // FIXME ... don't do here
  // Class which does locking
  QTTYLock mLock;
  bool mDoLocking;

  // Interface parameters
  int mSpeed;
  int mDataBits;
  int mStopBits;
  int mParity;
  bool mHWFlowControl;
  bool mSWFlowControl;

  // Terminal options of the serial device on entry.
  // The old options are restored on exit.
  struct termios oldTermOpts;

  // We have to buffer reads because we want to use QIODevice::readLine
  QByteBuffer mReadBuffer;

  // Reads some remaining data into mReadBuffer
  Q_LONG unreadBytes() const;
  Q_LONG reallyRead( char*, Q_ULONG );

  // sets options according to state variables
  void setDataBits( struct termios& );
  void setStopBits( struct termios& );
  void setParity( struct termios& );
  void setHWFlowControl( struct termios& );
  void setSWFlowControl( struct termios& );
  void setSpeed( struct termios& );
};

#endif
