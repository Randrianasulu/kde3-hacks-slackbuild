# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kbluetoothd -I../libkbluetooth $(all_includes) $(OPENOBEX_CFLAGS) $(BLUETOOTH_CFLAGS) 

# these are the headers for your project
noinst_HEADERS = kbtserialchat.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

#KDE_ICON = kbluetestd

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kbtserialchat

# the application source, library search path, and link libraries
kbtserialchat_SOURCES = main.cpp kbtserialchat.cpp maindialogbase.ui
kbtserialchat_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kbtserialchat_LDADD = $(top_builddir)/kbluetoothd/libkbluetoothd/libkbluetoothd.la ../libkbluetooth/libkbluetooth.la -lkio $(LIB_KDEUI)

# this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/Utilities
#shelldesktop_DATA = kbtserialchat.desktop
xdg_apps_DATA = kbtserialchat.desktop


# service files
shellservicedir = $(kde_servicesdir)
shellservice_DATA = kbluetoothd_kbtserialchat.desktop\
        kbluetoothd_kbtserialchat.sdp.xml

# mime type for the profile
#shellmimedir   = $(kde_mimedir)/bluetooth
#shellmime_DATA = serial-port-profile.desktop
