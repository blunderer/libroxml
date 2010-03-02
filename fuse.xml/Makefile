# V (Verbosity) is 0 (quiet) or 1 (verbose)
ifeq ($V,0)
override V =
endif

SRC=xmlfs.c
OBJ=$(SRC:.c=.o)
TARGET=mount.fuse.xml
CFLAGS=-g -D_FILE_OFFSET_BITS=64
LDFLAGS=-L.. -lroxml -lfuse

# rules verbosity
define ECHO_DO
@ $(if $V, echo $2, $(if $(strip $1), echo $1))
@ $2
endef

$(TARGET): $(OBJ)
	$(call ECHO_DO, '  LD      $(notdir $@)', \
	$(CC) $(LDFLAGS) -o $@ $^ )

%.o: %.c
	$(call ECHO_DO, '  CC      $(notdir $@)', \
	$(CC) $(CFLAGS) -c -o $@ $^ )

install:
	$(call ECHO_DO, '  INSTALL fuse.xml', \
	cp $(TARGET) $(DESTDIR)/sbin/ )

uninstall:
	$(call ECHO_DO, '  UNINST  fuse.xml', \
	rm -f $(DESTDIR)/sbin/$(TARGET) )

clean: 
	$(call ECHO_DO, '  RM      obj', \
	rm -f $(OBJ) )

mrproper: clean
	$(call ECHO_DO, '  RM      bin', \
	rm -f $(TARGET) )

.PHONY: mrproper clean
