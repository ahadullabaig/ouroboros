#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <ncurses.h>
#include "../../include/common.h"

/**
 * UI component utilities for rendering statistics and information
 */

/**
 * Draw a title with decorative border
 * @param win Window to draw in
 * @param y Y position
 * @param title Title text
 */
void ui_draw_title(WINDOW* win, int y, const char* title);

/**
 * Draw a decorative border around window
 * @param win Window to draw border in
 */
void ui_draw_border(WINDOW* win);

/**
 * Draw a stat line with label and value
 * @param win Window to draw in
 * @param y Y position
 * @param label Label text
 * @param value Value text
 */
void ui_draw_stat_line(WINDOW* win, int y, const char* label, const char* value);

/**
 * Format time in microseconds to human-readable string
 * @param microseconds Time in microseconds
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 */
void ui_format_time(uint64_t microseconds, char* buffer, size_t buffer_size);

/**
 * Format memory in bytes to human-readable string
 * @param bytes Memory in bytes
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 */
void ui_format_memory(uint64_t bytes, char* buffer, size_t buffer_size);

/**
 * Draw a horizontal separator line
 * @param win Window to draw in
 * @param y Y position
 */
void ui_draw_separator(WINDOW* win, int y);

#endif /* UI_COMPONENTS_H */
