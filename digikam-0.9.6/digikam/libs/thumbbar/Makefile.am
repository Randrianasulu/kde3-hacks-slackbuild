METASOURCES = AUTO

noinst_LTLIBRARIES = libthumbbar.la

libthumbbar_la_SOURCES = thumbbar.cpp thumbnailjob.cpp

libthumbbar_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)

INCLUDES = -I$(top_srcdir)/digikam/libs/dimg \
	       -I$(top_srcdir)/digikam/libs/dmetadata \
	       -I$(top_srcdir)/digikam/digikam \
		$(LIBKDCRAW_CFLAGS) \
		$(LIBKEXIV2_CFLAGS) \
		$(all_includes)

digikaminclude_HEADERS = thumbbar.h 

digikamincludedir = $(includedir)/digikam
