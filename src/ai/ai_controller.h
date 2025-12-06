#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "../../include/common.h"
#include "../game/game_state.h"
#include "pathfinding.h"
#include "safety_checker.h"

/**
 * AI decision structure
 * Contains the AI's chosen direction and decision metadata
 */
typedef struct {
    Direction chosen_direction;
    PathResult* path_to_food;
    SafetyResult* safety_check;  /* Phase 4: Safety validation result */
    uint64_t total_compute_time_us;
    bool used_fallback;          /* If greedy path failed, used tail-chasing */
} AIDecision;

/**
 * Make an AI decision for the current game state
 * Phase 4: Uses BFS + safety validation for perfect play
 * Only follows paths that guarantee escape routes exist
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
 * Fallback strategy: chase own tail
 * Used when no safe path to food exists
 * @param state Current game state
 * @return Direction to move
 */
Direction ai_fallback_strategy(GameState* state);

#endif /* AI_CONTROLLER_H */
