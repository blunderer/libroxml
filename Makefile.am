## Process this file with automake to generate Makefile.in

VERSION_INFO = -version-info $(SONAME_CURRENT):$(SONAME_REVISION):$(SONAME_AGE)

if DEBUG
  DEBUG_CFLAGS = -DDEBUG
endif

AUTOMAKE_OPTIONS = foreign nostdinc
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I./inc -I$(top_srcdir)/inc
AM_LD = $(CC)
AM_CFLAGS = -Wall $(DEBUG_CFLAGS)
EXTRA_DIST =

# Project headers
include_HEADERS = inc/roxml.h
noinst_HEADERS = \
	inc/roxml-defines.h \
	inc/roxml-internal.h \
	inc/roxml-parse-engine.h \
	inc/roxml-tune.h \
	inc/roxml-types.h \
	inc/roxml-win32-native.h
EXTRA_DIST += inc/roxml-doxy.h.in

# Libraries
lib_LTLIBRARIES = libroxml.la

# The libroxml library
libroxml_la_SOURCES = \
	src/roxml.c \
	src/roxml-internal.c \
	src/roxml-parse-engine.c
libroxml_la_LDFLAGS = $(VERSION_INFO)

bin_PROGRAMS =

# The roxml utility
if ENABLE_ROXML
  bin_PROGRAMS += roxml
  roxml_SOURCES = src/roxml-parser.c
  roxml_LDADD = libroxml.la
endif

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
	doxy.cfg.in \
	man.cfg.in \
	data \
	docs

if DOCS
docs: doxygen-docs
doxygen-docs: doxygen-html doxygen-man

doxygen-html:
	$(AM_V_GEN)$(DOXYGEN) doxy.cfg >/dev/null 2>&1
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/roxml.png docs/html/
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/libroxml-ex.png docs/html/

doxygen-man:
	$(AM_V_GEN)$(DOXYGEN) man.cfg >/dev/null 2>&1
	$(AM_V_GEN)chmod -R a+rw docs
	$(AM_V_GEN)rm docs/man/man3/*_inc_.3

clean-local: docs-clean
docs-clean:
	rm -fr docs/man docs/html docs/latex

man1_MANS = $(top_srcdir)/docs/roxml.1
nodist_man3_MANS = docs/man/man3/*.3

nodist_doc_DATA = docs/html/*

$(man1_MANS): docs
$(nodist_man3_MANS): docs
$(nodist_doc_DATA): docs

endif
