#include "queue.h"
#include "../utils/memory_tracker.h"
#include <stdlib.h>

Queue* queue_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    Queue* queue = (Queue*)memory_tracked_malloc(sizeof(Queue));
    if (!queue) {
        return NULL;
    }

    queue->data = (QueueNode*)memory_tracked_calloc(capacity, sizeof(QueueNode));
    if (!queue->data) {
        memory_tracked_free(queue);
        return NULL;
    }

    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;

    return queue;
}

void queue_destroy(Queue* queue) {
    if (queue) {
        if (queue->data) {
            memory_tracked_free(queue->data);
        }
        memory_tracked_free(queue);
    }
}

ResultCode queue_enqueue(Queue* queue, Position pos, int distance) {
    if (!queue) {
        return ERROR_NULL_POINTER;
    }

    if (queue_is_full(queue)) {
        return ERROR_QUEUE_FULL;
    }

    queue->data[queue->rear].pos = pos;
    queue->data[queue->rear].distance = distance;

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    return SUCCESS;
}

ResultCode queue_dequeue(Queue* queue, QueueNode* out_node) {
    if (!queue || !out_node) {
        return ERROR_NULL_POINTER;
    }

    if (queue_is_empty(queue)) {
        return ERROR_QUEUE_EMPTY;
    }

    *out_node = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return SUCCESS;
}

bool queue_is_empty(const Queue* queue) {
    return queue && queue->size == 0;
}

bool queue_is_full(const Queue* queue) {
    return queue && queue->size == queue->capacity;
}

void queue_clear(Queue* queue) {
    if (queue) {
        queue->front = 0;
        queue->rear = 0;
        queue->size = 0;
    }
}

size_t queue_size(const Queue* queue) {
    return queue ? queue->size : 0;
}
