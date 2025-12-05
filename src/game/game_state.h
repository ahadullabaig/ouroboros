#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../../include/common.h"
#include "../data_structures/snake.h"
#include "../data_structures/grid.h"

/**
 * Central game state management
 * Coordinates snake, grid, and game logic
 */
typedef struct {
    Snake* snake;
    Grid grid;
    GameMode mode;
    GameStatus status;
    uint32_t moves_count;
    uint64_t total_ai_time_us;     /* Accumulated AI decision time */
    uint64_t peak_memory_bytes;
    bool show_path_overlay;
    bool show_safety_check;
} GameState;

/**
 * Create a new game state
 * @param mode Game mode (manual or AI)
 * @return Pointer to new game state or NULL on failure
 */
GameState* game_state_create(GameMode mode);

/**
 * Destroy game state and free resources
 * @param state Game state to destroy
 */
void game_state_destroy(GameState* state);

/**
 * Reset game to initial state
 * @param state Game state to reset
 */
void game_state_reset(GameState* state);

/**
 * Update game state for one frame
 * @param state Game state to update
 * @param input_dir Direction input (from user or AI)
 * @return SUCCESS or error code
 */
ResultCode game_state_update(GameState* state, Direction input_dir);

/**
 * Spawn food at a random empty position
 * @param state Game state to update
 */
void game_state_spawn_food(GameState* state);

/**
 * Check for collisions (walls or self)
 * @param state Game state to check
 * @return true if collision detected, false otherwise
 */
bool game_state_check_collision(const GameState* state);

/**
 * Synchronize grid with current snake position
 * Updates grid cells to reflect snake body
 * @param state Game state to synchronize
 */
void game_state_sync_grid(GameState* state);

/**
 * Check if game is won (perfect game - all cells visited)
 * @param state Game state to check
 * @return true if won, false otherwise
 */
bool game_state_check_win(const GameState* state);

#endif /* GAME_STATE_H */
