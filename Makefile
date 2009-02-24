# O (Output) is the build directory
ifeq ($O,)
O = .
endif

INC=roxml.h
SRC_LIB=roxml.c
SRC_TST=xshell.c
SRC_BIN=roxml-parser.c
OBJ_TST=$(SRC_TST:%.c=$O/%.o)
OBJ_BIN=$(SRC_BIN:%.c=$O/%.o)
OBJ_LIB=$(SRC_LIB:%.c=$O/%.o)

override CPPFLAGS +=
override CFLAGS += -Wall -Wextra -Werror
override LDFLAGS +=

TARGET_SLIB=$O/libroxml.a
TARGET_LIB=$O/libroxml.so
TARGET_BIN=$O/roxml
TARGET_TST=$O/xshell

all: $(TARGET_SLIB) $(TARGET_LIB) $(TARGET_BIN) $(TARGET_TST)

$(TARGET_TST): $(OBJ_TST) | $(if $(filter -static, $(LDFLAGS)), $(TARGET_SLIB), $(TARGET_LIB))
	$(CC) $(LDFLAGS) $^ -o $@ -L$O -lroxml

$(TARGET_BIN): $(OBJ_BIN) | $(if $(filter -static, $(LDFLAGS)), $(TARGET_SLIB), $(TARGET_LIB))
	$(CC) $(LDFLAGS) $^ -o $@ -L$O -lroxml

$(TARGET_SLIB): $(OBJ_LIB)
	$(AR) rc $@ $^

$(TARGET_LIB): $(OBJ_LIB)
	$(CC) -shared $(LDFLAGS) $^ -o $@

$O/%.o: $(notdir %.c) | $O
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$O:
	mkdir -p $O

doxy: doxy.cfg
	doxygen doxy.cfg

clean:
	- rm -f $(TARGET_BIN)
	- rm -f $(TARGET_SLIB)
	- rm -f $(TARGET_LIB)
	- rm -f $(TARGET_TST)
	- rm -f $(OBJ_LIB)
	- rm -f $(OBJ_BIN)
	- rm -f $(OBJ_TST)

mrproper: clean
	- rm -fr docs
	- fakeroot make -f debian/rules clean

install: $(TARGET) doxy
	mkdir -p $(DESTDIR)/usr/lib/ $(DESTDIR)/usr/bin/ $(DESTDIR)/usr/include $(DESTDIR)/usr/lib/pkgconfig $(DESTDIR)/usr/share/doc/libroxml/
	cp -a $(TARGET_SLIB) $(DESTDIR)/usr/lib/
	cp -a $(TARGET_LIB) $(DESTDIR)/usr/lib/
	cp -a $(TARGET_TST) $(DESTDIR)/usr/bin/
	cp -a $(TARGET_BIN) $(DESTDIR)/usr/bin/
	cp -a $(INC) $(DESTDIR)/usr/include
	cp -a libroxml.pc $(DESTDIR)/usr/lib/pkgconfig
	cp -a docs/html $(DESTDIR)/usr/share/doc/libroxml/

uninstall:
	- rm -f $(DESTDIR)/usr/lib/pkgconfig/libroxml.pc
	- rm -f $(DESTDIR)/usr/lib/$(TARGET_SLIB)
	- rm -f $(DESTDIR)/usr/lib/$(TARGET_LIB)
	- rm -f $(DESTDIR)/usr/bin/$(TARGET_TST)
	- rm -f $(DESTDIR)/usr/bin/$(TARGET_BIN)
	- rm -f $(DESTDIR)/usr/include/$(INC)
	- rm -fr $(DESTDIR)/usr/share/doc/libroxml

.PHONY: clean mrproper uninstall

