# Compiler and directories
CC := gcc
SRC_DIR := ./src
INC_DIR := ./include
BIN_DIR := ./bin
OBJ_DIR := $(BIN_DIR)/obj

# Flags
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g -Wno-unused-parameter
LDFLAGS =

# Target program
TARGET = $(BIN_DIR)/program

# Source and Object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean