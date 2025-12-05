#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "../../include/common.h"
#include "../data_structures/grid.h"

/**
 * Path result structure
 * Contains the computed path and metadata
 */
typedef struct {
    Position* positions;      /* Array of positions in path */
    size_t length;            /* Number of positions in path */
    bool found;               /* Whether path was found */
    uint64_t compute_time_us; /* Computation time in microseconds */
} PathResult;

/**
 * Perform BFS pathfinding from start to goal
 * @param grid Grid to search (will be modified - pathfinding fields updated)
 * @param start Start position
 * @param goal Goal position
 * @return PathResult with path data (must be freed with pathfinding_free_result)
 */
PathResult* pathfinding_bfs(Grid* grid, Position start, Position goal);

/**
 * Free path result and associated data
 * @param result PathResult to free
 */
void pathfinding_free_result(PathResult* result);

/**
 * Get direction to move from current to next position
 * @param current Current position
 * @param next Next position
 * @return Direction to move
 */
Direction pathfinding_get_next_direction(Position current, Position next);

#endif /* PATHFINDING_H */
