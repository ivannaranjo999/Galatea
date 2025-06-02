# Compile todo tool
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LIB = -lncurses
TARGET = galatea
SRC = galatea.c
PREFIX ?= $(HOME)/.local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): galatea.c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIB)

install: $(TARGET)
	mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)
	@echo "Installed $(TARGET) to $(BINDIR)"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	@echo "Uninstalled $(TARGET) from $(BINDIR)"

clean:
	rm -f $(TARGET)

rebuild: clean all
