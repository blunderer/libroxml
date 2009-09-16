
SRC1 = unittest.c roxml.test.c ../roxml.c
OBJ1 = $(SRC1:.c=.o)
SRC2 = roxml.perf.c ../roxml.c
OBJ2 = $(SRC2:.c=.o)

CC = gcc
RM = rm -f

CFLAGS = -g -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra
TARGET1 = roxml.unitC
TARGET2 = roxml.perfs

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJ1)
	$(CC) -o $@ $^

$(TARGET2): $(OBJ2)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(OBJ1)
	$(RM) $(OBJ2)
	
mrproper: clean
	$(RM) $(TARGET1)
	$(RM) $(TARGET2)

.PHONY: clean mrproper

