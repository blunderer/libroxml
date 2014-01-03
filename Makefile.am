## Process this file with automake to generate Makefile.in

VERSION_INFO = -version-info $(SONAME_CURRENT):$(SONAME_REVISION):$(SONAME_AGE)

if DEBUG
  DEBUG_CFLAGS = -DDEBUG
endif

AUTOMAKE_OPTIONS = foreign nostdinc
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src
AM_LD = $(CC)
AM_CFLAGS = -Wall $(DEBUG_CFLAGS)
EXTRA_DIST =
CLEANFILES =

# Project headers
include_HEADERS = src/roxml.h
noinst_HEADERS = \
	src/roxml-defines.h \
	src/roxml-internal.h \
	src/roxml-parse-engine.h \
	src/roxml-tune.h \
	src/roxml-types.h \
	src/roxml-win32-native.h
EXTRA_DIST += src/roxml-doxy.h.in

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
	CMakeLists.txt

# Documentation
EXTRA_DIST += \
	data/html \
	data/icons \
	docs

if DOCS
docs: doxygen-docs
doxygen-docs: doxygen-html doxygen-man

doxygen-html:
	$(AM_V_GEN)$(DOXYGEN) docs/doxy.cfg >/dev/null 2>&1
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/roxml.png docs/html/
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/libroxml-ex.png docs/html/

doxygen-man:
	$(AM_V_GEN)$(DOXYGEN) docs/man.cfg >/dev/null 2>&1
	$(AM_V_GEN)chmod -R a+rw docs

clean-local: docs-clean
docs-clean:
	rm -Rf \
	docs/man \
	docs/html \
	docs/latex

man1_MANS = $(top_srcdir)/docs/roxml.1
nodist_man3_MANS = docs/man/man3/*.3

nodist_html_DATA = docs/html/*

$(man1_MANS): docs
$(nodist_man3_MANS): docs
$(nodist_html_DATA): docs

endif

# Unit tests

if ENABLE_TESTS

noinst_bin_PROGRAMS = \
	unittest/roxml.unitC \
	unittest/roxml.perf \
	unittest/roxml.mem \
	unittest/roxml.tiny.unitC
noinst_bindir = unittest

nodist_noinst_HEADERS = unittest/unittest.h

# Test libraries
noinst_LIBRARIES = \
		unittest/libroxml_test.a \
		unittest/libroxml_test_tiny.a

TEST_CFLAGS = -DUNITTEST -DREENTRANT -DLINUX -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES -Wall -Wextra @AM_TEST_CFLAGS@

# Test library libroxml_test.a
unittest_libroxml_test_a_SOURCES = \
	src/roxml.c \
	src/roxml-internal.c \
	src/roxml-parse-engine.c
unittest_libroxml_test_a_CFLAGS = $(TEST_CFLAGS)

# Test library libroxml_test_tiny.a
unittest_libroxml_test_tiny_a_SOURCES = \
	src/roxml.c \
	src/roxml-internal.c \
	src/roxml-parse-engine.c
unittest_libroxml_test_tiny_a_CFLAGS = \
	$(TEST_CFLAGS) \
	-UHAVE_CONFIG_H \
	-DCONFIG_XML_HEAP_BUFFERS=1 \
	-DCONFIG_XML_SMALL_BUFFER=1 \
	-DCONFIG_XML_SMALL_INPUT_FILE=1 \
	-DCONFIG_XML_THREAD_SAFE=0 \
	-DCONFIG_XML_READ_WRITE=0 \
	-DCONFIG_XML_COMMIT_XML_TREE=0 \
	-DCONFIG_XML_XPATH_ENGINE=0

# roxml.unitC
nodist_unittest_roxml_unitC_SOURCES = \
	unittest/unittest.c \
	unittest/roxml.test.c
unittest_roxml_unitC_LDADD = unittest/libroxml_test.a
unittest_roxml_unitC_CFLAGS = $(TEST_CFLAGS)

# roxml.perf
nodist_unittest_roxml_perf_SOURCES = \
	unittest/roxml.perf.c
unittest_roxml_perf_LDADD = unittest/libroxml_test.a
unittest_roxml_perf_CFLAGS = -O3 -DREENTRANT -DLINUX -DIGNORE_EMPTY_TEXT_NODES -Wall

# roxml.mem
nodist_unittest_roxml_mem_SOURCES = unittest/roxml.mem.c
unittest_roxml_mem_LDADD = unittest/libroxml_test.a
unittest_roxml_mem_CFLAGS = $(TEST_CFLAGS)

# roxml.tiny.unitC
nodist_unittest_roxml_tiny_unitC_SOURCES = unittest/unittest.c
unittest_roxml_tiny_unitC_LDADD = unittest/libroxml_test_tiny.a
unittest_roxml_tiny_unitC_CFLAGS = $(TEST_CFLAGS)

CLEANFILES += \
		unittest/out.xml.copy \
		unittest/out.xml out.xml.human \
		unittest/out.xml.spec.copy \
		unittest/out.xml.valid.human \
		unittest/out.buf.xml \
		unittest/out.buf.xml.human \
		unittest/out.xml.ns.generated \
		unittest/out.xml.ns \
		unittest/out.xml.human.sub \
		unittest/out.xml.ns.del \
		unittest/out.xml.modattr \
		unittest/out.xml.modattr2 \
		unittest/out.xml.modattr3 \
		unittest/out.xml.modattr4 \
		unittest/out.xml.ns.generated2 \
		unittest/out.xml.valid.raw \
		unittest/out.xml.valid.raw2 \
		unittest/out.xml.valid.raw3 \
		unittest/out.xml.valid.raw4 \
		unittest/out.xml.valid.xpath \
		unittest/out.xml.valid.xpath2 \
		unittest/out.xml.valid.xpath3 \
		unittest/out.xml.valid.xpath4

endif
