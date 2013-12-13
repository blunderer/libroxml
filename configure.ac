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
# Build roxml binary
################################
AC_ARG_ENABLE(roxml,
              [AS_HELP_STRING([--disable-roxml],
              [Disable roxml binary build [default=yes]])],
              [],
              [enable_roxml=yes])
AM_CONDITIONAL(ENABLE_ROXML,test x$enable_roxml != xno)

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
# Tune options
################################
AC_DEFUN([AC_ROXML_TUNE],
[
    AC_ARG_ENABLE($1,
                  [AS_HELP_STRING([$3],
                  [$4 [default=$2]])],
                  [
                      if test x$enableval = xno; then
                          $1=no
                      else
                          $1=yes
                      fi
                  ],
                  [$1=$2]
    )
    if test x$$1 != xno; then
        roxml_tune=1
    else
        roxml_tune=0
    fi
    AC_DEFINE_UNQUOTED([$5], [$roxml_tune], [$4 [default=$2]])
    AC_MSG_NOTICE([Tune option $5 set to $roxml_tune])
])

# CONFIG_XML_THREAD_SAFE
AC_ROXML_TUNE(xml_thread_safe,
              [yes],
              [--disable-xml-thread-safe],
              [Is the target library thread safe or not.],
              [CONFIG_XML_THREAD_SAFE])

# CONFIG_XML_HEAP_BUFFERS
AC_ROXML_TUNE(xml_heap_buffers,
              [no],
              [--enable-xml-heap-buffers],
              [Is the target library uses stack for internal buffers or heap. This is not compatible with thread safety.],
              [CONFIG_XML_HEAP_BUFFERS])

# CONFIG_XML_SMALL_BUFFER
AC_ROXML_TUNE(xml_small_buffer,
              [no],
              [--enable-xml-small-buffer],
              [Define the size of internal buffer. For very small systems, large internal buffers can cause the systeml the behave strangely.],
              [CONFIG_XML_SMALL_BUFFER])

# CONFIG_XML_SMALL_INPUT_FILE
AC_ROXML_TUNE(xml_small_input_file,
              [no],
              [--enable-xml-small-input-file],
              [Limit the size of input XML libroxml can handle to 64kb instead of 4Gb. Setting this option will reduce the size of RAM loaded tree.],
              [CONFIG_XML_SMALL_INPUT_FILE])

# CONFIG_XML_READ_WRITE
AC_ROXML_TUNE(xml_read_write,
              [yes],
              [--disable-xml-read-write],
              [Can the target library modify XML tree. When set to 0, this result in smaller binary (this saves about 3kb).],
              [CONFIG_XML_READ_WRITE])

# CONFIG_XML_COMMIT_XML_TREE
AC_ROXML_TUNE(xml_commit_xml_tree,
              [yes],
              [--disable-xml-commit-xml-tree],
              [Can the target library commit a tree. When set to 0, this result in smaller binary (this saves about 5kb).],
              [CONFIG_XML_COMMIT_XML_TREE])

# CONFIG_XML_XPATH_ENGINE
AC_ROXML_TUNE(xml_xpath_engine,
              [yes],
              [--disable-xml-xpath-engine],
              [Can the target library perform xpath queries. When set to 0, this result in smaller binary (this saves about 20kb).],
              [CONFIG_XML_XPATH_ENGINE])

################################
# Output files
################################
AC_CONFIG_FILES(Makefile libroxml.pc)
AC_OUTPUT