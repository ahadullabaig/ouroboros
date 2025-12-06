#ifndef SPACE_ANALYZER_H
#define SPACE_ANALYZER_H

#include "../../include/common.h"
#include "../data_structures/grid.h"
#include "../data_structures/snake.h"

/**
 * Space analysis result
 * Contains information about accessible space from a given position
 */
typedef struct {
    size_t accessible_cells;    /* Number of cells reachable from position */
    uint64_t compute_time_us;   /* Time spent analyzing space */
    bool is_valid;              /* Whether analysis completed successfully */
} SpaceAnalysis;

/**
 * Analyze accessible space from a given position using flood fill
 *
 * Uses BFS to count all cells reachable from start position.
 * Critical for avoiding self-trapping: accessible space must exceed snake length.
 *
 * @param grid Current grid state
 * @param start Starting position for analysis
 * @return SpaceAnalysis result (must be freed with space_analysis_destroy)
 */
SpaceAnalysis* space_analyze(Grid* grid, Position start);

/**
 * Analyze accessible space after simulating a move
 *
 * Temporarily updates grid to reflect the move, analyzes space, then reverts.
 * Used for evaluating move quality without modifying game state.
 *
 * @param grid Current grid state
 * @param snake Current snake
 * @param move Direction to simulate
 * @param food_pos Current food position
 * @return SpaceAnalysis result (must be freed with space_analysis_destroy)
 */
SpaceAnalysis* space_analyze_move(Grid* grid, const Snake* snake, Direction move, Position food_pos);

/**
 * Free space analysis result
 * @param analysis SpaceAnalysis to free
 */
void space_analysis_destroy(SpaceAnalysis* analysis);

#endif /* SPACE_ANALYZER_H */
