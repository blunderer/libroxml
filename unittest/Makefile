
SRC1 = unittest.c roxml.test.c ../src/roxml.c ../src/roxml-internal.c
OBJ1 = $(SRC1:.c=.o)
SRC2 = roxml.perf.c ../src/roxml.c ../src/roxml-internal.c
OBJ2 = $(SRC2:.c=.o)
SRC3 = roxml.mem.c ../src/roxml.c ../src/roxml-internal.c
OBJ3 = $(SRC3:.c=.o)

CC = gcc
RM = rm -f

CFLAGS1 = -lefence -g -I../inc -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES
#CFLAGS2 = -pg -I../inc -DREENTRANT -DLINUX -Wall -Wextra -DIGNORE_EMPTY_TEXT_NODES
CFLAGS2 = -O3 -I../inc -DREENTRANT -DLINUX -Wall -Wextra -DIGNORE_EMPTY_TEXT_NODES
TARGET1 = roxml.unitC
TARGET2 = roxml.perf
TARGET3 = roxml.mem

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(OBJ1)
	$(CC) -o $@ $^

$(TARGET2): $(OBJ2)
	$(CC) -o $@ $^

$(TARGET3): $(OBJ3)
	$(CC) -o $@ $^

roxml.perf.o: roxml.perf.c
	$(CC) -c -o $@ $< $(CFLAGS2)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS1)

clean:
	$(RM) $(OBJ1)
	$(RM) $(OBJ2)
	$(RM) $(OBJ3)
	$(RM) out.xml.copy out.xml out.xml.human
	
mrproper: clean
	$(RM) $(TARGET1)
	$(RM) $(TARGET2)
	$(RM) $(TARGET3)

.PHONY: clean mrproper

