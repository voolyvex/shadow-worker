#ifndef SHADOW_WORKER_DEBUG_H
#define SHADOW_WORKER_DEBUG_H

#include "utils/logger.h"
#include <stdio.h>
#include <stdlib.h>

#if !defined(DEBUG_MEMORY_TRACKING) && defined(_DEBUG)
    #define DEBUG_MEMORY_TRACKING
#endif

#ifdef DEBUG_MEMORY_TRACKING
    #define TRACK_ALLOC(ptr, size) \
        do { \
            if (ptr) { \
                LOG_DEBUG(LOG_CORE, "Memory allocated: %p (%zu bytes)", (void*)ptr, size); \
            } \
        } while(0)

    #define TRACK_FREE(ptr) \
        do { \
            if (ptr) { \
                LOG_DEBUG(LOG_CORE, "Memory freed: %p", (void*)ptr); \
            } \
        } while(0)

    #define VALIDATE_PTR(ptr) \
        do { \
            if (!ptr) { \
                LOG_ERROR(LOG_CORE, "NULL pointer detected at %s:%d", __FILE__, __LINE__); \
                return false; \
            } \
        } while(0)

    #define VALIDATE_PTR_RETURN(ptr, retval) \
        do { \
            if (!ptr) { \
                LOG_ERROR(LOG_CORE, "NULL pointer detected at %s:%d", __FILE__, __LINE__); \
                return retval; \
            } \
        } while(0)

#else
    #define TRACK_ALLOC(ptr, size) ((void)0)
    #define TRACK_FREE(ptr) ((void)0)
    #define VALIDATE_PTR(ptr) ((void)0)
    #define VALIDATE_PTR_RETURN(ptr, retval) if (!ptr) return retval
#endif

// Debug function to dump memory state
static inline void DumpMemoryState(const char* tag) {
#ifdef DEBUG_MEMORY_TRACKING
    LOG_DEBUG(LOG_CORE, "Memory state dump [%s]", tag);
    // Add system-specific memory state reporting here
#endif
}

#endif // SHADOW_WORKER_DEBUG_H 