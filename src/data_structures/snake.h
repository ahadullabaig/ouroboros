#ifndef SNAKE_H
#define SNAKE_H

#include "../../include/common.h"

/**
 * Snake segment (doubly linked list node)
 * Represents one cell of the snake's body
 */
typedef struct SnakeSegment {
    Position pos;
    struct SnakeSegment* next;
    struct SnakeSegment* prev;
} SnakeSegment;

/**
 * Snake structure
 * Doubly linked list with head and tail pointers for O(1) operations
 */
typedef struct {
    SnakeSegment* head;
    SnakeSegment* tail;
    Direction current_direction;
    Direction next_direction;
    size_t length;
    uint32_t score;
} Snake;

/**
 * Create a new snake with initial position and direction
 * @param initial_pos Starting position for snake head
 * @param initial_dir Starting direction
 * @return Pointer to new snake or NULL on failure
 */
Snake* snake_create(Position initial_pos, Direction initial_dir);

/**
 * Destroy snake and free all segments
 * @param snake Snake to destroy
 */
void snake_destroy(Snake* snake);

/**
 * Move the snake one step forward
 * @param snake Snake to move
 * @param grow If true, snake grows (don't remove tail)
 * @return SUCCESS or error code
 */
ResultCode snake_move(Snake* snake, bool grow);

/**
 * Set the next direction for the snake
 * Prevents 180-degree turns
 * @param snake Snake to update
 * @param new_dir New direction
 * @return SUCCESS or ERROR_INVALID_DIRECTION
 */
ResultCode snake_set_direction(Snake* snake, Direction new_dir);

/**
 * Check if snake collides with itself at a position
 * @param snake Snake to check
 * @param pos Position to check
 * @return true if collision, false otherwise
 */
bool snake_collides_with_self(const Snake* snake, Position pos);

/**
 * Check if snake contains a position
 * @param snake Snake to check
 * @param pos Position to check
 * @return true if snake occupies position, false otherwise
 */
bool snake_contains_position(const Snake* snake, Position pos);

/**
 * Get the segment at a specific position
 * @param snake Snake to search
 * @param pos Position to find
 * @return Pointer to segment or NULL if not found
 */
SnakeSegment* snake_get_segment_at(const Snake* snake, Position pos);

/**
 * Get the position where the head will be after moving in current direction
 * @param snake Snake to query
 * @return Next head position
 */
Position snake_get_next_head_position(const Snake* snake);

#endif /* SNAKE_H */
