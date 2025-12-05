#include "snake.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>

/* Helper function to create a new segment */
static SnakeSegment* segment_create(Position pos) {
    SnakeSegment* segment = (SnakeSegment*)memory_tracked_malloc(sizeof(SnakeSegment));
    if (!segment) {
        return NULL;
    }

    segment->pos = pos;
    segment->next = NULL;
    segment->prev = NULL;

    return segment;
}

Snake* snake_create(Position initial_pos, Direction initial_dir) {
    Snake* snake = (Snake*)memory_tracked_malloc(sizeof(Snake));
    if (!snake) {
        return NULL;
    }

    snake->current_direction = initial_dir;
    snake->next_direction = initial_dir;
    snake->length = INITIAL_SNAKE_LENGTH;
    snake->score = 0;

    /* Create initial segments */
    Position offset = direction_to_offset(initial_dir);

    /* Create head */
    snake->head = segment_create(initial_pos);
    if (!snake->head) {
        memory_tracked_free(snake);
        return NULL;
    }

    SnakeSegment* current = snake->head;

    /* Create body segments (growing backwards from head) */
    for (size_t i = 1; i < INITIAL_SNAKE_LENGTH; i++) {
        Position seg_pos = {
            initial_pos.x - (offset.x * (int)i),
            initial_pos.y - (offset.y * (int)i)
        };

        SnakeSegment* new_segment = segment_create(seg_pos);
        if (!new_segment) {
            snake_destroy(snake);
            return NULL;
        }

        /* Link segments */
        current->next = new_segment;
        new_segment->prev = current;
        current = new_segment;
    }

    snake->tail = current;

    return snake;
}

void snake_destroy(Snake* snake) {
    if (!snake) {
        return;
    }

    /* Free all segments */
    SnakeSegment* current = snake->head;
    while (current) {
        SnakeSegment* next = current->next;
        memory_tracked_free(current);
        current = next;
    }

    memory_tracked_free(snake);
}

ResultCode snake_move(Snake* snake, bool grow) {
    if (!snake) {
        return ERROR_NULL_POINTER;
    }

    /* Update current direction from next direction */
    snake->current_direction = snake->next_direction;

    /* Calculate new head position */
    Position new_head_pos = snake_get_next_head_position(snake);

    /* Create new head segment */
    SnakeSegment* new_head = segment_create(new_head_pos);
    if (!new_head) {
        return ERROR_OUT_OF_MEMORY;
    }

    /* Link new head to old head */
    new_head->next = snake->head;
    snake->head->prev = new_head;
    snake->head = new_head;

    if (grow) {
        /* Growing: keep tail, increment length and score */
        snake->length++;
        snake->score++;
    } else {
        /* Not growing: remove tail */
        if (snake->tail && snake->tail->prev) {
            SnakeSegment* old_tail = snake->tail;
            snake->tail = old_tail->prev;
            snake->tail->next = NULL;
            memory_tracked_free(old_tail);
        }
    }

    return SUCCESS;
}

ResultCode snake_set_direction(Snake* snake, Direction new_dir) {
    if (!snake) {
        return ERROR_NULL_POINTER;
    }

    if (new_dir == DIR_NONE) {
        return ERROR_INVALID_DIRECTION;
    }

    /* Prevent 180-degree turns */
    if (direction_is_opposite(snake->current_direction, new_dir)) {
        return ERROR_INVALID_DIRECTION;
    }

    snake->next_direction = new_dir;
    return SUCCESS;
}

bool snake_collides_with_self(const Snake* snake, Position pos) {
    if (!snake) {
        return false;
    }

    /* Check collision with body (skip head) */
    SnakeSegment* current = snake->head->next;
    while (current) {
        if (position_equals(current->pos, pos)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

bool snake_contains_position(const Snake* snake, Position pos) {
    if (!snake) {
        return false;
    }

    SnakeSegment* current = snake->head;
    while (current) {
        if (position_equals(current->pos, pos)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

SnakeSegment* snake_get_segment_at(const Snake* snake, Position pos) {
    if (!snake) {
        return NULL;
    }

    SnakeSegment* current = snake->head;
    while (current) {
        if (position_equals(current->pos, pos)) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Position snake_get_next_head_position(const Snake* snake) {
    if (!snake || !snake->head) {
        return (Position){-1, -1};
    }

    return position_move(snake->head->pos, snake->next_direction);
}
