#include "memory_tracker.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Global memory tracking state */
static struct {
    uint64_t current_bytes;
    uint64_t peak_bytes;
    bool initialized;
} g_memory_tracker = {0, 0, false};

/* Header prepended to each allocation to track size */
typedef struct {
    size_t size;
    uint32_t magic;  /* For validation */
} AllocationHeader;

#define ALLOCATION_MAGIC 0xDEADBEEF

void memory_tracker_init(void) {
    g_memory_tracker.current_bytes = 0;
    g_memory_tracker.peak_bytes = 0;
    g_memory_tracker.initialized = true;
}

void* memory_tracked_malloc(size_t size) {
    if (!g_memory_tracker.initialized) {
        memory_tracker_init();
    }

    if (size == 0) {
        return NULL;
    }

    /* Allocate extra space for header */
    size_t total_size = size + sizeof(AllocationHeader);
    AllocationHeader* header = (AllocationHeader*)malloc(total_size);

    if (!header) {
        return NULL;
    }

    /* Store allocation size and magic number */
    header->size = size;
    header->magic = ALLOCATION_MAGIC;

    /* Update statistics */
    g_memory_tracker.current_bytes += size;
    if (g_memory_tracker.current_bytes > g_memory_tracker.peak_bytes) {
        g_memory_tracker.peak_bytes = g_memory_tracker.current_bytes;
    }

    /* Return pointer after header */
    return (void*)(header + 1);
}

void* memory_tracked_calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = memory_tracked_malloc(total_size);

    if (ptr) {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

void memory_tracked_free(void* ptr) {
    if (!ptr) {
        return;
    }

    /* Get header from before the user pointer */
    AllocationHeader* header = ((AllocationHeader*)ptr) - 1;

    /* Validate magic number */
    if (header->magic != ALLOCATION_MAGIC) {
        /* Invalid free - likely not allocated with tracked allocator */
        return;
    }

    /* Update statistics */
    g_memory_tracker.current_bytes -= header->size;

    /* Free the whole allocation (including header) */
    free(header);
}

uint64_t memory_get_current_usage(void) {
    return g_memory_tracker.current_bytes;
}

uint64_t memory_get_peak_usage(void) {
    return g_memory_tracker.peak_bytes;
}

void memory_tracker_reset(void) {
    g_memory_tracker.peak_bytes = g_memory_tracker.current_bytes;
}
