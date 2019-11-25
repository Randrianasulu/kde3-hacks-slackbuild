INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kdebluetooth $(all_includes) $(DBUS_CFLAGS)

kde_module_LTLIBRARIES = kio_bluetooth.la

kio_bluetooth_la_METASOURCES = AUTO
kio_bluetooth_la_SOURCES = kiobluetooth.cpp
kio_bluetooth_la_LIBADD = ../../libkbluetooth/libkbluetooth.la -lkio
kio_bluetooth_la_LDFLAGS = -avoid-version -module $(all_libraries) $(KDE_PLUGIN)

protocol_DATA = bluetooth.protocol
protocoldir = $(kde_servicesdir)

# konqueror sidebar entry
sidebardir = $(kde_datadir)/konqsidebartng/virtual_folders/services/
sidebar_DATA = bluetooth_sidebarentry.desktop
