#ifndef SAFETY_CHECKER_H
#define SAFETY_CHECKER_H

#include "../../include/common.h"
#include "../data_structures/grid.h"
#include "../data_structures/snake.h"
#include "pathfinding.h"

/**
 * Safety Checker Module
 *
 * Validates moves by simulating their consequences and checking for escape routes.
 * This is the key to achieving "perfect games" - preventing self-trapping scenarios.
 *
 * Algorithm:
 * 1. Simulate the proposed move (without actually executing it)
 * 2. If move eats food: check if escape path exists from new position to tail
 * 3. If move doesn't eat food: just validate it's a legal move
 * 4. Return safety verdict with escape path details
 *
 * Performance target: <5ms per check (combined with BFS, total AI time <10ms)
 */

/**
 * Result of a safety check operation
 * Contains verdict and supporting data for rendering/debugging
 */
typedef struct {
    bool is_safe;                   /* Main result: is this move safe? */
    PathResult* escape_path;        /* Path from head to tail after move (NULL if not eating food) */
    uint64_t compute_time_us;       /* Time spent on safety check (microseconds) */
    Position simulated_head_pos;    /* Where head would be after move */
    bool will_eat_food;             /* Does this move eat food? */
} SafetyResult;

/**
 * Check if a proposed move is safe
 *
 * Simulates the move without modifying the game state, then validates that
 * an escape route exists if food is eaten.
 *
 * @param grid Current grid state (will not be modified permanently)
 * @param snake Current snake state
 * @param move Direction to move
 * @param food_pos Position of food
 * @return SafetyResult* Result of safety check (caller must free with safety_result_destroy)
 */
SafetyResult* safety_check_move(Grid* grid, const Snake* snake, Direction move, Position food_pos);

/**
 * Clean up a SafetyResult and all its resources
 *
 * @param result SafetyResult to destroy (can be NULL)
 */
void safety_result_destroy(SafetyResult* result);

#endif /* SAFETY_CHECKER_H */
