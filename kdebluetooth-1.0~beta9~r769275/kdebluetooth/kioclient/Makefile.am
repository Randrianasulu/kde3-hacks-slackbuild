INCLUDES = $(all_includes)
noinst_HEADERS =  commandhandler.h
METASOURCES = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kioclient

# the application source, library search path, and link libraries
kioclient_SOURCES = main.cpp commandhandler.cpp
kioclient_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kioclient_LDADD = $(top_builddir)/libkbluetooth/libkbluetooth.la -lkio $(LIB_KDECORE)

