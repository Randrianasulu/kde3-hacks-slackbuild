# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ -Ikio $(all_includes) $(OPENOBEX_CFLAGS) $(BLUETOOTH_CFLAGS)

# these are the headers for your project
noinst_HEADERS = myfileiconview.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

#KDE_ICON = kbluetestd

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
kbtobexsrvbin_PROGRAMS = kbtobexsrv
kbtobexsrvbindir = ${KBTOBEXSRV_BINDIR}

# the application source, library search path, and link libraries
kbtobexsrv_SOURCES = main.cpp maindlg.cpp maindlgbase.ui myfileiconview.cpp
kbtobexsrv_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kbtobexsrv_LDADD = ../libkbluetooth/libkbluetooth.la ../libkobex/libkobex.la $(LIB_KIO) $(OPENOBEX_LIBS) -lkio $(LIB_KDEUI) $(BLUETOOTH_LIBS)

# this is where the desktop file will go 
#shelldesktopdir = $(kde_appsdir)/Utilities
#shelldesktop_DATA = kbtobexsrv.desktop
xdg_apps_DATA = kbtobexsrv.desktop

# service files
shellservicedir = $(kde_servicesdir)
shellservice_DATA = kbluetooth_kbtobexsrv.desktop\
		    kbluetooth_kbtobexsrv.sdp.xml

# this is where the shell's XML-GUI resource file goes
#shellrcdir   = $(kde_datadir)/kbtobexsrv
#shellrc_DATA = kbluetestdui.rc
