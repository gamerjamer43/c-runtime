CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
SOURCES = main.c
HEADERS = dynarr.h typing.h ntypes.h
TARGET = app.exe

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	$(TARGET)

clean:
	del /Q $(TARGET)

.PHONY: all run clean
