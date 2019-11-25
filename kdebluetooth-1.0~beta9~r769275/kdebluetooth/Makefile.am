#COMPILE_FIRST = libkbluetooth
#COMPILE_BEFORE_foo = bar

#SUBDIRS = libkbluetooth kbluetooth libkobex libqobex kdebluetooth-common $(HANDSFREE) kbtobexclient kbtobexsrv kbtserialchat khciconfig kioslave bemused kioclient kbtsearch $(IRMCSYNCKONNECTOR)

# disabled for dbus integration (other modules will not build against new libkbluetooth atm)
SUBDIRS = libkbluetooth kbluetooth kcm_btpaired kdebluetooth-common libqobex kioslave kbluemon kbluelock kinputwizard libkobex kbtobexsrv kbtobexclient

#AUTOMAKE_OPTIONS = foreign
messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg"` > rc.cpp
	$(XGETTEXT) `find . -name "*.cpp" -o -name "*.h"` -o $(podir)/kdebluetooth.pot
