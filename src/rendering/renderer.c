#include "renderer.h"
#include "ui_components.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>
#include <string.h>

/* Window dimensions */
#define GAME_WIN_HEIGHT (GRID_HEIGHT + 2)  /* +2 for borders */
#define GAME_WIN_WIDTH (GRID_WIDTH * 2 + 2)  /* *2 for spacing, +2 for borders */
#define STATS_WIN_WIDTH 40
#define MIN_TERMINAL_HEIGHT 25
#define MIN_TERMINAL_WIDTH (GAME_WIN_WIDTH + STATS_WIN_WIDTH + 1)

static void renderer_init_colors(void) {
    start_color();
    use_default_colors();

    /* Initialize color pairs */
    init_pair(COLOR_PAIR_EMPTY, COLOR_BLACK, -1);
    init_pair(COLOR_PAIR_SNAKE_HEAD, COLOR_CYAN, -1);
    init_pair(COLOR_PAIR_SNAKE_BODY, COLOR_GREEN, -1);
    init_pair(COLOR_PAIR_SNAKE_TAIL, COLOR_GREEN, -1);
    init_pair(COLOR_PAIR_FOOD, COLOR_RED, -1);
    init_pair(COLOR_PAIR_WALL, COLOR_WHITE, -1);
    init_pair(COLOR_PAIR_PATH_OVERLAY, COLOR_YELLOW, -1);
    init_pair(COLOR_PAIR_SAFETY_CHECK, COLOR_BLUE, -1);
    init_pair(COLOR_PAIR_UI_BORDER, COLOR_WHITE, -1);
    init_pair(COLOR_PAIR_UI_TEXT, COLOR_WHITE, -1);
}

Renderer* renderer_create(void) {
    /* Initialize ncurses */
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    timeout(0);

    Renderer* renderer = (Renderer*)memory_tracked_malloc(sizeof(Renderer));
    if (!renderer) {
        endwin();
        return NULL;
    }

    /* Get terminal size */
    getmaxyx(stdscr, renderer->terminal_height, renderer->terminal_width);

    /* Check minimum terminal size */
    if (renderer->terminal_height < MIN_TERMINAL_HEIGHT ||
        renderer->terminal_width < MIN_TERMINAL_WIDTH) {
        endwin();
        fprintf(stderr, "Terminal too small! Need at least %dx%d\n",
                MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT);
        memory_tracked_free(renderer);
        return NULL;
    }

    /* Initialize colors */
    renderer_init_colors();

    /* Create game window (left side) */
    renderer->game_win_height = GAME_WIN_HEIGHT;
    renderer->game_win_width = GAME_WIN_WIDTH;
    renderer->game_window = newwin(renderer->game_win_height,
                                   renderer->game_win_width,
                                   1, 1);

    if (!renderer->game_window) {
        endwin();
        memory_tracked_free(renderer);
        return NULL;
    }

    /* Create stats window (right side) */
    renderer->stats_win_width = STATS_WIN_WIDTH;
    renderer->stats_window = newwin(renderer->game_win_height,
                                    renderer->stats_win_width,
                                    1, GAME_WIN_WIDTH + 2);

    if (!renderer->stats_window) {
        delwin(renderer->game_window);
        endwin();
        memory_tracked_free(renderer);
        return NULL;
    }

    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (!renderer) {
        return;
    }

    if (renderer->game_window) {
        delwin(renderer->game_window);
    }

    if (renderer->stats_window) {
        delwin(renderer->stats_window);
    }

    endwin();
    memory_tracked_free(renderer);
}

char renderer_get_cell_char(CellType type) {
    switch (type) {
        case CELL_EMPTY:       return ' ';
        case CELL_SNAKE_HEAD:  return 'O';
        case CELL_SNAKE_BODY:  return 'o';
        case CELL_SNAKE_TAIL:  return '.';
        case CELL_FOOD:        return '@';
        case CELL_WALL:        return '#';
        default:               return '?';
    }
}

