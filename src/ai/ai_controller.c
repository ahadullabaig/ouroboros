#include "ai_controller.h"
#include "pathfinding.h"
#include "../utils/timer.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>

AIDecision* ai_make_decision(GameState* state) {
    if (!state || !state->snake) {
        return NULL;
    }

    /* Allocate decision */
    AIDecision* decision = (AIDecision*)memory_tracked_malloc(sizeof(AIDecision));
    if (!decision) {
        return NULL;
    }

    decision->chosen_direction = DIR_NONE;
    decision->path_to_food = NULL;
    decision->safety_check = NULL;  /* Phase 3: No safety check yet */
    decision->total_compute_time_us = 0;
    decision->used_fallback = false;

    /* Start timing */
    Timer total_timer;
    timer_start(&total_timer);

    /* Sync grid with current snake position */
    game_state_sync_grid(state);

    Position head_pos = state->snake->head->pos;
    Position food_pos = state->grid.food_pos;

    /* Find path to food using BFS */
    decision->path_to_food = pathfinding_bfs(&state->grid, head_pos, food_pos);

    if (decision->path_to_food && decision->path_to_food->found &&
        decision->path_to_food->length >= 2) {
        /* Path found - get next direction */
        /* path[0] is current position, path[1] is next position */
        Position next_pos = decision->path_to_food->positions[1];
        decision->chosen_direction = pathfinding_get_next_direction(head_pos, next_pos);
    } else {
        /* No path to food - use fallback strategy (chase tail) */
        decision->chosen_direction = ai_fallback_strategy(state);
        decision->used_fallback = true;
    }

    /* Stop timing */
    timer_stop(&total_timer);
    decision->total_compute_time_us = timer_elapsed_us(&total_timer);

    /* Update state statistics */
    state->total_ai_time_us += decision->total_compute_time_us;

    return decision;
}

void ai_decision_destroy(AIDecision* decision) {
    if (!decision) {
        return;
    }

    if (decision->path_to_food) {
        pathfinding_free_result(decision->path_to_food);
    }

    /* safety_check will be freed in Phase 4 */

    memory_tracked_free(decision);
}

Direction ai_fallback_strategy(GameState* state) {
    if (!state || !state->snake || !state->snake->head || !state->snake->tail) {
        return DIR_RIGHT;  /* Default direction */
    }

    /* Chase own tail: find path from head to tail */
    Position head_pos = state->snake->head->pos;
    Position tail_pos = state->snake->tail->pos;

    /* Temporarily mark tail as walkable for pathfinding */
    CellType original_tail_type = grid_get_cell(&state->grid, tail_pos);
    grid_set_cell(&state->grid, tail_pos, CELL_EMPTY);

    PathResult* path_to_tail = pathfinding_bfs(&state->grid, head_pos, tail_pos);

    /* Restore tail */
    grid_set_cell(&state->grid, tail_pos, original_tail_type);

    Direction dir = state->snake->current_direction;  /* Keep current direction as default */

    if (path_to_tail && path_to_tail->found && path_to_tail->length >= 2) {
        Position next_pos = path_to_tail->positions[1];
        dir = pathfinding_get_next_direction(head_pos, next_pos);
    }

    pathfinding_free_result(path_to_tail);

    return dir;
}
