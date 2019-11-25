METASOURCES = AUTO

# Code 
bin_PROGRAMS = kinputwizard

kinputwizard_SOURCES = main.cpp inputwizard.cpp inputdialog.ui inputdialog_ext.ui
kinputwizard_LDFLAGS = $(KDE_RPATH) $(DBUS_LIBS) -ldbus-qt-1 $(all_libraries)
kinputwizard_LDADD = ../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI)


# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/System
#shelldesktop_DATA = kinputwizard.desktop
xdg_apps_DATA = kinputwizard.desktop


#EXTRA_DIST = $(kinputwizard_SOURCES)\
#             $(kde_servicetypes_DATA) \
#	     eventsrc 

#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kinputwizard
#	$(INSTALL_DATA) $(srcdir)/eventsrc $(DESTDIR)$(kde_datadir)/kinputwizard/eventsrc

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -I$(top_srcdir) -I$(top_srcdir)/libkinputwizard/ $(all_includes) $(DBUS_CFLAGS)

noinst_HEADERS = inputwizard.h
EXTRA_DIST = inputwizard.h

