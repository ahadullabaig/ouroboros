#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "../../include/common.h"
#include "../game/game_state.h"
#include "pathfinding.h"
#include "safety_checker.h"
#include "space_analyzer.h"

/**
 * AI decision structure
 * Contains the AI's chosen direction and decision metadata
 */
typedef struct {
    Direction chosen_direction;
    PathResult* path_to_food;
    SafetyResult* safety_check;     /* Phase 4: Safety validation result */
    SpaceAnalysis* space_analysis;  /* Phase 5: Accessible space analysis */
    uint64_t total_compute_time_us;
    bool used_fallback;             /* If greedy path failed, used space-maximization */
    bool used_space_strategy;       /* Phase 5: Used space-aware decision making */
} AIDecision;

/**
 * Make an AI decision for the current game state
 * Phase 5: Uses space-maximization + safety validation for perfect play
 *
 * Decision algorithm:
 * 1. Find shortest path to food (BFS)
 * 2. For each valid direction:
 *    - Check safety (escape route exists)
 *    - Analyze accessible space after move
 * 3. Choose move that maximizes accessible space (must be >= snake length)
 * 4. Use distance to food as tie-breaker
 *
 * This prevents self-trapping by ensuring adequate maneuvering room.
 *
 * @param state Current game state
 * @return AIDecision (must be freed with ai_decision_destroy)
 */
AIDecision* ai_make_decision(GameState* state);

/**
 * Free AI decision and associated data
 * @param decision AIDecision to free
 */
void ai_decision_destroy(AIDecision* decision);

/**
 * Fallback strategy: maximize accessible space
 * Used when no move provides adequate space or safe path to food
 *
 * Phase 5: Instead of just chasing tail, evaluate all safe moves
 * and choose the one that maximizes accessible space.
 *
 * @param state Current game state
 * @return Direction to move
 */
Direction ai_fallback_strategy(GameState* state);

/**
 * Space-maximization strategy: choose move with most accessible cells
 * Phase 5: Core decision-making strategy
 *
 * Evaluates all four directions and chooses the one that:
 * 1. Is safe (passes safety validation)
 * 2. Maximizes accessible space (prefers open areas)
 * 3. Minimizes distance to food (tie-breaker)
 *
 * @param state Current game state
 * @param space_result Output parameter for space analysis of chosen move
 * @return Direction to move
 */
Direction ai_space_maximization_strategy(GameState* state, SpaceAnalysis** space_result);

#endif /* AI_CONTROLLER_H */
