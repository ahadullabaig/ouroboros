#include "ui_components.h"
#include <string.h>
#include <stdio.h>

void ui_draw_title(WINDOW* win, int y, const char* title) {
    if (!win || !title) {
        return;
    }

    int width = getmaxx(win);
    int title_len = (int)strlen(title);
    int x = (width - title_len) / 2;

    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_UI_TEXT));
    mvwprintw(win, y, x, "%s", title);
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_UI_TEXT));
}

void ui_draw_border(WINDOW* win) {
    if (!win) {
        return;
    }

    wattron(win, COLOR_PAIR(COLOR_PAIR_UI_BORDER));
    box(win, 0, 0);
    wattroff(win, COLOR_PAIR(COLOR_PAIR_UI_BORDER));
}

void ui_draw_stat_line(WINDOW* win, int y, const char* label, const char* value) {
    if (!win || !label || !value) {
        return;
    }

    wattron(win, A_BOLD);
    mvwprintw(win, y, 2, "%s:", label);
    wattroff(win, A_BOLD);

    mvwprintw(win, y, 20, "%s", value);
}

void ui_format_time(uint64_t microseconds, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return;
    }

    if (microseconds < 1000) {
        snprintf(buffer, buffer_size, "%lu us", (unsigned long)microseconds);
    } else if (microseconds < 1000000) {
        double ms = microseconds / 1000.0;
        snprintf(buffer, buffer_size, "%.2f ms", ms);
    } else {
        double s = microseconds / 1000000.0;
        snprintf(buffer, buffer_size, "%.2f s", s);
    }
}

void ui_format_memory(uint64_t bytes, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return;
    }

    if (bytes < 1024) {
        snprintf(buffer, buffer_size, "%lu B", (unsigned long)bytes);
    } else if (bytes < 1024 * 1024) {
        double kb = bytes / 1024.0;
        snprintf(buffer, buffer_size, "%.2f KB", kb);
    } else {
        double mb = bytes / (1024.0 * 1024.0);
        snprintf(buffer, buffer_size, "%.2f MB", mb);
    }
}

void ui_draw_separator(WINDOW* win, int y) {
    if (!win) {
        return;
    }

    int width = getmaxx(win);
    wattron(win, COLOR_PAIR(COLOR_PAIR_UI_BORDER));
    mvwhline(win, y, 1, ACS_HLINE, width - 2);
    wattroff(win, COLOR_PAIR(COLOR_PAIR_UI_BORDER));
}
