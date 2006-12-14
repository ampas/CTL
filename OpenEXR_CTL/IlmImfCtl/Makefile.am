## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libIlmImfCtl.la

INCLUDES = @CTL_CXXFLAGS@ @OPENEXR_CXXFLAGS@

AM_LDFLAGS = @CTL_LDFLAGS@ @OPENEXR_LDFLAGS@


libIlmImfCtl_la_SOURCES = \
	ImfCtlApplyTransforms.cpp \
	ImfCtlApplyTransforms.h \
	ImfCtlCopyFunctionArg.cpp \
	ImfCtlCopyFunctionArg.h


libIlmImfCtlincludedir = $(includedir)/OpenEXR

libIlmImfCtlinclude_HEADERS = \
	ImfCtlApplyTransforms.h

noinst_HEADERS = \
	ImfCtlCopyFunctionArg.h

libIlmImfCtl_la_LDFLAGS = -version-info @LIBTOOL_VERSION@ -no-undefined

