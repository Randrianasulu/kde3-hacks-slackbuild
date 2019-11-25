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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtextstream.h>
#include <qstring.h>
#include <qfile.h>

#ifdef HAVE_BAUDBOY_H
// Header shipped with newer RedHat distributions.
// We have to use this here, because /var/lock is owned by root:lock
// and no one is in this group. Only the binary /usr/sbin/lockdev is
// owned by this group and has the setgid flag set. This binary is called
// in ttylock etc ...
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <baudboy.h>
# include <cstdlib>
#else
# ifdef HAVE_LOCKDEV_H
// Library shipped with newer RedHat and Debian(?) distributions.
// Use this if baudboy is not available.
#  include <lockdev.h>
#  include <sys/types.h>
#  include <unistd.h>
# else
// If lockdev.h is also unavailable do locking
// like described in the serial HOWTO.
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <qfile.h>
#  include <signal.h>
#  include <errno.h>
#  include <string.h>
#  include <grp.h>
#  include <pwd.h>
# endif
#endif

#include "qttylock.h"

QTTYLock::QTTYLock()
  : isLocked(false) {
}

QTTYLock::QTTYLock(const QString& dev)
  : device(dev), isLocked(false) {
}

QTTYLock::~QTTYLock() {
  if (isLocked)
    unlock();
}

void
QTTYLock::setDevice(const QString& dev) {
  if (isLocked) {
    unlock();
    isLocked = false;
  }
  device = dev;
}

QString
QTTYLock::getDevice() const{
  return device;
}

bool
QTTYLock::lock() {
#ifdef HAVE_BAUDBOY_H
  isLocked = ttylock( QFile::encodeName( device ) ) == EXIT_SUCCESS;
#else
# ifdef HAVE_LOCKDEV_H
  pid_t ret = dev_lock( QFile::encodeName( device ) );
  isLocked = ! ret;
# else
  if ( !isLocked ) {
    removeStaleLocks();

    QString pidF = pidFile( ::getpid() );
    QString nameF = nameFile();
    QString numericF = numericFile();
    QString tmpF = pidF + ".tmp";
    QFile file( tmpF );
    if (file.open(IO_WriteOnly)) {
      QTextStream stream(&file);
      stream << int( ::getpid() ) << endl;
      file.close();
      
      if (! ::link( QFile::encodeName( tmpF ),
		    QFile::encodeName( numericF ))) {
	// We have ::linked the file.
	if (! ::link( QFile::encodeName( tmpF ),
		      QFile::encodeName( nameF ))) {
	  // We have ::linked the file.
	  if (! ::link(QFile::encodeName( tmpF ),
		       QFile::encodeName( pidF ))) {
	    // The Lock is really ours. Remove the temporary file.
	    ::unlink( QFile::encodeName( tmpF ));
	    isLocked = true;
	  } else {
	    ::unlink( QFile::encodeName( nameF ));
	    ::unlink( QFile::encodeName( numericF ));
	    ::unlink( QFile::encodeName( tmpF ));
	    isLocked = false;
	  }
	} else {
	  ::unlink( QFile::encodeName( numericF ));
	  ::unlink( QFile::encodeName( tmpF ));
	  isLocked = false;
	}
      } else {
	::unlink( QFile::encodeName( tmpF ));
	isLocked = false;
      }
    } else {
      // Something wired has happened
      isLocked = false;
    }
  }
# endif
#endif
  return isLocked;
}

void
QTTYLock::unlock() {
#ifdef HAVE_BAUDBOY_H
  ttyunlock( QFile::encodeName( device ) );
#else
# ifdef HAVE_LOCKDEV_H
  dev_unlock( QFile::encodeName( device ), ::getpid() );
# else
  if ( QFile::exists( pidFile( ::getpid() ) ) ) {
    // if the File with the pid exists assume that we own the lock
    ::unlink( QFile::encodeName( nameFile() ) );
    ::unlink( QFile::encodeName( numericFile() ) );
    ::unlink( QFile::encodeName( pidFile( ::getpid() ) ) );
  }
# endif
#endif
  isLocked = false;
}

bool
QTTYLock::testLock() {
  if ( isLocked )
    return true;
  else {
#ifdef HAVE_BAUDBOY_H
    return ttylocked( QFile::encodeName( device ) ) != EXIT_SUCCESS;
#else
# ifdef HAVE_LOCKDEV_H
    pid_t ret = dev_testlock( QFile::encodeName( device ) );
    return ret;
# else
    removeStaleLocks();
    return QFile::exists( numericFile() ) || QFile::exists( nameFile() );
# endif
#endif
  }
}

#if !(defined(HAVE_BAUDBOY_H) || defined(HAVE_LOCKDEV_H))

QString
QTTYLock::numericFile() {
  QString f;
  struct stat sbuf;
  if ( !stat( QFile::encodeName( device ), &sbuf ) )
    f.sprintf( "/LCK.%03d.%03d", major(sbuf.st_rdev), minor(sbuf.st_rdev) );
  const char* prefix = getLockDir();
  if ( prefix ) {
    f.prepend( prefix );
    return f;
  } else
    return QString::null;
}

QString
QTTYLock::nameFile() {
  const char* prefix = getLockDir();
  if ( prefix ) {
    QString f = prefix;
    f += "/LCK..";
    f += device.section( '/', -1 );
    return f;
  } else
    return QString::null;
}

QString
QTTYLock::pidFile(pid_t p) {
  QString f;
  f.sprintf( "/LCK...%d", p );
  const char* prefix = getLockDir();
  if ( prefix ) {
    f.prepend( prefix );
    return f;
  } else
    return QString::null;
}

void
QTTYLock::removeStaleLocks() {
  QString nameF = nameFile();
  QString numericF = numericFile();

  if ( QFile::exists( nameF ) ) {
    QFile file( nameF );
    if ( file.open( IO_ReadOnly ) ) {
      QTextStream stream( &file );
      int pid;
      stream >> pid;
      file.close();
      if ( !stillAlive(pid) ) {
	// We have found a stale lock here
	::unlink( QFile::encodeName( nameF ) );
	::unlink( QFile::encodeName( pidFile(pid) ) );
      }
    }
  }
  if ( QFile::exists( numericF ) ) {
    QFile file( numericF );
    if ( file.open( IO_ReadOnly ) ) {
      QTextStream stream( &file );
      int pid;
      stream >> pid;
      file.close();
      if ( !stillAlive( pid ) ) {
	// We have found a stale lock here
	::unlink( QFile::encodeName( numericF ) );
	::unlink( QFile::encodeName( pidFile(pid) ) );
      }
    }
  }
}

bool
QTTYLock::stillAlive(pid_t p) {
  return !(p > 0 && ::kill(p, 0) < 0 && errno == ESRCH);
}

const char* QTTYLock::getLockDir() {
  // Get a writeble lock directory.
  for ( int i=0; lockDirs[i]; ++i ) {
    if ( access( lockDirs[i], R_OK|W_OK ) == 0 )
      return lockDirs[i];
  }

  return 0;
}

const char* QTTYLock::lockDirs[] = {
  "/var/lock",
  "/etc/locks",
  "/var/spool/locks",
  "/var/spool/uucp",
  0
};

#endif
