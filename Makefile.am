## Process this file with automake to generate Makefile.in

VERSION_INFO = -version-info $(SONAME_CURRENT):$(SONAME_REVISION):$(SONAME_AGE)

if DEBUG
  DEBUG_CFLAGS = -DDEBUG
endif

AUTOMAKE_OPTIONS = foreign nostdinc
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I. -I$(top_srcdir) -I$(top_srcdir)/inc
AM_LD = $(CC)
AM_CFLAGS = -Wall $(DEBUG_CFLAGS)
EXTRA_DIST =

# Project headers
include_HEADERS = inc/roxml.h
noinst_HEADERS = \
	inc/roxml-defines.h \
	inc/roxml-doxy.h \
	inc/roxml-internal.h \
	inc/roxml-parse-engine.h \
	inc/roxml-tune.h \
	inc/roxml-types.h \
	inc/roxml-win32-native.h

# Libraries
lib_LTLIBRARIES = libroxml.la

# The libroxml library
libroxml_la_SOURCES = \
	src/roxml.c \
	src/roxml-internal.c \
	src/roxml-parse-engine.c
libroxml_la_LDFLAGS = $(VERSION_INFO)

# Binaries
bin_PROGRAMS = roxml

# The roxml utility
roxml_SOURCES = src/roxml-parser.c
roxml_LDADD = libroxml.la

# mount.fuse.xml helper
if WITH_FUSE
  bin_PROGRAMS += mount.fuse.xml
  mount_fuse_xml_SOURCES = fuse.xml/xmlfs.c
  mount_fuse_xml_CFLAGS = -D_FILE_OFFSET_BITS=64 $(FUSE_CFLAGS)
  mount_fuse_xml_LDADD = libroxml.la $(FUSE_LIBS)
endif
EXTRA_DIST += fuse.xml/README

# pkg-config .pc file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libroxml.pc
EXTRA_DIST += libroxml.pc.in

# Additional project files
EXTRA_DIST += \
	License.txt \
	CMakeLists.txt \
	debian \
	libroxml.spec

# Documentation
EXTRA_DIST += \
	doxy.cfg \
	man.cfg \
	data \
	docs

if DOCS
docs: doxygen-docs
doxygen-docs: doxygen-html doxygen-man

doxygen-html:
	$(AM_V_GEN)$(DOXYGEN) doxy.cfg >/dev/null 2>&1
	@cp data/icons/roxml.png docs/html/
	@cp data/icons/libroxml-ex.png docs/html/

doxygen-man:
	$(AM_V_GEN)$(DOXYGEN) man.cfg >/dev/null 2>&1
	@chmod -R a+rw docs
	@rm docs/man/man3/*_inc_.3

clean-local: docs-clean
docs-clean:
	rm -fr docs/man docs/html docs/latex

man1_MANS = docs/roxml.1
nodist_man3_MANS = docs/man/man3/*.3

nodist_doc_DATA = docs/html/*

$(man1_MANS): docs
$(nodist_man3_MANS): docs
$(nodist_doc_DATA): docs

endif
