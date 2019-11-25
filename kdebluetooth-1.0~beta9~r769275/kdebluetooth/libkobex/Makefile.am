INCLUDES = -I$(top_srcdir)/kdebluetooth $(all_includes) $(OPENOBEX_CFLAGS) $(BLUETOOTH_CFLAGS)
libkobex_la_METASOURCES = AUTO

noinst_LTLIBRARIES = libkobex.la
libkobex_la_SOURCES = kobex.cpp kobexbluetooth.cpp kobexclient.cpp kobexheader.cpp kobexobject.cpp kobexserver.cpp

noinst_HEADERS = kobex.h kobexbluetooth.h kobexclient.h kobexheader.h kobexobject.h kobexserver.h

libkobex_la_LIBADD = ../libkbluetooth/libkbluetooth.la $(OPENOBEX_LIBS)

