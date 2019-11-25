#
#   This file is part of libqobex.
#
#   Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Library General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public License
#   along with this library; see the file COPYING.LIB.  If not, write to
#   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
#   Boston, MA 02110-1301, USA.
#

AM_CPPFLAGS = $(QT_INCLUDES)

lib_LTLIBRARIES = libqobex.la

QOBEX_VERSION = 0:9:0

INCLUDES = -I$(top_srcdir)/kdebluetooth -Ikio $(all_includes) $(OPENOBEX_CFLAGS) $(BLUETOOTH_CFLAGS)
qobexincludedir = $(includedir)/qobex
qobexinclude_HEADERS = \
  qobexobject.h qobexheader.h \
  qobexlengthvaluebase.h qobexauth.h qobexapparam.h \
  qobexbase.h qobexclient.h qobextransport.h qobexuuid.h qobexarray.h \
  qobexserver.h qobexserverconnection.h \
  qobexserverops.h \
  qobexbfbtransport.h qbfbframe.h qbfbdata.h \
  qttylock.h qserialdevice.h qbytebuffer.h \
  qobexericssontransport.h \
  qobexserialtransport.h \
  qobexintransport.h \
  qobexirdatransport.h \
  qobexbttransport.h

libqobex_la_SOURCES = \
  qobexobject.cpp qobexheader.cpp \
  qobexlengthvaluebase.cpp qobexauth.cpp qobexapparam.cpp \
  qobexbase.cpp qobexclient.cpp qobextransport.cpp qobexuuid.cpp \
  qobexserver.cpp qobexserverconnection.cpp \
  qobexserverops.cpp qobexnullserverops.cpp \
  qobexbfbtransport.cpp qbfbframe.cpp qbfbdata.cpp \
  qttylock.cpp qserialdevice.cpp \
  qobexericssontransport.cpp \
  qobexserialtransport.cpp \
  qobexintransport.cpp \
  qobexirdatransport.cpp \
  qobexbttransport.cpp
  
libqobex_la_LIBADD = $(BTTRANSPORT_OBJ) $(IRDATRANSPORT_OBJ) $(KDE_RPATH) $(OPENOBEX_LIBS) -lkio $(LIB_KDEUI)

libqobex_la_LDFLAGS = -version-info $(QOBEX_VERSION) $(QT_LDFLAGS) $(LIB_QT) \
  $(BLUETOOTH_LIBS) $(LIB_LOCKDEV) $(all_libraries) $(all_libraries)

METASOURCES = AUTO

