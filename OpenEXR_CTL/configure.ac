dnl Process this file with autoconf to produce a configure script.
AC_INIT(OpenEXR_CTL, 1.0.1)
AC_SUBST(OPENEXR_CTL_VERSION, 1.0.1)
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR(IlmImfCtlTest/main.cpp)
AM_INIT_AUTOMAKE(1.6.3)  dnl Require automake 1.6.3 or better
AM_MAINTAINER_MODE

LIBTOOL_CURRENT=2
LIBTOOL_REVISION=0
LIBTOOL_AGE=0
LIBTOOL_VERSION=$LIBTOOL_CURRENT:$LIBTOOL_REVISION:$LIBTOOL_AGE
AC_SUBST(LIBTOOL_VERSION)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

dnl
dnl PKGCONFIG preparations
dnl

if test -z "${PKG_CONFIG_PATH}"; then
	PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/pkgconfig
fi

export PKG_CONFIG_PATH

LIB64_IF_EXISTS=""
if [[ -e /usr/lib64 ]]; then
   LIB64_IF_EXISTS="-L/usr/lib64"
fi         

dnl
dnl get ccflags and libs from openexr packages, then check 
dnl whether test programs compile
AM_PATH_PKGCONFIG(
   [OPENEXR_CXXFLAGS],
   [OPENEXR_LDFLAGS],
   [OPENEXR_LIBS],
   [OpenEXR],
   [OpenEXR],
   [$LIB64_IF_EXISTS -L/usr/local/lib],
   [-lIlmImf -lImath -lIex -lHalf -lz],
   [openexr-prefix])

dnl
dnl get ccflags and libs from ctl packages, then check 
dnl whether test programs compile
AM_PATH_PKGCONFIG(
   [CTL_CXXFLAGS],
   [CTL_LDFLAGS],
   [CTL_LIBS],
   [CTL],
   [CTL OpenEXR],
   [$LIB64_IF_EXISTS -L/usr/local/lib],
   [-lIlmCtlSimd -lIlmCtlMath -lIlmCtl -lImath -lHalf -lIex -lIlmThread -lpthread],
   [ctl-prefix])

dnl --enable-threading
AC_ARG_ENABLE(threading,
              AC_HELP_STRING([--enable-threading],
                             [enable multi-threading [[default=yes]]]),
              [multithread="${enableval}"], [multithread=yes])

if test x$PKG_CONFIG == xno && test "x${multithread}" != xno ; then
    ACX_PTHREAD(
    [
	AC_DEFINE(HAVE_PTHREAD)
	ILMBASE_LIBS="$PTHREAD_LIBS $ILMBASE_LIBS"
	ILMBASE_CXXFLAGS="$ILMBASE_CXXFLAGS $PTHREAD_CFLAGS"
	CTL_LIBS="$PTHREAD_LIBS $CTL_LIBS"
	CTL_CXXFLAGS="$CTL_CXXFLAGS $PTHREAD_CFLAGS"
	CC="$PTHREAD_CC"
	
	AM_POSIX_SEM()
    ],
    [AC_MSG_ERROR([POSIX thread support required])])
    AC_MSG_NOTICE([multithread true, LIBS = $LIBS, CC = $CC, CXXFLAGS = $CXXFLAGS])
fi


AM_COMPILELINKRUN(
   [OpenEXR],
   [openexrtest],
   [$OPENEXR_CXXFLAGS],
   [$OPENEXR_LDFLAGS],
   [$OPENEXR_LIBS],[[
#include <stdlib.h>
#include <ImfTestFile.h>
]],
   [[Imf::isOpenExrFile("notExist");]],
   AC_MSG_RESULT([Compiled and ran OpenEXR test program.]), 
   AC_MSG_ERROR([Could not compile OpenEXR test program.]))




