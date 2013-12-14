# V (Verbosity) is 0 (quiet) or 1 (verbose)
ifeq ($V,0)
override V =
endif

OBJ1 = unittest.o roxml.test.o ../src/roxml.o ../src/roxml-internal.o ../src/roxml-parse-engine.o
OBJ2 = roxml.perf.o ../src/roxml.o ../src/roxml-internal.o ../src/roxml-parse-engine.o
OBJ3 = roxml.mem.o ../src/roxml.o ../src/roxml-internal.o ../src/roxml-parse-engine.o
OBJ4 = unittest.o roxml.test-tiny.o ../src/roxml-tiny.o ../src/roxml-internal-tiny.o ../src/roxml-parse-engine-tiny.o

RM = rm -f

NOUNUSED = -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable

CFLAGS1 = -lefence -g -I../inc -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra $(NOUNUSED) -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES
CFLAGS2 = -O3 -I../inc -DREENTRANT -DLINUX -Wall -Wextra $(NOUNUSED) -DIGNORE_EMPTY_TEXT_NODES
CFLAGS4 = -lefence -g -I../inc -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra $(NOUNUSED) -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES

CFLAGS4 += -DCONFIG_XML_HEAP_BUFFERS=1
CFLAGS4 += -DCONFIG_XML_SMALL_BUFFER=1
CFLAGS4 += -DCONFIG_XML_SMALL_INPUT_FILE=1
CFLAGS4 += -DCONFIG_XML_THREAD_SAFE=0
CFLAGS4 += -DCONFIG_XML_READ_WRITE=0
CFLAGS4 += -DCONFIG_XML_COMMIT_XML_TREE=0
CFLAGS4 += -DCONFIG_XML_XPATH_ENGINE=0

TARGET1 = roxml.unitC
TARGET2 = roxml.perf
TARGET3 = roxml.mem
TARGET4 = roxml.tiny.unitC

# rules verbosity
define ECHO_DO
@ $(if $V, echo $2, $(if $(strip $1), echo $1))
@ $2
endef

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET1): $(OBJ1)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^)

$(TARGET2): $(OBJ2)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^ )

$(TARGET3): $(OBJ3)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^ )

$(TARGET4): $(OBJ4)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^ )

roxml.perf.o: roxml.perf.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) -c -o $@ $< $(CFLAGS2) )

%.o: %.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) -c -o $@ $< $(CFLAGS1) )

%-tiny.o: %.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) -c -o $@ $< $(CFLAGS4) )

clean:
	$(call ECHO_DO, '  RM      objs', \
	$(RM) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) )
	$(call ECHO_DO, '  RM      results files', \
	$(RM) out.xml.copy out.xml out.xml.human out.xml.spec.copy \
	  out.xml.valid.human out.buf.xml out.buf.xml.human \
	  out.xml.ns.generated out.xml.ns out.xml.human.sub \
	  out.xml.ns.del out.xml.modattr out.xml.modattr2 \
	  out.xml.modattr3 out.xml.modattr4 out.xml.ns.generated2 \
	  out.xml.valid.raw out.xml.valid.raw2 out.xml.valid.raw3 \
	  out.xml.valid.raw4 out.xml.valid.xpath out.xml.valid.xpath2 \
	  out.xml.valid.xpath3 out.xml.valid.xpath4)
	
mrproper: clean
	$(call ECHO_DO, '  RM      bins', \
	$(RM) $(TARGET1) $(TARGET2) $(TARGET3) )

.PHONY: clean mrproper

