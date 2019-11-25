lib_LTLIBRARIES = libkbluetooth.la
INCLUDES = $(all_includes) $(DBUS_CFLAGS) $(BLUETOOTH_CFLAGS)

libkbluetooth_la_SOURCES = adapter.cpp authagent.cpp dbusinit.cpp dbussignal.cpp dbusfilter.cpp \
			   manager.cpp service.cpp passkeyagent.cpp \
			   servicerecord.cpp devicemimeconverter.cpp inputdevice.cpp inputmanager.cpp deviceaddress.cpp \
			   rfcommsocket.cpp serviceselectionwidgetbase.ui servicediscovery.cpp sdpservice.cpp \
			   sdpdevice.cpp sdpattribute.cpp serviceselectionwidget.cpp inquiry.cpp hcisocket.cpp hcidefault.cpp \
			   namerequest.cpp adapter_old.cpp obexmanager.cpp obexsession.cpp

libkbluetooth_la_LDFLAGS = $(all_libraries)
libkbluetooth_la_LIBADD = $(LIB_KDECORE) $(DBUS_LIBS) $(LIB_KIO) $(BLUETOOTH_LIBS) -ldbus-qt-1

libkbluetoothincludedir = $(includedir)/libkbluetooth
libkbluetoothinclude_HEADERS = adapter.h dbusinit.h dbussignal.h dbussignal.h manager.h inputdevice.h inputmanager.h \
			       security.h dbusfilter.h service.h passkeyagent.h servicerecord.h devicemimeconverter.h \
                               deviceaddress.h rfcommsocket.h servicediscovery.h sdpservice.h namerequest.h adapter_old.h \
                               sdpdevice.h sdpattribute.h serviceselectionwidget.h inquiry.h hcisocket.h hcidefault.h \
 	 	 	 	 	 	 	    obexmanager.cpp obexsession.cpp
METASOURCES = AUTO

