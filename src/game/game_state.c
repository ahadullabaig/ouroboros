#include "game_state.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>
#include <time.h>

GameState* game_state_create(GameMode mode) {
    GameState* state = (GameState*)memory_tracked_malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }

    /* Initialize random number generator */
    srand((unsigned int)time(NULL));

    /* Create snake in center, moving right */
    Position start_pos = {GRID_WIDTH / 2, GRID_HEIGHT / 2};
    state->snake = snake_create(start_pos, DIR_RIGHT);
    if (!state->snake) {
        memory_tracked_free(state);
        return NULL;
    }

    /* Initialize grid */
    grid_init(&state->grid);

    /* Set initial state */
    state->mode = mode;
    state->status = GAME_RUNNING;
    state->moves_count = 0;
    state->total_ai_time_us = 0;
    state->peak_memory_bytes = 0;
    state->show_path_overlay = true;
    state->show_safety_check = true;

    /* Sync grid with snake and spawn first food */
    game_state_sync_grid(state);
    game_state_spawn_food(state);

    return state;
}

void game_state_destroy(GameState* state) {
    if (!state) {
        return;
    }

    if (state->snake) {
        snake_destroy(state->snake);
    }

    memory_tracked_free(state);
}

void game_state_reset(GameState* state) {
    if (!state) {
        return;
    }

    /* Destroy old snake */
    if (state->snake) {
        snake_destroy(state->snake);
    }

    /* Create new snake */
    Position start_pos = {GRID_WIDTH / 2, GRID_HEIGHT / 2};
    state->snake = snake_create(start_pos, DIR_RIGHT);

    /* Reset grid */
    grid_init(&state->grid);

    /* Reset state variables */
    state->status = GAME_RUNNING;
    state->moves_count = 0;
    state->total_ai_time_us = 0;

    /* Sync and spawn food */
    game_state_sync_grid(state);
    game_state_spawn_food(state);
}

ResultCode game_state_update(GameState* state, Direction input_dir) {
    if (!state || !state->snake) {
        return ERROR_NULL_POINTER;
    }

    if (state->status != GAME_RUNNING) {
        return SUCCESS;  /* Game not running, no update */
    }

    /* Set snake direction if valid */
    if (input_dir != DIR_NONE) {
        snake_set_direction(state->snake, input_dir);
    }

    /* Get next head position */
    Position next_pos = snake_get_next_head_position(state->snake);

    /* Check if snake will eat food */
    bool will_eat = position_equals(next_pos, state->grid.food_pos);

    /* Move snake */
    ResultCode result = snake_move(state->snake, will_eat);
    if (result != SUCCESS) {
        return result;
    }

    state->moves_count++;

    /* Check for collisions */
    if (game_state_check_collision(state)) {
        state->status = GAME_OVER;
        return ERROR_COLLISION;
    }

    /* If ate food, spawn new food */
    if (will_eat) {
        game_state_sync_grid(state);
        game_state_spawn_food(state);

        /* Check for win condition */
        if (game_state_check_win(state)) {
            state->status = GAME_WON;
        }
    } else {
        game_state_sync_grid(state);
    }

    /* Update peak memory */
    uint64_t current_memory = memory_get_current_usage();
    if (current_memory > state->peak_memory_bytes) {
        state->peak_memory_bytes = current_memory;
    }

    return SUCCESS;
}

void game_state_spawn_food(GameState* state) {
    if (!state || !state->snake) {
        return;
    }

    /* Find all empty positions */
    Position empty_positions[GRID_SIZE];
    int empty_count = 0;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Position pos = {x, y};
            if (!snake_contains_position(state->snake, pos)) {
                empty_positions[empty_count++] = pos;
            }
        }
    }

    /* No empty positions? Game won! */
    if (empty_count == 0) {
        state->status = GAME_WON;
        return;
    }

    /* Pick random empty position */
    int random_index = rand() % empty_count;
    state->grid.food_pos = empty_positions[random_index];
    grid_set_cell(&state->grid, state->grid.food_pos, CELL_FOOD);
}

bool game_state_check_collision(const GameState* state) {
    if (!state || !state->snake || !state->snake->head) {
        return true;
    }

    Position head_pos = state->snake->head->pos;

    /* Check wall collision */
    if (!grid_is_valid_position(head_pos)) {
        return true;
    }

    /* Check self collision (with body, excluding head) */
    if (snake_collides_with_self(state->snake, head_pos)) {
        return true;
    }

    return false;
}

void game_state_sync_grid(GameState* state) {
    if (!state || !state->snake) {
        return;
    }

    /* Clear grid (keep food position) */
    grid_clear(&state->grid);

    /* Place food back */
    if (grid_is_valid_position(state->grid.food_pos)) {
        grid_set_cell(&state->grid, state->grid.food_pos, CELL_FOOD);
    }

    /* Place snake segments */
    SnakeSegment* current = state->snake->head;

    /* Head */
    if (current) {
        grid_set_cell(&state->grid, current->pos, CELL_SNAKE_HEAD);
        current = current->next;
    }

    /* Body */
    while (current && current->next) {
        grid_set_cell(&state->grid, current->pos, CELL_SNAKE_BODY);
        current = current->next;
    }

    /* Tail */
    if (current) {
        grid_set_cell(&state->grid, current->pos, CELL_SNAKE_TAIL);
    }
}

bool game_state_check_win(const GameState* state) {
    if (!state || !state->snake) {
        return false;
    }

    /* Win condition: snake length equals grid size (perfect game) */
    return state->snake->length >= GRID_SIZE;
}
