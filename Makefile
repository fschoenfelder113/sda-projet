# Compiler
CC = gcc

# Flags
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g

# Target program
TARGET = genealogie

# Default target
all: $(TARGET)

#Compilation
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@

# Clean up
clean:
	-@rm -rf $(TARGET)

.PHONY: all clean