void renderer_draw_game(Renderer* renderer, const GameState* state, const AIDecision* ai_decision) {
    if (!renderer || !state) {
        return;
    }

    (void)ai_decision;  /* Unused in Phase 2 */

    WINDOW* win = renderer->game_window;
    werase(win);

    /* Draw border */
    ui_draw_border(win);

    /* Draw grid */
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Position pos = {x, y};
            CellType cell = grid_get_cell(&state->grid, pos);
            char ch = renderer_get_cell_char(cell);

            int color_pair = COLOR_PAIR_EMPTY;
            int attrs = 0;

            switch (cell) {
                case CELL_SNAKE_HEAD:
                    color_pair = COLOR_PAIR_SNAKE_HEAD;
                    attrs = A_BOLD;
                    break;
                case CELL_SNAKE_BODY:
                    color_pair = COLOR_PAIR_SNAKE_BODY;
                    break;
                case CELL_SNAKE_TAIL:
                    color_pair = COLOR_PAIR_SNAKE_TAIL;
                    attrs = A_DIM;
                    break;
                case CELL_FOOD:
                    color_pair = COLOR_PAIR_FOOD;
                    attrs = A_BOLD | A_BLINK;
                    break;
                default:
                    break;
            }

            wattron(win, COLOR_PAIR(color_pair) | attrs);
            mvwaddch(win, y + 1, x * 2 + 1, ch);
            mvwaddch(win, y + 1, x * 2 + 2, ch);
            wattroff(win, COLOR_PAIR(color_pair) | attrs);
        }
    }
}

