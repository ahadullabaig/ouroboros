#ifndef GRID_H
#define GRID_H

#include "../../include/common.h"

/**
 * Grid cell structure for BFS pathfinding
 */
typedef struct {
    CellType type;       /* Current cell type (empty, snake, food, etc.) */
    int distance;        /* Distance from start (for BFS) */
    Position parent;     /* Parent position (for path reconstruction) */
    bool visited;        /* Visited flag (for BFS) */
} GridCell;

/**
 * Grid structure representing the game board
 */
typedef struct {
    GridCell cells[GRID_HEIGHT][GRID_WIDTH];
    Position food_pos;
} Grid;

/**
 * Initialize grid with empty cells
 * @param grid Grid to initialize
 */
void grid_init(Grid* grid);

/**
 * Reset pathfinding-related fields (visited, distance, parent)
 * Call before each BFS operation
 * @param grid Grid to reset
 */
void grid_reset_pathfinding(Grid* grid);

/**
 * Get cell type at position
 * @param grid Grid to query
 * @param pos Position to check
 * @return Cell type at position
 */
CellType grid_get_cell(const Grid* grid, Position pos);

/**
 * Set cell type at position
 * @param grid Grid to modify
 * @param pos Position to set
 * @param type Cell type to set
 */
void grid_set_cell(Grid* grid, Position pos, CellType type);

/**
 * Check if position is within grid bounds
 * @param pos Position to check
 * @return true if valid, false otherwise
 */
bool grid_is_valid_position(Position pos);

/**
 * Check if position is walkable (for pathfinding)
 * @param grid Grid to check
 * @param pos Position to check
 * @return true if walkable (empty or food), false otherwise
 */
bool grid_is_walkable(const Grid* grid, Position pos);

/**
 * Get all valid neighbors of a position
 * @param pos Position to get neighbors for
 * @param neighbors Output array for neighbors (size 4)
 * @param count Output parameter for number of valid neighbors
 */
void grid_get_neighbors(Position pos, Position neighbors[4], int* count);

/**
 * Clear entire grid to empty cells
 * @param grid Grid to clear
 */
void grid_clear(Grid* grid);

#endif /* GRID_H */
