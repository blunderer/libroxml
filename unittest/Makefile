# V (Verbosity) is 0 (quiet) or 1 (verbose)
ifeq ($V,0)
override V =
endif

SRC1 = unittest.c roxml.test.c ../src/roxml.c ../src/roxml-internal.c ../src/roxml-parse-engine.c
OBJ1 = $(SRC1:.c=.o)
SRC2 = roxml.perf.c ../src/roxml.c ../src/roxml-internal.c ../src/roxml-parse-engine.c
OBJ2 = $(SRC2:.c=.o)
SRC3 = roxml.mem.c ../src/roxml.c ../src/roxml-internal.c ../src/roxml-parse-engine.c
OBJ3 = $(SRC3:.c=.o)

RM = rm -f

CFLAGS1 = -lefence -g -I../inc -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra -Wno-unused -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES
CFLAGS2 = -O3 -I../inc -DREENTRANT -DLINUX -Wall -Wextra -Wno-unused -DIGNORE_EMPTY_TEXT_NODES
TARGET1 = roxml.unitC
TARGET2 = roxml.perf
TARGET3 = roxml.mem

# rules verbosity
define ECHO_DO
@ $(if $V, echo $2, $(if $(strip $1), echo $1))
@ $2
endef

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(OBJ1)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^)

$(TARGET2): $(OBJ2)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^ )

$(TARGET3): $(OBJ3)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) -o $@ $^ )

roxml.perf.o: roxml.perf.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) -c -o $@ $< $(CFLAGS2) )

%.o: %.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) -c -o $@ $< $(CFLAGS1) )

clean:
	$(call ECHO_DO, '  RM      objs', \
	$(RM) $(OBJ1) $(OBJ2) $(OBJ3) )
	$(call ECHO_DO, '  RM      results files', \
	$(RM) out.xml.copy out.xml out.xml.human )
	
mrproper: clean
	$(call ECHO_DO, '  RM      bins', \
	$(RM) $(TARGET1) $(TARGET2) $(TARGET3) )

.PHONY: clean mrproper

