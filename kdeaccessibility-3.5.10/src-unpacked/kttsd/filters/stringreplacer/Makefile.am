INCLUDES = \
	-I$(top_srcdir)/kttsd/libkttsd -I$(top_builddir)/kttsd/libkttsd \
	$(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkttsd_stringreplacerplugin.la

libkttsd_stringreplacerplugin_la_SOURCES = \
	stringreplacerconfwidget.ui \
	editreplacementwidget.ui \
	stringreplacerconf.cpp \
	stringreplacerproc.cpp \
	stringreplacerplugin.cpp
libkttsd_stringreplacerplugin_la_LDFLAGS = $(KDE_PLUGIN) $(all_libraries)
libkttsd_stringreplacerplugin_la_LIBADD = $(top_builddir)/kttsd/libkttsd/libkttsd.la

services_DATA = kttsd_stringreplacerplugin.desktop
servicesdir = $(kde_servicesdir)

# Install data files.
stringreplacerdatadir = $(kde_datadir)/kttsd/stringreplacer/
stringreplacerdata_DATA = \
	abbreviations.xml \
	chat.xml \
	chat-de.xml \
	emoticons.xml \
	polish_festival_fixes.xml \
	polish_festival_unspeakables.xml \
	kmail.xml \
	festival_unspeakable_chars.xml \
	qt2plaintext.xml

noinst_HEADERS =  stringreplacerconfwidget.h editreplacementwidget.h
