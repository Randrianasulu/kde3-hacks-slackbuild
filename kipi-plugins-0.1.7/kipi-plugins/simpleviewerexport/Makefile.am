INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(LIBKDCRAW_CFLAGS) $(LIBKEXIV2_CFLAGS) $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = kipiplugin_simpleviewer.la

kipiplugin_simpleviewer_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP) $(LIBKEXIV2_LIBS_DEP) $(LIBKDCRAW_LIBS_DEP)

kipiplugin_simpleviewer_la_SOURCES = plugin_simpleviewer.cpp svedialog.cpp \
	                                 simpleviewerexport.cpp firstrundlg.cpp

kipiplugin_simpleviewer_la_LIBADD = $(LIBKIPI_LIBS) $(LIBKDCRAW_LIBS) $(LIBKEXIV2_LIBS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

kipiplugin_simpleviewer_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

kde_services_DATA = kipiplugin_simpleviewer.desktop

kipiplugin_simpleviewer_svdir = $(kde_datadir)/kipiplugin_simpleviewerexport/simpleviewer_html
kipiplugin_simpleviewer_sv_DATA = space.png

kipiplugin_simpleviewer_svtemplatedir = $(kde_datadir)/kipiplugin_simpleviewerexport
kipiplugin_simpleviewer_svtemplate_DATA = index.template

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_simpleviewer.pot