void renderer_draw_stats(Renderer* renderer, const GameState* state, const AIDecision* ai_decision) {
    if (!renderer || !state) {
        return;
    }

    WINDOW* win = renderer->stats_window;
    werase(win);

    /* Draw border */
    ui_draw_border(win);

    /* Title */
    ui_draw_title(win, 1, "OUROBOROS");

    int line = 3;

    /* Game mode */
    const char* mode_str = (state->mode == MODE_MANUAL) ? "Manual" : "AI Demo";
    ui_draw_stat_line(win, line++, "Mode", mode_str);
    line++;

    /* Score and moves */
    char score_buf[32];
    snprintf(score_buf, sizeof(score_buf), "%u", state->snake ? state->snake->score : 0);
    ui_draw_stat_line(win, line++, "Score", score_buf);

    char moves_buf[32];
    snprintf(moves_buf, sizeof(moves_buf), "%u", state->moves_count);
    ui_draw_stat_line(win, line++, "Moves", moves_buf);

    char length_buf[32];
    snprintf(length_buf, sizeof(length_buf), "%zu", state->snake ? state->snake->length : 0);
    ui_draw_stat_line(win, line++, "Length", length_buf);

    line++;
    ui_draw_separator(win, line++);
    line++;

    /* AI Statistics (Phase 5) */
    if (state->mode == MODE_AI_DEMO && ai_decision) {
        wattron(win, A_BOLD);
        mvwprintw(win, line++, 2, "AI Statistics:");
        wattroff(win, A_BOLD);

        /* Phase 5: Space Analysis */
        if (ai_decision->space_analysis && ai_decision->space_analysis->is_valid) {
            char space_buf[32];
            snprintf(space_buf, sizeof(space_buf), "%zu cells",
                     ai_decision->space_analysis->accessible_cells);
            ui_draw_stat_line(win, line++, "Access Space", space_buf);

            /* Space as percentage of grid */
            size_t grid_size = GRID_WIDTH * GRID_HEIGHT;
            float space_percent = (float)ai_decision->space_analysis->accessible_cells / grid_size * 100.0f;
            char space_pct_buf[32];
            snprintf(space_pct_buf, sizeof(space_pct_buf), "%.1f%%", space_percent);
            ui_draw_stat_line(win, line++, "Space %", space_pct_buf);
        }

        /* BFS compute time */
        if (ai_decision->path_to_food) {
            char bfs_time_buf[32];
            snprintf(bfs_time_buf, sizeof(bfs_time_buf), "%.2f ms",
                     ai_decision->path_to_food->compute_time_us / 1000.0);
            ui_draw_stat_line(win, line++, "BFS Time", bfs_time_buf);
        }

        /* Safety check status */
        if (ai_decision->safety_check) {
            const char* safety_status = ai_decision->safety_check->is_safe ? "SAFE" : "UNSAFE";
            int color_pair = ai_decision->safety_check->is_safe ? COLOR_PAIR_SNAKE_BODY : COLOR_PAIR_FOOD;

            mvwprintw(win, line, 2, "Safety: ");
            wattron(win, COLOR_PAIR(color_pair) | A_BOLD);
            wprintw(win, "%s", safety_status);
            wattroff(win, COLOR_PAIR(color_pair) | A_BOLD);
            line++;

            /* Safety check compute time */
            char safety_time_buf[32];
            snprintf(safety_time_buf, sizeof(safety_time_buf), "%.2f ms",
                     ai_decision->safety_check->compute_time_us / 1000.0);
            ui_draw_stat_line(win, line++, "Safety Time", safety_time_buf);

            /* Escape path length */
            if (ai_decision->safety_check->escape_path &&
                ai_decision->safety_check->escape_path->found) {
                char escape_buf[32];
                snprintf(escape_buf, sizeof(escape_buf), "%zu steps",
                         ai_decision->safety_check->escape_path->length);
                ui_draw_stat_line(win, line++, "Escape Path", escape_buf);
            }
        } else {
            ui_draw_stat_line(win, line++, "Safety", "N/A");
        }

        /* Total AI compute time */
        char total_time_buf[32];
        snprintf(total_time_buf, sizeof(total_time_buf), "%.2f ms",
                 ai_decision->total_compute_time_us / 1000.0);
        ui_draw_stat_line(win, line++, "Total Time", total_time_buf);

        /* Strategy indicators */
        if (ai_decision->used_space_strategy && !ai_decision->used_fallback) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_SNAKE_BODY) | A_BOLD);
            mvwprintw(win, line++, 2, "  [Space Strategy]");
            wattroff(win, COLOR_PAIR(COLOR_PAIR_SNAKE_BODY) | A_BOLD);
        } else if (ai_decision->used_fallback) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_PATH_OVERLAY) | A_BOLD);
            mvwprintw(win, line++, 2, "  [Fallback Mode]");
            wattroff(win, COLOR_PAIR(COLOR_PAIR_PATH_OVERLAY) | A_BOLD);
        }

        line++;
        ui_draw_separator(win, line++);
        line++;
    }

    /* Memory stats */
    char mem_buf[32];
    ui_format_memory(memory_get_current_usage(), mem_buf, sizeof(mem_buf));
    ui_draw_stat_line(win, line++, "Memory", mem_buf);

    ui_format_memory(state->peak_memory_bytes, mem_buf, sizeof(mem_buf));
    ui_draw_stat_line(win, line++, "Peak Memory", mem_buf);

    line++;
    ui_draw_separator(win, line++);
    line++;

    /* Controls */
    wattron(win, A_BOLD);
    mvwprintw(win, line++, 2, "Controls:");
    wattroff(win, A_BOLD);
    mvwprintw(win, line++, 2, "Arrow Keys - Move");
    mvwprintw(win, line++, 2, "Q - Quit");
    mvwprintw(win, line++, 2, "R - Restart");
}

void renderer_draw_game_over(Renderer* renderer, const GameState* state) {
    if (!renderer || !state) {
        return;
    }

    WINDOW* win = renderer->game_window;

    /* Draw game over message in center */
    const char* msg1;
    const char* msg2 = "Press R to restart or Q to quit";

    if (state->status == GAME_WON) {
        msg1 = "PERFECT GAME!";
    } else {
        msg1 = "GAME OVER";
    }

    int msg1_len = (int)strlen(msg1);
    int msg2_len = (int)strlen(msg2);
    int center_y = GRID_HEIGHT / 2;
    int center_x = GAME_WIN_WIDTH / 2;

    wattron(win, A_BOLD | A_BLINK);
    mvwprintw(win, center_y, center_x - msg1_len / 2, "%s", msg1);
    wattroff(win, A_BOLD | A_BLINK);

    mvwprintw(win, center_y + 2, center_x - msg2_len / 2, "%s", msg2);
}

void renderer_refresh(Renderer* renderer) {
    if (!renderer) {
        return;
    }

    wnoutrefresh(renderer->game_window);
    wnoutrefresh(renderer->stats_window);
    doupdate();
}
