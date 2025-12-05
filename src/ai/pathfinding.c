#include "pathfinding.h"
#include "../data_structures/queue.h"
#include "../utils/timer.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>

PathResult* pathfinding_bfs(Grid* grid, Position start, Position goal) {
    if (!grid) {
        return NULL;
    }

    /* Allocate result */
    PathResult* result = (PathResult*)memory_tracked_malloc(sizeof(PathResult));
    if (!result) {
        return NULL;
    }

    result->positions = NULL;
    result->length = 0;
    result->found = false;
    result->compute_time_us = 0;

    /* Start timing */
    Timer timer;
    timer_start(&timer);

    /* Reset pathfinding fields in grid */
    grid_reset_pathfinding(grid);

    /* Create queue for BFS */
    Queue* queue = queue_create(QUEUE_CAPACITY);
    if (!queue) {
        memory_tracked_free(result);
        return NULL;
    }

    /* Initialize BFS */
    grid->cells[start.y][start.x].visited = true;
    grid->cells[start.y][start.x].distance = 0;
    grid->cells[start.y][start.x].parent = (Position){-1, -1};
    queue_enqueue(queue, start, 0);

    bool path_found = false;

    /* BFS main loop */
    while (!queue_is_empty(queue)) {
        QueueNode current_node;
        if (queue_dequeue(queue, &current_node) != SUCCESS) {
            break;
        }

        Position current = current_node.pos;

        /* Check if we reached the goal */
        if (position_equals(current, goal)) {
            path_found = true;
            break;
        }

        /* Explore neighbors */
        Position neighbors[4];
        int neighbor_count;
        grid_get_neighbors(current, neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; i++) {
            Position next = neighbors[i];

            /* Check if walkable and not visited */
            if (grid_is_walkable(grid, next) &&
                !grid->cells[next.y][next.x].visited) {

                /* Mark as visited */
                grid->cells[next.y][next.x].visited = true;
                grid->cells[next.y][next.x].distance = current_node.distance + 1;
                grid->cells[next.y][next.x].parent = current;

                /* Enqueue */
                queue_enqueue(queue, next, current_node.distance + 1);
            }
        }
    }

    /* Reconstruct path if found */
    if (path_found) {
        /* Count path length by backtracking */
        size_t path_length = 0;
        Position current = goal;
        while (!position_equals(current, start)) {
            path_length++;
            current = grid->cells[current.y][current.x].parent;
        }
        path_length++; /* Include start position */

        /* Allocate path array */
        result->positions = (Position*)memory_tracked_malloc(
            sizeof(Position) * path_length);

        if (result->positions) {
            /* Fill path array in reverse */
            current = goal;
            for (int i = (int)path_length - 1; i >= 0; i--) {
                result->positions[i] = current;
                if (!position_equals(current, start)) {
                    current = grid->cells[current.y][current.x].parent;
                }
            }

            result->length = path_length;
            result->found = true;
        }
    }

    /* Cleanup */
    queue_destroy(queue);

    /* Stop timing */
    timer_stop(&timer);
    result->compute_time_us = timer_elapsed_us(&timer);

    return result;
}

void pathfinding_free_result(PathResult* result) {
    if (!result) {
        return;
    }

    if (result->positions) {
        memory_tracked_free(result->positions);
    }

    memory_tracked_free(result);
}

Direction pathfinding_get_next_direction(Position current, Position next) {
    if (next.x > current.x) return DIR_RIGHT;
    if (next.x < current.x) return DIR_LEFT;
    if (next.y > current.y) return DIR_DOWN;
    if (next.y < current.y) return DIR_UP;
    return DIR_NONE;
}
