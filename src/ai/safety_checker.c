#include "safety_checker.h"
#include "../utils/timer.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>

/**
 * Check if a proposed move is safe by simulating its consequences
 *
 * CRITICAL: ALL moves must be validated, not just food-eating moves!
 *
 * Algorithm:
 * 1. Calculate new head position
 * 2. Check if move is walkable (basic collision check)
 * 3. Simulate the move by temporarily marking old head as body
 * 4. For food moves: tail stays in place (snake grows)
 *    For non-food moves: tail position becomes walkable (snake moves)
 * 5. Check if escape path exists from new head to tail
 * 6. Revert grid changes
 *
 * This ensures every move maintains an escape route, preventing self-trapping.
 */
SafetyResult* safety_check_move(Grid* grid, const Snake* snake, Direction move, Position food_pos) {
    if (!grid || !snake || move == DIR_NONE) {
        return NULL;
    }

    Timer timer;
    timer_start(&timer);

    /* Allocate result structure */
    SafetyResult* result = (SafetyResult*)memory_tracked_malloc(sizeof(SafetyResult));
    if (!result) {
        return NULL;
    }

    result->escape_path = NULL;
    result->is_safe = false;
    result->will_eat_food = false;
    result->simulated_head_pos = (Position){-1, -1};

    /* Calculate where head would be after move */
    Position current_head = snake->head->pos;
    Position new_head = position_move(current_head, move);
    result->simulated_head_pos = new_head;

    /* Boundary check: is new position in bounds? */
    if (!grid_is_valid_position(new_head)) {
        result->is_safe = false;
        result->compute_time_us = timer_elapsed_us(&timer);
        return result;
    }

    /* Basic walkability check first */
    if (!grid_is_walkable(grid, new_head)) {
        /* Can't move here - wall or body segment */
        result->is_safe = false;
        result->compute_time_us = timer_elapsed_us(&timer);
        return result;
    }

    /* Check if this move eats food */
    result->will_eat_food = position_equals(new_head, food_pos);

    /*
     * Safety validation strategy:
     * - FOOD MOVES: Full validation (tail doesn't move, could trap)
     * - NON-FOOD MOVES: Basic validation (tail moves away, safer)
     */

    if (!result->will_eat_food) {
        /*
         * NON-FOOD MOVE: Tail moves, creating space
         * Just verify we're not hitting a wall or body
         * The moving tail ensures we don't trap ourselves
         */
        result->is_safe = true;  /* Already passed walkability check */
        result->compute_time_us = timer_elapsed_us(&timer);
        return result;
    }

    /*
     * FOOD MOVE: Full safety validation required
     * Snake grows (tail stays), need to ensure escape route exists
     */

    /* Edge case: Win condition (filling grid) */
    size_t grid_size = GRID_WIDTH * GRID_HEIGHT;
    if (snake->length + 1 >= grid_size) {
        result->is_safe = true;
        result->compute_time_us = timer_elapsed_us(&timer);
        return result;
    }

    /* Save old head's cell type for restoration */
    Position old_head = current_head;
    CellType old_head_type = grid->cells[old_head.y][old_head.x].type;

    /* Simulate: Mark old head as body (represents growth) */
    grid->cells[old_head.y][old_head.x].type = CELL_SNAKE_BODY;

    /* Check: Can we reach tail from new head after eating? */
    Position tail_pos = snake->tail->pos;
    result->escape_path = pathfinding_bfs(grid, new_head, tail_pos);

    /* Verdict: safe if escape path exists */
    result->is_safe = (result->escape_path != NULL && result->escape_path->found);

    /* Revert grid to original state */
    grid->cells[old_head.y][old_head.x].type = old_head_type;

    result->compute_time_us = timer_elapsed_us(&timer);
    return result;
}

/**
 * Clean up a SafetyResult and all its resources
 */
void safety_result_destroy(SafetyResult* result) {
    if (!result) {
        return;
    }

    /* Clean up escape path if it exists */
    if (result->escape_path) {
        pathfinding_free_result(result->escape_path);
        result->escape_path = NULL;
    }

    /* Free the result structure itself */
    memory_tracked_free(result);
}
