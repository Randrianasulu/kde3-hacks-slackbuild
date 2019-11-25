INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kdebluetooth -I$(top_srcdir)/kdebluetooth/libkbluetooth/ $(all_includes) $(DBUS_CFLAGS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kcm_btpaired.la

kcm_btpaired_la_SOURCES = pairedtabbase.ui pairedtab.cpp kcm_btpaired.cpp \
		exportdialog.ui
kcm_btpaired_la_LIBADD = $(LIB_KIO) ../libkbluetooth/libkbluetooth.la $(LIB_KIO) $(LIB_KDEUI) $(SDP_LIBS) $(DBUS_LIBS) -ldbus-qt-1
kcm_btpaired_la_LDFLAGS = -avoid-version -module -no-undefined $(all_libraries)

#kbluetoothdcm_DATA = kcm_btpaired.desktop
#kbluetoothdcmdir = $(kde_appsdir)/Settings/Network/Bluetooth
xdg_apps_DATA = kcm_btpaired.desktop

noinst_HEADERS = 

