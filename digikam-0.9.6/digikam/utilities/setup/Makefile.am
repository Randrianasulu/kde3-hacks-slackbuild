METASOURCES = AUTO

INCLUDES = -I$(top_srcdir)/digikam/digikam \
	   -I$(top_srcdir)/digikam/utilities/cameragui \
	   -I$(top_srcdir)/digikam/utilities/batch \
	   -I$(top_srcdir)/digikam/utilities/imageeditor/canvas \
	   -I$(top_srcdir)/digikam/libs/dialogs \
	   -I$(top_srcdir)/digikam/libs/dimg \
	   -I$(top_srcdir)/digikam/libs/dimg/loaders \
	   -I$(top_srcdir)/digikam/libs/widgets/common \
	   $(LIBKIPI_CFLAGS) $(LIBKDCRAW_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libsetup.la libshowfotosetup.la

libsetup_la_SOURCES = cameraselection.cpp setupcamera.cpp \
	              setupmime.cpp setupplugins.cpp setupidentity.cpp \
	              setupgeneral.cpp setup.cpp \
	              setupcollections.cpp setupmetadata.cpp \
	              setupeditor.cpp setupmisc.cpp setupicc.cpp \
	              setupiofiles.cpp setupdcraw.cpp setupslideshow.cpp \
	              setuptooltip.cpp setuplighttable.cpp 

libsetup_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_GPHOTO)

libshowfotosetup_la_SOURCES = setupiofiles.cpp setupdcraw.cpp \
	                      setupicc.cpp setupslideshow.cpp 

libshowfotosetup_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) 

