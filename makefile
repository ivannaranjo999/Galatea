# Compile todo tool
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = todo

all: $(TARGET)

$(TARGET): todo.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f $(TARGET)

rebuild: clean all
