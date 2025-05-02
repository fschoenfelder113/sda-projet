# Compiler
CC = gcc

# Flags
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g -Wno-unused-parameter

# Target program
TARGET = genealogie.c

# Default target
all: $(TARGET)

#Compilation
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@

# Clean up
clean:
	-@rm -rf $(TARGET)

.PHONY: all clean
