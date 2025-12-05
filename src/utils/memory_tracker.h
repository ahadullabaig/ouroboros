#ifndef MEMORY_TRACKER_H
#define MEMORY_TRACKER_H

#include <stddef.h>
#include <stdint.h>

/**
 * Memory tracking system for monitoring heap allocations
 * Wraps malloc/calloc/free to track current and peak memory usage
 */

/**
 * Initialize the memory tracker
 * Must be called before any tracked allocations
 */
void memory_tracker_init(void);

/**
 * Allocate memory with tracking
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory or NULL on failure
 */
void* memory_tracked_malloc(size_t size);

/**
 * Allocate zeroed memory with tracking
 * @param nmemb Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory or NULL on failure
 */
void* memory_tracked_calloc(size_t nmemb, size_t size);

/**
 * Free memory allocated with tracking
 * @param ptr Pointer to memory to free (can be NULL)
 */
void memory_tracked_free(void* ptr);

/**
 * Get current memory usage in bytes
 * @return Number of bytes currently allocated
 */
uint64_t memory_get_current_usage(void);

/**
 * Get peak memory usage in bytes
 * @return Maximum number of bytes allocated at any point
 */
uint64_t memory_get_peak_usage(void);

/**
 * Reset memory usage statistics
 * Does not affect actual allocations, only resets counters
 */
void memory_tracker_reset(void);

#endif /* MEMORY_TRACKER_H */
