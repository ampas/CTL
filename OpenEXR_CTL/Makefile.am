## Process this file with automake to produce Makefile.in

# tell autoconf to include the m4 macros in the /m4 directory 
# (an alternative to the acinclude.m4 mechanism)
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = IlmImfCtl IlmImfCtlTest CtlModules exrdpx exr_ctl_exr

DIST_SUBDIRS = \
	$(SUBDIRS) \
	vc

EXTRA_DIST = \
	AUTHORS COPYING ChangeLog INSTALL LICENSE NEWS \
        README README.CVS README.OSX README.win32 bootstrap

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = OpenEXR_CTL.pc


