METASOURCES = AUTO

# Code 
bin_PROGRAMS = kbluemon

kbluemon_SOURCES = main.cpp monitor.cpp mondialog.ui
kbluemon_LDFLAGS = $(KDE_RPATH) $(DBUS_LIBS) -ldbus-qt-1 $(all_libraries)
kbluemon_LDADD = ../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/System
#shelldesktop_DATA = kbluemon.desktop
xdg_apps_DATA = kbluemon.desktop


#EXTRA_DIST = $(kbluemon_SOURCES)\
#             $(kde_servicetypes_DATA) \
#	     eventsrc 

#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kbluemon
#	$(INSTALL_DATA) $(srcdir)/eventsrc $(DESTDIR)$(kde_datadir)/kbluemon/eventsrc

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -I$(top_srcdir) -I$(top_srcdir)/libkbluemon/ $(all_includes) $(DBUS_CFLAGS)

noinst_HEADERS = monitor.h remotedevice.h
EXTRA_DIST = monitor.h remotedevice.h

