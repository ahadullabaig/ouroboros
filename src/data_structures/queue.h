#ifndef QUEUE_H
#define QUEUE_H

#include "../../include/common.h"

/**
 * Circular queue for BFS traversal
 * Pre-allocated fixed-size array to avoid per-frame allocations
 */

/* Queue node for BFS */
typedef struct {
    Position pos;
    int distance;
} QueueNode;

/* Circular queue structure */
typedef struct {
    QueueNode* data;
    size_t capacity;
    size_t front;
    size_t rear;
    size_t size;
} Queue;

/**
 * Create a new queue with specified capacity
 * @param capacity Maximum number of elements
 * @return Pointer to new queue or NULL on failure
 */
Queue* queue_create(size_t capacity);

/**
 * Destroy queue and free memory
 * @param queue Queue to destroy
 */
void queue_destroy(Queue* queue);

/**
 * Add element to the rear of the queue
 * @param queue Queue to enqueue to
 * @param pos Position to enqueue
 * @param distance Distance value for BFS
 * @return SUCCESS or ERROR_QUEUE_FULL
 */
ResultCode queue_enqueue(Queue* queue, Position pos, int distance);

/**
 * Remove element from the front of the queue
 * @param queue Queue to dequeue from
 * @param out_node Output parameter for dequeued node
 * @return SUCCESS or ERROR_QUEUE_EMPTY
 */
ResultCode queue_dequeue(Queue* queue, QueueNode* out_node);

/**
 * Check if queue is empty
 * @param queue Queue to check
 * @return true if empty, false otherwise
 */
bool queue_is_empty(const Queue* queue);

/**
 * Check if queue is full
 * @param queue Queue to check
 * @return true if full, false otherwise
 */
bool queue_is_full(const Queue* queue);

/**
 * Clear all elements from queue
 * @param queue Queue to clear
 */
void queue_clear(Queue* queue);

/**
 * Get current size of queue
 * @param queue Queue to query
 * @return Number of elements in queue
 */
size_t queue_size(const Queue* queue);

#endif /* QUEUE_H */
