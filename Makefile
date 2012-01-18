# O (Output) is the build directory
ifeq '$O' ''
O = .
endif
# V (Verbosity) is 0 (quiet) or 1 (verbose)
ifeq '$V' ''
V = 0
endif
# D (Debug) is 0 (optimize) or 1 (build debug version)
ifeq '$D' ''
D = 0
endif

# files
INC = inc/roxml.h
SRC_LIB = src/roxml.c src/roxml-internal.c src/roxml-parse-engine.c
SRC_BIN = src/roxml-parser.c
DEPS = $(patsubst %.c, $O/%.d, $(SRC_LIB) $(SRC_BIN))
OBJS = $(OBJ_LIB) $(OBJ_BIN)
OBJ_LIB = $(SRC_LIB:%.c=$O/%.o)
OBJ_BIN = $(SRC_BIN:%.c=$O/%.o)
TARGETS = $(TARGET_SLIB) $(TARGET_LN) $(TARGET_LIB) $(TARGET_BIN)
TARGET_SLIB = $O/libroxml.a
TARGET_LIB = $O/libroxml.so.0
TARGET_LN = $O/libroxml.so
TARGET_BIN = $O/roxml
BINS = $(TARGET_SLIB) $(TARGET_LIB) $(TARGET_LN) $(TARGET_BIN)

OS=$(shell uname)

# specific, modifiable flags
# set D=1 on command line to produce debuggable binary
ifeq ("$(OPTIM)", "")
ifeq '$D' '0'
OPTIM = -O3
else
OPTIM = -g -O0
endif
endif
DEFINES = -DIGNORE_EMPTY_TEXT_NODES

# options
override CPPFLAGS += -Iinc/
override CFLAGS += $(OPTIM) -fPIC -Wall -Wextra -Wno-unused -Werror -Iinc/ $(DEFINES)
override LDFLAGS += 

ifeq ("$(OS)", "Darwin")
	override LINKERFLAG += ""
	DEBIAN_RULES=Makefile
	FAKEROOT=
else
	override LINKERFLAG += -Wl,-soname,libroxml.so.0
	DEBIAN_RULES=debian/rules
	FAKEROOT="fakeroot"
endif

# first rule (default)
all:

# dependencies
ifeq ($(or \
	$(findstring doxy, $(MAKECMDGOALS)), \
	$(findstring clean, $(MAKECMDGOALS)), \
	$(findstring mrproper, $(MAKECMDGOALS)), \
	$(findstring uninstall, $(MAKECMDGOALS)) \
),)
-include $(DEPS)
endif

# rules verbosity
ifneq '$V' '0'
P = @ true
E =
else
P = @ echo
E = @
endif

# rules

$O/src :
	$P '  MKDIR   $(@F)'
	$E mkdir -p $@

$O/%.d : %.c | $O/src
	$P '  DEP     $(@F)'
	$E $(CC) -MM -MT '$@ $O/$*.o' $(CPPFLAGS) $< -MF $@ || rm -f $@

