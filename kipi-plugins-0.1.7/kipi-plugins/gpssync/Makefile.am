INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKEXIV2_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_gpssync.la

kipiplugin_gpssync_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP)  

# Srcs for the plugin
kipiplugin_gpssync_la_SOURCES = plugin_gpssync.cpp gpssyncdialog.cpp gpslistviewitem.cpp \
	                        gpsbabelbinary.cpp gpsdataparser.cpp gpseditdialog.cpp \
	                        gpsmapwidget.cpp kmlexport.cpp kmlexportconfig.cpp \
				kmlgpsdataparser.cpp 

# Libs needed by the plugin
kipiplugin_gpssync_la_LIBADD = $(LIBKEXIV2_LIBS) -lkipiplugins $(LIBKIPI_LIBS) $(LIB_KHTML) \
	                       $(LIB_KPARTS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) \
	                       $(LIB_KFILE)

# LD flags for the plugin
kipiplugin_gpssync_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries)

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_gpssync.desktop

# Icons set for the plugin.
kipiplugin_gpssyncicondir = $(kde_datadir)/kipiplugin_gpssync/icons
kipiplugin_gpssyncicon_ICON = AUTO


# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_gpssync.pot

