METASOURCES = AUTO

# Code 
bin_PROGRAMS = kbluelock

kbluelock_SOURCES = main.cpp kbluelock.cpp lockdialog.ui
kbluelock_LDFLAGS = $(KDE_RPATH) $(DBUS_LIBS) -ldbus-qt-1 $(all_libraries)
kbluelock_LDADD = ../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/System
#shelldesktop_DATA = kbluelock.desktop
xdg_apps_DATA = kbluelock.desktop


#EXTRA_DIST = $(kbluelock_SOURCES)\
#             $(kde_servicetypes_DATA) \
#	     eventsrc 

#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kbluelock
#	$(INSTALL_DATA) $(srcdir)/eventsrc $(DESTDIR)$(kde_datadir)/kbluelock/eventsrc

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -I$(top_srcdir) $(all_includes) $(DBUS_CFLAGS)

noinst_HEADERS = kbluelock.h
EXTRA_DIST = kbluelock.h

