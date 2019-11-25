METASOURCES = AUTO

# Code 
bin_PROGRAMS = kbluetooth

kbluetooth_SOURCES = adapterconfig.cpp adapterdialog.ui authorize.cpp authdialog.ui deviceconfig.cpp trayicon.cpp main.cpp application.cpp pindefdialog.ui pindialog.cpp confirmationdlgbase.ui confirmation.cpp rfcommportlistener.cpp procinheritsock.cpp portlistener.cpp sdprecord.cpp
kbluetooth_LDFLAGS = $(KDE_RPATH) $(DBUS_LIBS) -ldbus-qt-1 $(all_libraries)
kbluetooth_LDADD = ../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/System
#shelldesktop_DATA = kbluetooth.desktop
xdg_apps_DATA = kbluetooth.desktop

# Autostart entry
autostartdir = $(datadir)/autostart
autostart_DATA = kbluetooth.autostart.desktop


#EXTRA_DIST = $(kbluetooth_SOURCES)\
#             $(kde_servicetypes_DATA) \
#	     eventsrc 

#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kbluetooth
#	$(INSTALL_DATA) $(srcdir)/eventsrc $(DESTDIR)$(kde_datadir)/kbluetooth/eventsrc

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir) -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ $(all_includes) $(DBUS_CFLAGS)

noinst_HEADERS = trayicon.h application.h pindialog.h authorize.h procinheritsock.h rfcommportlistener.h confirmation.h portlistener.h sdprecord.h

EXTRA_DIST = trayicon.h application.h pindialog.h authorize.h eventsrc
install-data-local: 
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kdebluetooth
	$(INSTALL_DATA) $(srcdir)/eventsrc $(DESTDIR)$(kde_datadir)/kdebluetooth/eventsrc
			

