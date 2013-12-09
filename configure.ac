#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([MAJOR_VERSION], 2)
m4_define([MINOR_VERSION], 2)
m4_define([MICRO_VERSION], 3)

AC_PREREQ(2.59)
AC_INIT([libroxml],
        [MAJOR_VERSION.MINOR_VERSION.MICRO_VERSION],
        [tristan.lelong@libroxml.net])
AC_CONFIG_AUX_DIR(aux_scripts)
AC_CONFIG_MACRO_DIR([m4])

# Values for SONAME. See -version-info for details.
AC_SUBST(SONAME_CURRENT, 0)
AC_SUBST(SONAME_REVISION, 0)
AC_SUBST(SONAME_AGE, 0)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

AC_CONFIG_HEADERS([inc/config.h])
AM_INIT_AUTOMAKE(subdir-objects)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_SILENT_RULES([yes])

# needed to handle 64-bit architecture
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(size_t)

################################
# Debugging options
################################
AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
              [Turn on debugging [default=no]])],
              [],
              [enable_debug=no])
AM_CONDITIONAL(DEBUG,test x$enable_debug = xyes)

################################
# Fuse helper
################################
AC_ARG_WITH(fuse,
              [AS_HELP_STRING([--with-fuse],
              [Build fuse helper [default=no]])],
              [],
              [with_fuse=no])
AM_CONDITIONAL(WITH_FUSE,test x$with_fuse = xyes)
if test x$with_fuse = xyes; then
    AC_CHECK_HEADERS([fuse.h],
                     [
                     PKG_CHECK_MODULES([FUSE], [fuse], [], [
                         AC_SEARCH_LIBS([fuse_main], [fuse], [], [
                             AC_MSG_ERROR([unable to find the fuse library])
                         ])
                     ])
                     ],
                     AC_MSG_ERROR([fuse.h not found: the fuse xml helper cannot be built]),
                     [
#define _FILE_OFFSET_BITS 64
#include <fuse.h>
                     ]
    )
fi

################################
# Documentation
################################
AC_ARG_ENABLE(docs,
              [AS_HELP_STRING([--enable-docs],
              [Enable documentation build [default=no]])],
              [],
              [enable_docs=no])
AM_CONDITIONAL(DOCS,test x$enable_docs != xno)
if test x$enable_docs != xno; then
    AC_PATH_PROG([DOXYGEN], [doxygen])
    if test x$DOXYGEN = x; then
        AC_MSG_ERROR([doxygen not found: the docs cannot be built])
    fi
fi

################################
# Output files
################################
AC_CONFIG_FILES(Makefile libroxml.pc)
AC_OUTPUT
