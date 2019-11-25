
INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kdebluetooth $(all_includes) $(DBUS_CFLAGS)

kde_module_LTLIBRARIES = kio_sdp.la

kio_sdp_la_METASOURCES = AUTO
kio_sdp_la_SOURCES = btsdp.cpp
kio_sdp_la_LIBADD = ../../libkbluetooth/libkbluetooth.la $(DBUS_LIBS) -lkio
kio_sdp_la_LDFLAGS = -avoid-version -module $(all_libraries) $(KDE_PLUGIN)

protocol_DATA = btsdp.protocol
protocoldir = $(kde_servicesdir)

