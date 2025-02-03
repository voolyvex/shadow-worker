#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "entity_types.h"

// Add size type safety
#define SAFE_SIZE_T(x) ((x) > SIZE_MAX ? SIZE_MAX : (x))

// Helper function to align memory to specified boundary
static void* AlignedAlloc(size_t size, size_t alignment) {
    void* ptr = NULL;
#ifdef _WIN32
    ptr = _aligned_malloc(SAFE_SIZE_T(size), alignment);
#else
    if (posix_memalign(&ptr, alignment, SAFE_SIZE_T(size)) != 0) {
        ptr = NULL;
    }
#endif
    return ptr;
}

static void AlignedFree(void* ptr) {
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

ComponentRegistry* CreateComponentRegistry(void) {
    ComponentRegistry* registry = (ComponentRegistry*)malloc(sizeof(ComponentRegistry));
    if (!registry) return NULL;

    memset(registry, 0, sizeof(ComponentRegistry));
    
    // Initialize arrays with default capacities
    for (size_t i = 0; i < MAX_COMPONENT_TYPES; i++) {
        registry->componentArrays[i] = NULL;
        registry->componentSizes[i] = 0;
        registry->componentCounts[i] = 0;
        registry->componentCapacities[i] = 0;
    }

    return registry;
}

void DestroyComponentRegistry(ComponentRegistry* registry) {
    if (!registry) return;

    // Free all component arrays
    for (int i = 0; i < MAX_COMPONENT_TYPES; i++) {
        if (registry->componentArrays[i]) {
            AlignedFree(registry->componentArrays[i]);
        }
    }

    free(registry);
}

void* GetComponentArray(ComponentRegistry* registry, ComponentFlags type) {
    if (!registry || type >= MAX_COMPONENT_TYPES) return NULL;
    return registry->componentArrays[type];
}

void* AddComponentToRegistry(ComponentRegistry* registry, ComponentFlags type, size_t componentSize) {
    if (!registry || type >= MAX_COMPONENT_TYPES) return NULL;

    // If array doesn't exist, create it
    if (!registry->componentArrays[type]) {
        size_t initialCapacity = INITIAL_POOL_SIZE;
        registry->componentArrays[type] = AlignedAlloc(componentSize * initialCapacity, COMPONENT_ARRAY_ALIGNMENT);
        if (!registry->componentArrays[type]) return NULL;

        registry->componentSizes[type] = componentSize;
        registry->componentCapacities[type] = initialCapacity;
        registry->componentCounts[type] = 0;
    }

    // Check if we need to grow the array
    if (registry->componentCounts[type] >= registry->componentCapacities[type]) {
        size_t newCapacity = registry->componentCapacities[type] * POOL_GROWTH_FACTOR;
        void* newArray = AlignedAlloc(componentSize * newCapacity, COMPONENT_ARRAY_ALIGNMENT);
        if (!newArray) return NULL;

        // Copy existing data
        memcpy(newArray, registry->componentArrays[type], 
               componentSize * registry->componentCounts[type]);
        
        AlignedFree(registry->componentArrays[type]);
        registry->componentArrays[type] = newArray;
        registry->componentCapacities[type] = newCapacity;
    }

    // Return pointer to the next available slot
    void* componentPtr = (uint8_t*)registry->componentArrays[type] + 
                        (registry->componentCounts[type] * componentSize);
    registry->componentCounts[type]++;
    
    return componentPtr;
}

void RemoveComponentFromRegistry(ComponentRegistry* registry, ComponentFlags type, size_t entityId) {
    if (!registry || type >= MAX_COMPONENT_TYPES || !registry->componentArrays[type]) return;

    size_t componentSize = registry->componentSizes[type];
    size_t lastIndex = registry->componentCounts[type] - 1;

    // Only proceed if the entity ID is valid
    if (entityId >= registry->componentCounts[type]) return;

    // If it's not the last element, move the last element to the removed position
    if (entityId < lastIndex) {
        void* destPtr = (uint8_t*)registry->componentArrays[type] + (entityId * componentSize);
        void* srcPtr = (uint8_t*)registry->componentArrays[type] + (lastIndex * componentSize);
        memcpy(destPtr, srcPtr, componentSize);
    }

    registry->componentCounts[type]--;
} 