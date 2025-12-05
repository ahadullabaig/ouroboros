#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

/**
 * High-resolution timer for performance measurement
 * Uses clock_gettime with CLOCK_MONOTONIC for accurate timing
 */
typedef struct {
    struct timespec start;
    struct timespec end;
} Timer;

/**
 * Start the timer
 * @param timer Timer to start
 */
void timer_start(Timer* timer);

/**
 * Stop the timer
 * @param timer Timer to stop
 */
void timer_stop(Timer* timer);

/**
 * Get elapsed time in microseconds
 * @param timer Timer to query
 * @return Elapsed time in microseconds
 */
uint64_t timer_elapsed_us(const Timer* timer);

/**
 * Get elapsed time in milliseconds
 * @param timer Timer to query
 * @return Elapsed time in milliseconds
 */
uint64_t timer_elapsed_ms(const Timer* timer);

#endif /* TIMER_H */
