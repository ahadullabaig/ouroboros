#include "ai_controller.h"
#include "pathfinding.h"
#include "safety_checker.h"
#include "space_analyzer.h"
#include "../utils/timer.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>
#include <limits.h>

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
    decision->safety_check = NULL;
    decision->space_analysis = NULL;
    decision->total_compute_time_us = 0;
    decision->used_fallback = false;
    decision->used_space_strategy = false;

    /* Start timing */
    Timer total_timer;
    timer_start(&total_timer);

    /* Sync grid with current snake position */
    game_state_sync_grid(state);

    Position head_pos = state->snake->head->pos;
    Position food_pos = state->grid.food_pos;

    /* Find path to food using BFS (for reference and tie-breaking) */
    decision->path_to_food = pathfinding_bfs(&state->grid, head_pos, food_pos);

    /*
     * Phase 5: Space-Maximization Strategy
     *
     * Instead of blindly following the shortest path to food,
     * evaluate all moves and choose the one that:
     * 1. Maximizes accessible space (prevents self-trapping)
     * 2. Minimizes distance to food (tie-breaker)
     *
     * This enables near-perfect play by maintaining maneuvering room.
     */
    decision->chosen_direction = ai_space_maximization_strategy(state, &decision->space_analysis);
    decision->used_space_strategy = true;

    /* If space strategy failed, use fallback */
    if (decision->chosen_direction == DIR_NONE) {
        decision->chosen_direction = ai_fallback_strategy(state);
        decision->used_fallback = true;
        decision->used_space_strategy = false;
    }

    /* Get safety check for the chosen direction (for display purposes) */
    if (decision->chosen_direction != DIR_NONE) {
        decision->safety_check = safety_check_move(&state->grid, state->snake,
                                                   decision->chosen_direction, food_pos);
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

    /* Phase 4: Clean up safety check result */
    if (decision->safety_check) {
        safety_result_destroy(decision->safety_check);
    }

    /* Phase 5: Clean up space analysis result */
    if (decision->space_analysis) {
        space_analysis_destroy(decision->space_analysis);
    }

    memory_tracked_free(decision);
}

Direction ai_space_maximization_strategy(GameState* state, SpaceAnalysis** space_result) {
    if (!state || !state->snake || !space_result) {
        return DIR_NONE;
    }

    Position head_pos = state->snake->head->pos;
    Position food_pos = state->grid.food_pos;

    /* Evaluate all four directions */
    Direction dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    Direction best_dir = DIR_NONE;
    size_t best_space = 0;
    int best_distance_to_food = INT_MAX;

    /* Minimum required space: at least snake length to ensure maneuvering room */
    size_t min_required_space = state->snake->length;

    for (int i = 0; i < 4; i++) {
        Direction dir = dirs[i];
        Position new_head = position_move(head_pos, dir);

        /* Skip invalid positions */
        if (!grid_is_valid_position(new_head)) {
            continue;
        }

        /* Skip non-walkable cells */
        if (!grid_is_walkable(&state->grid, new_head)) {
            continue;
        }

        /* Check safety first (Phase 4) */
        SafetyResult* safety = safety_check_move(&state->grid, state->snake, dir, food_pos);
        bool is_safe = (safety && safety->is_safe);
        safety_result_destroy(safety);

        if (!is_safe) {
            continue;  /* Skip unsafe moves */
        }

        /* Analyze accessible space after this move (Phase 5) */
        SpaceAnalysis* space = space_analyze_move(&state->grid, state->snake, dir, food_pos);

        if (!space || !space->is_valid) {
            space_analysis_destroy(space);
            continue;
        }

        /* Calculate distance to food from new position (for tie-breaking) */
        int distance_to_food = abs(new_head.x - food_pos.x) + abs(new_head.y - food_pos.y);

        /*
         * Selection criteria (in order of priority):
         * 1. Must have adequate space (>= snake length)
         * 2. Prefer maximum accessible space
         * 3. Prefer minimum distance to food (tie-breaker)
         */
        bool is_better = false;

        if (space->accessible_cells >= min_required_space) {
            if (best_dir == DIR_NONE) {
                /* First valid move found */
                is_better = true;
            } else if (space->accessible_cells > best_space) {
                /* More space is better */
                is_better = true;
            } else if (space->accessible_cells == best_space && distance_to_food < best_distance_to_food) {
                /* Same space, closer to food is better */
                is_better = true;
            }
        }

        if (is_better) {
            best_dir = dir;
            best_space = space->accessible_cells;
            best_distance_to_food = distance_to_food;

            /* Store space analysis for the best move */
            if (*space_result) {
                space_analysis_destroy(*space_result);
            }
            *space_result = space;
        } else {
            space_analysis_destroy(space);
        }
    }

    return best_dir;
}

Direction ai_fallback_strategy(GameState* state) {
    if (!state || !state->snake || !state->snake->head) {
        return DIR_RIGHT;  /* Default direction */
    }

    /*
     * Phase 5 Fallback: Space-maximization even when no good path exists
     *
     * Instead of just chasing tail, evaluate all safe moves and choose
     * the one with maximum accessible space, ignoring food proximity.
     */

    Position head_pos = state->snake->head->pos;
    Position food_pos = state->grid.food_pos;

    Direction dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    Direction best_dir = state->snake->current_direction;  /* Default: keep moving */
    size_t best_space = 0;

    for (int i = 0; i < 4; i++) {
        Direction dir = dirs[i];
        Position new_head = position_move(head_pos, dir);

        /* Skip invalid positions */
        if (!grid_is_valid_position(new_head)) {
            continue;
        }

        /* Skip non-walkable cells */
        if (!grid_is_walkable(&state->grid, new_head)) {
            continue;
        }

        /* Analyze space (don't check safety - desperate situation) */
        SpaceAnalysis* space = space_analyze_move(&state->grid, state->snake, dir, food_pos);

        if (space && space->is_valid) {
            if (space->accessible_cells > best_space) {
                best_dir = dir;
                best_space = space->accessible_cells;
            }
            space_analysis_destroy(space);
        }
    }

    return best_dir;
}
