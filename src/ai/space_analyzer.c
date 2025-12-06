#include "space_analyzer.h"
#include "../data_structures/queue.h"
#include "../utils/timer.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>
#include <string.h>

/**
 * Count accessible cells from a position using BFS flood fill
 *
 * Algorithm:
 * 1. Create visited tracking array (20x20 grid)
 * 2. BFS from start position, counting all reachable walkable cells
 * 3. Return count of accessible cells
 *
 * This is used to evaluate move quality: moves that maximize accessible
 * space are preferred, as they keep more escape routes open.
 */
SpaceAnalysis* space_analyze(Grid* grid, Position start) {
    if (!grid) {
        return NULL;
    }

    Timer timer;
    timer_start(&timer);

    /* Allocate result */
    SpaceAnalysis* result = (SpaceAnalysis*)memory_tracked_malloc(sizeof(SpaceAnalysis));
    if (!result) {
        return NULL;
    }

    result->accessible_cells = 0;
    result->is_valid = false;
    result->compute_time_us = 0;

    /* Check if start position is valid and walkable */
    if (!grid_is_valid_position(start) || !grid_is_walkable(grid, start)) {
        result->compute_time_us = timer_elapsed_us(&timer);
        return result;
    }

    /* Create visited array - use calloc for zero initialization */
    bool visited[GRID_HEIGHT][GRID_WIDTH];
    memset(visited, 0, sizeof(visited));

    /* Create queue for BFS */
    Queue* queue = queue_create(QUEUE_CAPACITY);
    if (!queue) {
        memory_tracked_free(result);
        return NULL;
    }

    /* Start BFS from start position */
    queue_enqueue(queue, start, 0);  /* Distance doesn't matter for space counting */
    visited[start.y][start.x] = true;
    size_t count = 1;  /* Start position counts */

    /* BFS flood fill */
    while (!queue_is_empty(queue)) {
        QueueNode current_node;
        if (queue_dequeue(queue, &current_node) != SUCCESS) {
            break;
        }

        Position current = current_node.pos;

        /* Explore all 4 directions */
        Direction dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
        for (int i = 0; i < 4; i++) {
            Position neighbor = position_move(current, dirs[i]);

            /* Skip if out of bounds */
            if (!grid_is_valid_position(neighbor)) {
                continue;
            }

            /* Skip if already visited */
            if (visited[neighbor.y][neighbor.x]) {
                continue;
            }

            /* Skip if not walkable */
            if (!grid_is_walkable(grid, neighbor)) {
                continue;
            }

            /* Mark visited and enqueue */
            visited[neighbor.y][neighbor.x] = true;
            queue_enqueue(queue, neighbor, 0);  /* Distance doesn't matter */
            count++;
        }
    }

    queue_destroy(queue);

    result->accessible_cells = count;
    result->is_valid = true;
    result->compute_time_us = timer_elapsed_us(&timer);

    return result;
}

/**
 * Analyze space after simulating a move
 *
 * This is the key function for Phase 5 space-aware decision making.
 * It simulates what the grid would look like after a move, then counts
 * accessible cells from the new head position.
 *
 * Simulation logic:
 * - Save old head position and type
 * - Mark old head as BODY (snake moved forward)
 * - If move eats food: tail stays (snake grows)
 * - If move doesn't eat food: clear tail cell (snake moves)
 * - Analyze accessible space from new head position
 * - Revert all grid changes
 */
SpaceAnalysis* space_analyze_move(Grid* grid, const Snake* snake, Direction move, Position food_pos) {
    if (!grid || !snake || move == DIR_NONE) {
        return NULL;
    }

    Timer timer;
    timer_start(&timer);

    /* Calculate new head position */
    Position current_head = snake->head->pos;
    Position new_head = position_move(current_head, move);

    /* Check if move is valid */
    if (!grid_is_valid_position(new_head)) {
        SpaceAnalysis* result = (SpaceAnalysis*)memory_tracked_malloc(sizeof(SpaceAnalysis));
        if (result) {
            result->accessible_cells = 0;
            result->is_valid = false;
            result->compute_time_us = timer_elapsed_us(&timer);
        }
        return result;
    }

    /* Check if move eats food */
    bool will_eat_food = position_equals(new_head, food_pos);

    /* Save original grid state for restoration */
    Position old_head = current_head;
    CellType old_head_type = grid->cells[old_head.y][old_head.x].type;

    Position tail_pos = snake->tail->pos;
    CellType old_tail_type = grid->cells[tail_pos.y][tail_pos.x].type;

    /* Simulate the move */
    grid->cells[old_head.y][old_head.x].type = CELL_SNAKE_BODY;

    if (!will_eat_food) {
        /* Non-food move: tail moves forward, clear old tail position */
        grid->cells[tail_pos.y][tail_pos.x].type = CELL_EMPTY;
    }
    /* If eating food: tail stays in place (snake grows) */

    /* Analyze accessible space from new head position */
    SpaceAnalysis* result = space_analyze(grid, new_head);

    /* Revert grid to original state */
    grid->cells[old_head.y][old_head.x].type = old_head_type;
    grid->cells[tail_pos.y][tail_pos.x].type = old_tail_type;

    /* Update timing to include full operation */
    if (result) {
        result->compute_time_us = timer_elapsed_us(&timer);
    }

    return result;
}

void space_analysis_destroy(SpaceAnalysis* analysis) {
    if (!analysis) {
        return;
    }
    memory_tracked_free(analysis);
}
