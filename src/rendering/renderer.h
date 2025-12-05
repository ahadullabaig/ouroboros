#ifndef RENDERER_H
#define RENDERER_H

#include <ncurses.h>
#include "../../include/common.h"
#include "../game/game_state.h"
#include "../ai/ai_controller.h"

/**
 * Renderer structure for ncurses-based visualization
 */
typedef struct {
    WINDOW* game_window;
    WINDOW* stats_window;
    int terminal_height;
    int terminal_width;
    int game_win_height;
    int game_win_width;
    int stats_win_width;
} Renderer;

/**
 * Create and initialize renderer
 * @return Pointer to new renderer or NULL on failure
 */
Renderer* renderer_create(void);

/**
 * Destroy renderer and cleanup ncurses
 * @param renderer Renderer to destroy
 */
void renderer_destroy(Renderer* renderer);

/**
 * Draw the game grid and snake
 * @param renderer Renderer to use
 * @param state Current game state
 * @param ai_decision AI decision data (for visualization, can be NULL)
 */
void renderer_draw_game(Renderer* renderer, const GameState* state, const AIDecision* ai_decision);

/**
 * Draw statistics panel
 * @param renderer Renderer to use
 * @param state Current game state
 * @param ai_decision AI decision data (can be NULL)
 */
void renderer_draw_stats(Renderer* renderer, const GameState* state, const AIDecision* ai_decision);

/**
 * Draw game over screen
 * @param renderer Renderer to use
 * @param state Game state
 */
void renderer_draw_game_over(Renderer* renderer, const GameState* state);

/**
 * Refresh all windows
 * @param renderer Renderer to refresh
 */
void renderer_refresh(Renderer* renderer);

/**
 * Get character representation for a cell type
 * @param type Cell type
 * @return Character to display
 */
char renderer_get_cell_char(CellType type);

#endif /* RENDERER_H */
