# Compile todo tool
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LIB = -lncurses
TARGET = galatea

all: $(TARGET)

$(TARGET): galatea.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LIB)

clean:
	rm -f $(TARGET)

rebuild: clean all
