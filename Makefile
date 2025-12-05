# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O2 -g
LDFLAGS = -lncurses -lm
INCLUDES = -I./include

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/game/game_state.c \
       $(SRC_DIR)/ai/ai_controller.c \
       $(SRC_DIR)/ai/pathfinding.c \
       $(SRC_DIR)/data_structures/snake.c \
       $(SRC_DIR)/data_structures/queue.c \
       $(SRC_DIR)/data_structures/grid.c \
       $(SRC_DIR)/rendering/renderer.c \
       $(SRC_DIR)/rendering/ui_components.c \
       $(SRC_DIR)/utils/timer.c \
       $(SRC_DIR)/utils/memory_tracker.c

# Object files
OBJS = $(OBJ_DIR)/main.o \
       $(OBJ_DIR)/game/game_state.o \
       $(OBJ_DIR)/ai/ai_controller.o \
       $(OBJ_DIR)/ai/pathfinding.o \
       $(OBJ_DIR)/data_structures/snake.o \
       $(OBJ_DIR)/data_structures/queue.o \
       $(OBJ_DIR)/data_structures/grid.o \
       $(OBJ_DIR)/rendering/renderer.o \
       $(OBJ_DIR)/rendering/ui_components.o \
       $(OBJ_DIR)/utils/timer.o \
       $(OBJ_DIR)/utils/memory_tracker.o

# Binary
TARGET = $(BIN_DIR)/ouroboros

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR)/game
	@mkdir -p $(OBJ_DIR)/ai
	@mkdir -p $(OBJ_DIR)/data_structures
	@mkdir -p $(OBJ_DIR)/rendering
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(BIN_DIR)

# Link executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/game/%.o: $(SRC_DIR)/game/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/ai/%.o: $(SRC_DIR)/ai/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/data_structures/%.o: $(SRC_DIR)/data_structures/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/rendering/%.o: $(SRC_DIR)/rendering/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/utils/%.o: $(SRC_DIR)/utils/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the game
run: $(TARGET)
	./$(TARGET)

# Run in manual mode
manual: $(TARGET)
	./$(TARGET) --manual

# Run in AI mode (not yet implemented)
ai: $(TARGET)
	./$(TARGET) --ai

# Run with Valgrind for memory leak detection
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all directories run manual ai memcheck clean
