## Process this file with automake to produce Makefile.in

bin_PROGRAMS = exr_ctl_exr

exr_ctl_exr_CPPFLAGS = $(AM_CPPFLAGS) 

INCLUDES = \
	-I$(top_srcdir)/IlmImfCtl \
	@OPENEXR_CXXFLAGS@ \
	@CTL_CXXFLAGS@

LDADD =	\
	-L$(top_builddir)/IlmImfCtl -lIlmImfCtl \
	@CTL_LIBS@ @OPENEXR_LIBS@ \
	@OPENEXR_LDFLAGS@ \
	@CTL_LDFLAGS@

exr_ctl_exr_SOURCES = \
	applyCtl.cpp \
	applyCtl.h \
	exrCtlExr.cpp \
	exrCtlExr.h \
	main.cpp

noinst_HEADERS =  \
	applyCtl.h \
	exrCtlExr.h

EXTRA_DIST = \
	change_saturation.ctl


# this produces a data directory for exr_ctl_exr to find default 
# ctl files you will need to set the environment variable 
# CTL_MODULE_PATH to point to this directory 
# (by default /usr/local/lib/CTL) for exr_ctl_exr to work properly
# note exrdisplay also uses these .ctl files
ctldatadir = $(libdir)/CTL
ctldata_DATA = \
	change_saturation.ctl

# since ctl files appear above, tell the makefile to do nothing
# with them (except install them)
%.ctl:
	@true
