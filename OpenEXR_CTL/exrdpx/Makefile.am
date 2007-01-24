## Process this file with automake to produce Makefile.in

bin_PROGRAMS = exrdpx

exrdpx_CPPFLAGS = $(AM_CPPFLAGS) 

INCLUDES = \
	@OPENEXR_CXXFLAGS@ \
	@CTL_CXXFLAGS@ \
	-I$(top_srcdir)/IlmImfCtl

LDADD =	\
	@CTL_LIBS@ @OPENEXR_LIBS@ \
	@OPENEXR_LDFLAGS@ \
	@CTL_LDFLAGS@ \
	-L$(top_builddir)/IlmImfCtl -lIlmImfCtl

exrdpx_SOURCES = \
	applyCtl.cpp \
	applyCtl.h \
	dpxHeader.cpp \
	dpxHeader.h \
	dpxToExr.cpp \
	dpxToExr.h \
	exrToDpx.cpp \
	exrToDpx.h \
	main.cpp

noinst_HEADERS =  \
	applyCtl.h \
	dpxHeader.h \
	dpxToExr.h \
	exrToDpx.h

EXTRA_DIST = \
	transform_DPX_EXR.ctl \
	transform_EXR_DPX.ctl


# this produces a data directory for exrdpx to find default 
# ctl files you will need to set the environment variable 
# CTL_MODULE_PATH to point to this directory 
# (by default /usr/local/lib/CTL) for exrdpx to work properly
# note exrdisplay also uses these .ctl files
ctldatadir = $(libdir)/CTL
ctldata_DATA = \
	transform_DPX_EXR.ctl \
	transform_EXR_DPX.ctl

# since ctl files appear above, tell the makefile to do nothing
# with them (except install them)
%.ctl:
	@true
