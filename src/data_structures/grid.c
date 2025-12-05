#include "grid.h"
#include <string.h>

void grid_init(Grid* grid) {
    if (!grid) {
        return;
    }

    memset(grid->cells, 0, sizeof(grid->cells));

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid->cells[y][x].type = CELL_EMPTY;
            grid->cells[y][x].distance = -1;
            grid->cells[y][x].parent = (Position){-1, -1};
            grid->cells[y][x].visited = false;
        }
    }

    grid->food_pos = (Position){-1, -1};
}

void grid_reset_pathfinding(Grid* grid) {
    if (!grid) {
        return;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid->cells[y][x].distance = -1;
            grid->cells[y][x].parent = (Position){-1, -1};
            grid->cells[y][x].visited = false;
        }
    }
}

CellType grid_get_cell(const Grid* grid, Position pos) {
    if (!grid || !grid_is_valid_position(pos)) {
        return CELL_WALL;  /* Out of bounds = wall */
    }

    return grid->cells[pos.y][pos.x].type;
}

void grid_set_cell(Grid* grid, Position pos, CellType type) {
    if (!grid || !grid_is_valid_position(pos)) {
        return;
    }

    grid->cells[pos.y][pos.x].type = type;
}

bool grid_is_valid_position(Position pos) {
    return pos.x >= 0 && pos.x < GRID_WIDTH &&
           pos.y >= 0 && pos.y < GRID_HEIGHT;
}

bool grid_is_walkable(const Grid* grid, Position pos) {
    if (!grid || !grid_is_valid_position(pos)) {
        return false;
    }

    CellType type = grid->cells[pos.y][pos.x].type;
    return type == CELL_EMPTY || type == CELL_FOOD || type == CELL_SNAKE_TAIL;
}

void grid_get_neighbors(Position pos, Position neighbors[4], int* count) {
    if (!neighbors || !count) {
        return;
    }

    *count = 0;

    /* Directions: Up, Down, Left, Right */
    Direction directions[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

    for (int i = 0; i < 4; i++) {
        Position next = position_move(pos, directions[i]);
        if (grid_is_valid_position(next)) {
            neighbors[*count] = next;
            (*count)++;
        }
    }
}

void grid_clear(Grid* grid) {
    if (!grid) {
        return;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid->cells[y][x].type = CELL_EMPTY;
        }
    }
}
