# Simple Makefile - all outputs go to build/

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = 
LIBS = -lpthread

# All outputs go here
BUILD_DIR = build

# Source files
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=$(BUILD_DIR)/%.o)

# Final executable in build directory
TARGET = $(BUILD_DIR)/webserver

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile each .c file to .o in build/
$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link all .o files into executable in build/
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
