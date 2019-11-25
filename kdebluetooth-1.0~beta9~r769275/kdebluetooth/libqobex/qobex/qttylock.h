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

#ifndef QTTYLOCK_H
#define QTTYLOCK_H
#include <qstring.h>

#include <sys/types.h>

class QTTYLock {
 public:

  QTTYLock();
  QTTYLock( const QString& );
  ~QTTYLock();

  void setDevice( const QString& );
  QString getDevice() const;

  bool lock();
  void unlock();
  bool testLock();

 private:
  QString device;
  // Keep track of _OWN_ locks, avoid unlocking an already
  // unlocked device in the destructor.
  bool isLocked;

#if !(defined(HAVE_BAUDBOY_H) || defined(HAVE_LOCKDEV_H))
  QString numericFile();
  QString nameFile();
  QString pidFile(pid_t);
  void removeStaleLocks();
  bool stillAlive(pid_t);
  const char* getLockDir();

  static const char* lockDirs[];
#endif
};

#endif
