#
#   This file is part of kio_obex.
#
#   Copyright (c) 2003-2004 Mathias Froehlich <Mathias.Froehlich@web.de>
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

INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -I$(top_srcdir)/kdebluetooth/libqobex $(all_includes)

kde_module_LTLIBRARIES = kio_obex.la
kio_obex_la_SOURCES = obex.cpp
kio_obex_la_LIBADD = ../../../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) ../../../libqobex/qobex/libqobex.la $(LIB_QT)
kio_obex_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_PLUGIN) $(all_libraries)

EXTRA_DIST = obex.protocol

obexservicesdir = $(kde_servicesdir)
obexservices_DATA = obex.protocol

METASOURCES = AUTO

# kioobex_start is a mimetype handler for 
# bluetooth/obex-ftp-profile, so that users can
# navigate from kio_sdp to kio_obex
bin_SCRIPTS = kioobex_start
shelldesktopdir = $(kde_appsdir)/.hidden
shelldesktop_DATA = kioobex_start.desktop
