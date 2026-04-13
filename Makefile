CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

HEADERS = src/dynarr.h src/typing.h src/ntypes.h
SOURCES = main.c
TARGET = app.exe

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	$(TARGET)

clean:
	del /Q $(TARGET)

.PHONY: all run clean