$O/%.o : %.c
	$P '  CC      $(@F)'
	$E $(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(TARGET_SLIB) : $(OBJ_LIB)
	$P '  AR      $(@F)'
	$E $(AR) rc $@ $^

$(TARGET_LIB) : $(OBJ_LIB)
	$P '  LD      $(@F)'
	$E $(CC) -shared $(LINKERFLAG) $(LDFLAGS) $^ -o $@

$(TARGET_LN): $(TARGET_LIB)
	$P '  LN      $(notdir $@)'
	$E - ln -fs $(shell basename $^) $@

$(TARGET_BIN): $(OBJ_BIN)
$(TARGET_BIN): | $(if $(filter -static, $(LDFLAGS)), $(TARGET_SLIB), $(TARGET_LIB))
	$P '  LD      $(@F)'
	$E $(CC) $(LDFLAGS) $^ -L$O -lroxml -lpthread -o $@

.PHONY : all
all : $(TARGET_SLIB) $(if $(filter -static, $(LDFLAGS)), , $(TARGET_LN)) $(TARGET_BIN)

.PHONY : doxy
doxy : doxy.cfg man.cfg
	$P '  DOXYGEN'
	$E - doxygen doxy.cfg &>/dev/null
	$E - cp data/icons/roxml.png docs/html/
	$E - cp data/icons/libroxml-ex.png docs/html/
	$P '  MAN'
	$E - doxygen man.cfg &>/dev/null
	$E - chmod -R a+rw docs

.PHONY: clean
clean:
	$P '  RM      deps objs bins'
	$E - rm -f $(DEPS) $(OBJS) $(BINS) 

.PHONY : mrproper
mrproper : clean
	$P '  RM      docs'
	$E - rm -fr docs/man docs/html docs/latex
	$P '  CLEAN   debian'
	$E - $(FAKEROOT) $(MAKE) -f $(abspath $(DEBIAN_RULES)) clean &>/dev/null
	$P '  CLEAN   fuse.xml'
	$E - $(MAKE) -C $(abspath fuse.xml) mrproper >/dev/null

.PHONY: fuse.xml
fuse.xml: $(TARGET_LN)
	$P '  BUILD   fuse.xml'
	$E - $(MAKE) -C $(abspath fuse.xml)

.PHONY: install
install: $(TARGETS) doxy
	$P '  INSTALL DIRS'
	$E mkdir -p $(DESTDIR)/usr/bin
	$E mkdir -p $(DESTDIR)/usr/include
	$E mkdir -p $(DESTDIR)/usr/lib/pkgconfig
	$E mkdir -p $(DESTDIR)/usr/share/man/man3
	$E mkdir -p $(DESTDIR)/usr/share/man/man1
	$E mkdir -p $(DESTDIR)/usr/share/doc/libroxml/html
	$P '  INSTALL FILES'
	$E install -D $(TARGET_SLIB) $(DESTDIR)/usr/lib
	$E install -D $(TARGET_LIB) $(DESTDIR)/usr/lib
	$E install -D $(TARGET_BIN) $(DESTDIR)/usr/bin
	$E install -D $(INC) $(DESTDIR)/usr/include
	$E install -D docs/roxml.1 $(DESTDIR)/usr/share/man/man1/
	$E [ ! -d docs/man/man3 ] || install -D docs/man/man3/* $(DESTDIR)/usr/share/man/man3/
	$E [ ! -d docs/html ] || install -D docs/html/* $(DESTDIR)/usr/share/doc/libroxml/html/
	$E install -m644 libroxml.pc $(DESTDIR)/usr/lib/pkgconfig
	$E cp -d $(TARGET_LN) $(DESTDIR)/usr/lib

.PHONY: uninstall
uninstall:
	$P '  UNINSTALL'
	$E - rm -f $(DESTDIR)/usr/lib/pkgconfig/libroxml.pc
	$E - rm -f $(DESTDIR)/usr/lib/$(TARGET_SLIB)
	$E - rm -f $(DESTDIR)/usr/lib/$(TARGET_LIB)
	$E - rm -f $(DESTDIR)/usr/bin/$(TARGET_BIN)
	$E - rm -f $(DESTDIR)/usr/include/$(INC)
	$E - rm -fr $(DESTDIR)/usr/share/doc/libroxml
	$E - rm -fr $(DESTDIR)/usr/share/man/man1/roxml.1
	$E - rm -fr $(DESTDIR)/usr/share/man/man3/roxml*
	$E - rm -fr $(DESTDIR)/usr/share/man/man3/ROXML*
	$E - rm -fr $(DESTDIR)/usr/share/man/man3/node_t.3
	$E - rm -fr $(DESTDIR)/usr/share/man/man3/RELEASE_ALL.3
	$E - rm -fr $(DESTDIR)/usr/share/man/man3/RELEASE_LAST.3

.PHONY: help
help:
	@echo
	@echo make [D=1] [V=1] [O=path] [OPTIM=optimize-options]
	@echo "   D=1: build debug version (default: D=0)"
	@echo "   V=1: verbose output (default: V=0)"
	@echo "   O=path: build binary in path (default: O=.)"
	@echo "   OPTIM=opt: force optimization options (default: OPTIM=-O3 if D=0, OPTIM=\"-g -O0\" if D=1)"
	@echo
	@echo "The default options build the binaries in the current directory, optimized for speed (-O3)"
	@echo

