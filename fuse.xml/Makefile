
SRC=xmlfs.c
OBJ=$(SRC:.c=.o)
TARGET=mount.fuse.xml
CFLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=-L.. -lroxml -lfuse

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

install:
	cp $(TARGET) $(DESTDIR)/sbin/

uninstall:
	rm -f $(DESTDIR)/sbin/$(TARGET)

clean: 
	rm -f $(OBJ)
	rm -f $(TARGET)

.PHONY: clean
