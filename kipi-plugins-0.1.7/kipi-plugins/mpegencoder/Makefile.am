INCLUDES = $(KIPI_PLUGINS_COMMON_INCLUDE) $(LIBKIPI_CFLAGS) $(all_includes)

METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kipiplugin_mpegencoder.la
kipiplugin_mpegencoder_la_DEPENDENCIES = $(LIBKIPI_LIBS_DEP)
# Srcs for the plugin
kipiplugin_mpegencoder_la_SOURCES = plugin_mpegencoder.cpp kimg2mpgbase.ui\
                                       kimg2mpg.cpp kshowdebuggingoutput.cpp \
                                       optionsdialog.cpp checkbinprog.cpp

# Libs needed by the plugin
kipiplugin_mpegencoder_la_LIBADD = $(LIBKIPI_LIBS) $(LIB_KIO) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) 

# LD flags for the plugin
kipiplugin_mpegencoder_la_LDFLAGS = $(KIPI_PLUGINS_COMMON_LDFLAGS) -module $(KDE_PLUGIN) $(all_libraries) -lkipiplugins

# Install the desktop file needed to detect the plugin
kde_services_DATA = kipiplugin_mpegencoder.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kipiplugin_mpegencoder.pot

####### This script name should probably be more 'namespaced'. Think about distros putting everything in /usr/bin...
bin_SCRIPTS = images2mpg
man_MANS = images2mpg.1

