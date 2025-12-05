#define _POSIX_C_SOURCE 200112L
#include "timer.h"
#include <time.h>

void timer_start(Timer* timer) {
    if (timer) {
        clock_gettime(CLOCK_MONOTONIC, &timer->start);
    }
}

void timer_stop(Timer* timer) {
    if (timer) {
        clock_gettime(CLOCK_MONOTONIC, &timer->end);
    }
}

uint64_t timer_elapsed_us(const Timer* timer) {
    if (!timer) {
        return 0;
    }

    uint64_t start_us = (uint64_t)timer->start.tv_sec * 1000000ULL +
                        (uint64_t)timer->start.tv_nsec / 1000ULL;
    uint64_t end_us = (uint64_t)timer->end.tv_sec * 1000000ULL +
                      (uint64_t)timer->end.tv_nsec / 1000ULL;

    return end_us - start_us;
}

uint64_t timer_elapsed_ms(const Timer* timer) {
    return timer_elapsed_us(timer) / 1000ULL;
}
