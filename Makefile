
INC=roxml.h
SRC_LIB=roxml.c
SRC_TST=xshell.c
SRC_BIN=roxml-parser.c
OBJ_TST=$(SRC_TST:.c=.o)
OBJ_BIN=$(SRC_BIN:.c=.o)
OBJ_LIB=$(SRC_LIB:.c=.o)

LDFLAGS=
CFLAGS=-I. -g -Wall -Wextra

TARGET_LIB=libroxml.so
TARGET_BIN=roxml
TARGET_TST=xshell

all: $(TARGET_LIB) $(TARGET_BIN) $(TARGET_TST)

$(TARGET_TST): $(OBJ_TST)
	$(CC) $(LDFLAGS) -L. -lroxml $^ -o $@

$(TARGET_BIN): $(OBJ_BIN)
	$(CC) $(LDFLAGS) -L. -lroxml $^ -o $@

$(TARGET_LIB): $(OBJ_LIB)
	$(CC) -shared $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%-bin.o: %.c
	$(CC) -c $(CFLAGS) -DBUILD_AS_BIN $< -o $@

%-lib.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

doxy: doxy.cfg
	doxygen doxy.cfg

clean:
	rm -f $(TARGET_BIN)
	rm -f $(TARGET_LIB)
	rm -f $(TARGET_TST)
	rm -f $(OBJ_LIB)
	rm -f $(OBJ_BIN)
	rm -f $(OBJ_TST)

mrproper: clean
	rm -fr docs

install: $(TARGET) doxy
	cp -a $(TARGET_LIB) $(DESTDIR)/usr/lib/
	cp -a $(TARGET_TST) $(DESTDIR)/usr/bin/
	cp -a $(INC) $(DESTDIR)/usr/include

uninstall:
	rm -f $(DESTDIR)/usr/lib/$(TARGET_LIB)
	rm -f $(DESTDIR)/usr/bin/$(TARGET_TST)
	rm -f $(DESTDIR)/usr/include/$(INC)

.PHONY: clean mrproper uninstall

