#include <assert.h>
#include <stdint.h>
#include "../include/world.h"
#include "../include/resource_manager.h"

void test_memory_allocation(void) {
    ResourceManager* resources = GetResourceManager();
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    // Test memory alignment for pointers
    assert((uintptr_t)world->tiles % sizeof(void*) == 0);
    assert((uintptr_t)world->entityPool % sizeof(void*) == 0);
    
    UnloadWorld(world);
}

void test_memory_leaks(void) {
    // Create and destroy world multiple times to check for memory leaks
    for (int i = 0; i < 10; i++) {
        ResourceManager* resources = GetResourceManager();
        World* world = InitWorld(resources);
        assert(world != NULL);
        UnloadWorld(world);
    }
}

void run_memory_tests(void) {
    test_memory_allocation();
    test_memory_leaks();
} 