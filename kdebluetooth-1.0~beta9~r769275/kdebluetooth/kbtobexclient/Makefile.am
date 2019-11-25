# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -Ikio $(all_includes) $(OPENOBEX_CFLAGS) $(BLUETOOTH_CFLAGS) $(DBUS_CFLAGS)

# these are the headers for your project


# let automoc handle all of the meta source files (moc)



#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kbtobexclient

# the application source, library search path, and link libraries

kbtobexclient_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kbtobexclient_LDADD = ../libkbluetooth/libkbluetooth.la ../libkobex/libkobex.la $(LIB_KIO) $(OPENOBEX_LIBS) -lkio $(LIB_KDEUI) $(BLUETOOTH_LIBS) $(DBUS_LIBS) -ldbus-qt-1

# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/Utilities
#shelldesktop_DATA = kbtobexclient.desktop
xdg_apps_DATA = kbtobexclient.desktop

# The konqueror sidebar entry
servicemenudir = $(kde_datadir)/konqueror/servicemenus
servicemenu_DATA = kbtobexclient_sendfile.desktop

# The XML GUI file
kbtobexclientdatadir = $(kde_datadir)/kbtobexclient
kbtobexclientdata_DATA = kbtobexclientui.rc

kbtobexclient_SOURCES = main.cpp sender.cpp fileiconview.cpp diroperator.cpp \
	fileview.cpp dndfileview.cpp mainwindow.cpp

noinst_HEADERS = sender.h fileiconview.h diroperator.h fileview.h dndfileview.h \
	mainwindow.h

METASOURCES = AUTO

