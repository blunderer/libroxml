
SRC = unittest.c roxml.test.c ../roxml.c
OBJ = $(SRC:.c=.o)

CC = gcc
RM = rm -f

CFLAGS = -g -DUNITTEST -DREENTRANT -DLINUX -Wall -Wextra


TARGET = roxml.unitC

$(LIB): $(TARGET) clean

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(OBJ)
	
mrproper: clean
	$(RM) $(TARGET)

.PHONY: clean mrproper

