## Process this file with automake to produce Makefile.in

check_PROGRAMS = IlmImfCtlTest


IlmImfCtlTest_SOURCES = \
	main.cpp \
	testSourceDestination.cpp \
	testSourceDestination.h \
	testTypes.cpp \
	testTypes.h

IlmImfCTLTest_CPPFLAGS =

INCLUDES = \
	@CTL_CXXFLAGS@ \
	@OPENEXR_CXXFLAGS@ \
	-I$(top_srcdir)/IlmImfCtl

AM_LDFLAGS = \
	@CTL_LDFLAGS@ \
	@OPENEXR_LDFLAGS@

LDADD =	\
	@CTL_LIBS@ @OPENEXR_LIBS@ \
	-L$(top_builddir)/IlmImfCtl \
	-lIlmImfCtl

TESTS = IlmImfCtlTest
TESTS_ENVIRONMENT = export CTL_MODULE_PATH=$(top_srcdir)/IlmImfCtlTest:.;

EXTRA_DIST = \
	function1.ctl \
	function2.ctl \
	function3.ctl