AM_COMPILELINKRUN(
   [CTL],
   [ctltest],
   [$CTL_CXXFLAGS],
   [$CTL_LDFLAGS],
   [$CTL_LIBS],[[
#include <CtlSimdInterpreter.h>
#include <Iex.h>
]],
   [[Ctl::SimdInterpreter interp; interp.setMaxInstCount(10);]],
   AC_MSG_RESULT([The ctl test program succeeded.]), 
   AC_MSG_ERROR([The ctl test program failed.]))

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T


dnl gcc optimization
AC_MSG_CHECKING(for gcc optimization flags)
old_cflags=$CFLAGS
CFLAGS="$CFLAGS -pipe"
AC_TRY_COMPILE([#include <stdio.h>],
[ printf ("hello, world"); ],
[ EXTRA_OPT_CFLAGS="-pipe"],[ EXTRA_OPT_CFLAGS=""])
CFLAGS=$old_cflags
AC_MSG_RESULT([$EXTRA_OPT_CFLAGS])


dnl Platform-specific stuff
case "$host" in
*darwin*) 
  AC_DEFINE(HAVE_DARWIN) 

  dnl OS X universal binary support, requires --disable-dependency-tracking
  AC_ARG_ENABLE(osx-universal-binaries,
	      AC_HELP_STRING([--enable-osx-universal-binaries],
			     [build universal binaries on OS X [[default=no]]]),
	      [build_osxuniversal="${enableval}"], [build_osxuniversal=no])

  if test "${build_osxuniversal}" != no ; then
    if test "$enable_dependency_tracking" != no ; then
      AC_MSG_ERROR([--enable-osx-universal-binary requires --disable-dependency-tracking.
Please re-run configure with these options:
  --disable-dependency-tracking --enable-osx-universal-binary
  		  ])
    fi
    CXXFLAGS="$CXXFLAGS -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386"
    dnl LDFLAGS="$LDFLAGS -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386"
  fi

  CXXFLAGS="$CXXFLAGS -Wno-long-double"
  ;;
esac

AM_CFLAGS="$EXTRA_OPT_CFLAGS"
AM_CXXFLAGS="$EXTRA_OPT_CFLAGS"

AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_CXXFLAGS)


dnl
dnl if some autoconf expert knows how to get the actual prefix used, please 
dnl tell us.  Until then...
dnl 
if test "x$prefix" != "xNONE"; then
   ACTUAL_PREFIX=$prefix
else
   ACTUAL_PREFIX=/usr/local
fi

dnl
dnl The -DCTLMODULE_BASE_PATH does not need to be passed to the .pc file, plus
dnl the quotes don't get passed through pkg-config properly
dnl
CTL_CXXFLAGS_PC="$CTL_CXXFLAGS"
CTL_CXXFLAGS="$CTL_CXXFLAGS -DCTL_MODULE_BASE_PATH=\\\"$ACTUAL_PREFIX/lib/CTL\\\""



AC_SUBST(OPENEXR_CXXFLAGS)
AC_SUBST(OPENEXR_LDFLAGS)
AC_SUBST(OPENEXR_LIBS)
AC_SUBST(CTL_CXXFLAGS)
AC_SUBST(CTL_CXXFLAGS_PC)
AC_SUBST(CTL_LDFLAGS)
AC_SUBST(CTL_LIBS)


AC_OUTPUT([
Makefile
OpenEXR_CTL.pc
IlmImfCtl/Makefile
IlmImfCtlTest/Makefile
CtlModules/Makefile
exrdpx/Makefile
exr_ctl_exr/Makefile
vc/Makefile
])

if test "x$build_osxuniversal" = xyes; then
  AC_MSG_RESULT([
---------------------------------------------
Summary for OpenEXR_CTL features:

build OS X universal binaries                   $build_osxuniversal
---------------------------------------------
  ])
else
  AC_MSG_RESULT([
---------------------------------------------
Summary for OpenEXR_CTL features:

No variable options in this package.  Configure completed successfully.
---------------------------------------------
  ])
fi

