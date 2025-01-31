# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2
INCLUDES = -I./include -I$(RAYLIB_PATH)/include
LDFLAGS = -L$(RAYLIB_PATH)/lib
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Raylib path (adjust as needed)
RAYLIB_PATH = C:/raylib

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/core/*.c) \
       $(wildcard $(SRC_DIR)/entities/*.c) \
       $(wildcard $(SRC_DIR)/world/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output executable
TARGET = $(BIN_DIR)/shadow_worker

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/entities
	@mkdir -p $(BUILD_DIR)/world
	@mkdir -p $(BIN_DIR)

# Link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the game
run: all
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: all

# Install dependencies (Windows with MSYS2/MinGW)
install-deps:
	pacman -S mingw-w64-x86_64-raylib

.PHONY: all clean run debug release install-deps directories 