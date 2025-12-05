#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ============================================
 * GRID CONFIGURATION
 * ============================================ */
#define GRID_WIDTH 20
#define GRID_HEIGHT 20
#define GRID_SIZE (GRID_WIDTH * GRID_HEIGHT)

/* ============================================
 * GAME CONFIGURATION
 * ============================================ */
#define INITIAL_SNAKE_LENGTH 3
#define TARGET_FPS 10
#define FRAME_DELAY_MS (1000 / TARGET_FPS)
#define MAX_AI_COMPUTE_MS 10

/* Queue capacity for BFS (worst case: entire grid + 1) */
#define QUEUE_CAPACITY (GRID_SIZE + 1)

/* ============================================
 * CELL TYPES
 * ============================================ */
typedef enum {
    CELL_EMPTY = 0,
    CELL_SNAKE_HEAD,
    CELL_SNAKE_BODY,
    CELL_SNAKE_TAIL,
    CELL_FOOD,
    CELL_WALL
} CellType;

/* ============================================
 * DIRECTIONS
 * ============================================ */
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

/* ============================================
 * POSITION STRUCTURE
 * ============================================ */
typedef struct {
    int x;
    int y;
} Position;

/* Position comparison helper */
static inline bool position_equals(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

/* ============================================
 * RESULT CODES
 * ============================================ */
typedef enum {
    SUCCESS = 0,
    ERROR_OUT_OF_MEMORY = -1,
    ERROR_INVALID_POSITION = -2,
    ERROR_QUEUE_FULL = -3,
    ERROR_QUEUE_EMPTY = -4,
    ERROR_NULL_POINTER = -5,
    ERROR_INVALID_DIRECTION = -6,
    ERROR_COLLISION = -7
} ResultCode;

/* ============================================
 * GAME MODE
 * ============================================ */
typedef enum {
    MODE_MANUAL,
    MODE_AI_DEMO
} GameMode;

/* ============================================
 * GAME STATUS
 * ============================================ */
typedef enum {
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WON  /* Achieved perfect game (all cells visited) */
} GameStatus;

/* ============================================
 * NCURSES COLOR PAIRS
 * ============================================ */
enum {
    COLOR_PAIR_EMPTY = 1,
    COLOR_PAIR_SNAKE_HEAD,
    COLOR_PAIR_SNAKE_BODY,
    COLOR_PAIR_SNAKE_TAIL,
    COLOR_PAIR_FOOD,
    COLOR_PAIR_WALL,
    COLOR_PAIR_PATH_OVERLAY,
    COLOR_PAIR_SAFETY_CHECK,
    COLOR_PAIR_UI_BORDER,
    COLOR_PAIR_UI_TEXT
};

/* ============================================
 * DIRECTION HELPERS
 * ============================================ */

/* Check if two directions are opposite */
static inline bool direction_is_opposite(Direction a, Direction b) {
    return (a == DIR_UP && b == DIR_DOWN) ||
           (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

/* Get position offset for a direction */
static inline Position direction_to_offset(Direction dir) {
    Position offset = {0, 0};
    switch (dir) {
        case DIR_UP:    offset.y = -1; break;
        case DIR_DOWN:  offset.y = 1;  break;
        case DIR_LEFT:  offset.x = -1; break;
        case DIR_RIGHT: offset.x = 1;  break;
        case DIR_NONE:  break;
    }
    return offset;
}

/* Get next position given current position and direction */
static inline Position position_move(Position pos, Direction dir) {
    Position offset = direction_to_offset(dir);
    Position new_pos = {pos.x + offset.x, pos.y + offset.y};
    return new_pos;
}

#endif /* COMMON_H */
