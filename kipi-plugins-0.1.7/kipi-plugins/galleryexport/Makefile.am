INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKEXIV2_CFLAGS) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_galleryexport.la

kipiplugin_galleryexport_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP)  

kipiplugin_galleryexport_la_SOURCES = plugin_galleryexport.cpp \
		galleries.cpp gallerylist.cpp galleryconfig.cpp \
		gallerywindow.cpp gallerytalker.cpp \
		gallerywidget.cpp gallerympform.cpp galleryalbumdialog.ui \
		galleryviewitem.cpp

kipiplugin_galleryexport_la_LIBADD = $(LIBKEXIV2_LIBS) \
	-lkwalletclient $(LIB_KHTML) $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_galleryexport_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_galleryexport.desktop

kipiplugin_galleryexportpicsdir = $(kde_datadir)/kipiplugin_galleryexport/pics
kipiplugin_galleryexportpics_DATA = gallery.png

#kipiplugin_galleryexporticondir = $(kde_datadir)/kipiplugin_galleryexport/icons
#kipiplugin_galleryexporticon_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_galleryexport.pot

