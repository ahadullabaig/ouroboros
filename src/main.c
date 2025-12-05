#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>
#include "../include/common.h"
#include "game/game_state.h"
#include "ai/ai_controller.h"
#include "rendering/renderer.h"
#include "utils/timer.h"
#include "utils/memory_tracker.h"

/* Global flag for signal handling */
volatile sig_atomic_t g_running = 1;

void signal_handler(int signum) {
    (void)signum;
    g_running = 0;
}

Direction get_keyboard_input(void) {
    int ch = getch();

    switch (ch) {
        case KEY_UP:
        case 'w':
        case 'W':
            return DIR_UP;

        case KEY_DOWN:
        case 's':
        case 'S':
            return DIR_DOWN;

        case KEY_LEFT:
        case 'a':
        case 'A':
            return DIR_LEFT;

        case KEY_RIGHT:
        case 'd':
        case 'D':
            return DIR_RIGHT;

        case 'q':
        case 'Q':
            g_running = 0;
            return DIR_NONE;

        case 'r':
        case 'R':
            /* Restart handled in main loop */
            return DIR_NONE;

        default:
            return DIR_NONE;
    }
}

void print_usage(const char* program_name) {
    fprintf(stderr, "Ouroboros: Autonomous Snake AI\n");
    fprintf(stderr, "Usage: %s [options]\n", program_name);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -m, --manual    Manual play mode (default)\n");
    fprintf(stderr, "  -a, --ai        AI demonstration mode\n");
    fprintf(stderr, "  -h, --help      Show this help message\n");
    fprintf(stderr, "\nControls (Manual Mode):\n");
    fprintf(stderr, "  Arrow Keys / WASD  -  Move snake\n");
    fprintf(stderr, "  R  -  Restart game\n");
    fprintf(stderr, "  Q  -  Quit\n");
}

int main(int argc, char* argv[]) {
    /* Parse command line arguments */
    GameMode mode = MODE_MANUAL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ai") == 0) {
            mode = MODE_AI_DEMO;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--manual") == 0) {
            mode = MODE_MANUAL;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    /* Initialize memory tracker */
    memory_tracker_init();

    /* Create game state */
    GameState* state = game_state_create(mode);
    if (!state) {
        fprintf(stderr, "Failed to create game state\n");
        return 1;
    }

    /* Create renderer */
    Renderer* renderer = renderer_create();
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer\n");
        game_state_destroy(state);
        return 1;
    }

    /* Setup signal handling */
    signal(SIGINT, signal_handler);

    /* Main game loop */
    Timer frame_timer;
    Direction last_input = DIR_NONE;
    AIDecision* ai_decision = NULL;

    while (g_running) {
        timer_start(&frame_timer);

        /* Handle input */
        Direction input_dir = DIR_NONE;

        if (mode == MODE_MANUAL) {
            input_dir = get_keyboard_input();

            /* Handle restart */
            if (getch() == 'r' || getch() == 'R') {
                game_state_reset(state);
                continue;
            }
        } else {
            /* AI mode */
            if (state->status == GAME_RUNNING) {
                ai_decision = ai_make_decision(state);
                if (ai_decision) {
                    input_dir = ai_decision->chosen_direction;
                }
            }

            /* Allow user to quit or restart */
            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                g_running = 0;
            }
            if (ch == 'r' || ch == 'R') {
                if (ai_decision) {
                    ai_decision_destroy(ai_decision);
                    ai_decision = NULL;
                }
                game_state_reset(state);
                continue;
            }
        }

        /* Update game state */
        if (state->status == GAME_RUNNING) {
            /* Keep moving in last direction if no input */
            if (input_dir == DIR_NONE && mode == MODE_MANUAL) {
                input_dir = last_input;
            }

            if (input_dir != DIR_NONE) {
                last_input = input_dir;
            }

            game_state_update(state, input_dir);
        }

        /* Render */
        renderer_draw_game(renderer, state, ai_decision);
        renderer_draw_stats(renderer, state, ai_decision);

        if (state->status != GAME_RUNNING) {
            renderer_draw_game_over(renderer, state);
        }

        renderer_refresh(renderer);

        /* Cleanup AI decision */
        if (ai_decision) {
            ai_decision_destroy(ai_decision);
            ai_decision = NULL;
        }

        /* Frame rate limiting */
        timer_stop(&frame_timer);
        uint64_t elapsed_ms = timer_elapsed_ms(&frame_timer);

        if (elapsed_ms < FRAME_DELAY_MS) {
            struct timespec sleep_time = {
                .tv_sec = 0,
                .tv_nsec = (FRAME_DELAY_MS - elapsed_ms) * 1000000
            };
            nanosleep(&sleep_time, NULL);
        }
    }

    /* Cleanup */
    renderer_destroy(renderer);
    game_state_destroy(state);

    printf("\nGame Over!\n");
    printf("Final Score: %u\n", state ? state->snake->score : 0);
    printf("Total Moves: %u\n", state ? state->moves_count : 0);

    return 0;
}
