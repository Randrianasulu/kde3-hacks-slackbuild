# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kalsamix.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kalsamix

# the application source, library search path, and link libraries

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = kalsamix.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kalsamix
shellrc_DATA = kalsamix_splash.png kalsamixui.rc

kalsamix_SOURCES = alsacb.cpp kalsa.cpp kalsamix.cpp kamconfig.cpp \
	kamdcopiface.cpp kamdcopiface.skel kamsystemtray.cpp kamuapp.cpp kcard.cpp kchannel.cpp \
	kconfigwin.cpp kgroupwidget.cpp kitemsgroup.cpp kledex.cpp kmasterview.cpp \
	kmixconst.cpp kmixer.cpp kmixeritem.cpp kmixerslider.cpp kmixerview.cpp \
	kmutewidget.cpp kosd.cpp kpreset.cpp ksliderex.cpp kvertlabel.cpp kvumeter.cpp main.cpp
kalsamix_LDADD = $(ARTSKDE) -lasound
noinst_HEADERS = kamconfig.h kamdcopiface.h kamsystemtray.h \
	kamsystemtray_interface.h kamuapp.h
