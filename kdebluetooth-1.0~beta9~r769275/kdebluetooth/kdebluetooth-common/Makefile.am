SUBDIRS = icons mimetypes sdpmimemappings dunhandler faxhandler

# Services

# Desktop file defining a servicetype for sdp service handlers
kde_servicetypes_DATA = sdpservicehandler.desktop

EXTRA_DIST = $(kde_servicetypes_DATA)

# Bluetooth directory entry in the control center
xdg_directory_DATA = kde-settings-network-bluetooth.directory

# from kdebase/applnk/Makefile.am
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_appsdir)/Settings/Network/Bluetooth
	$(INSTALL_DATA) $(srcdir)/kde-settings-network-bluetooth.directory \
	  $(DESTDIR)$(kde_appsdir)/Settings/Network/Bluetooth/.directory

uninstall-local:
	-rm -f $(DESTDIR)$(kde_appsdir)/Settings/Network/Bluetooth/.directory

# Update scripts
#update_DATA = kdebluetooth.upd kdebluetooth.sh
#updatedir = $(kde_datadir)/kconf_update
