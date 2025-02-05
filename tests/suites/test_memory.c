#include "../include/test_suites.h"
#include "../../include/world.h"
#include "../../include/entity_pool.h"
#include "../../include/resource_manager.h"
#include <stdio.h>
#include <assert.h>

static int TestMemoryAllocation(void);
static int TestMemoryLeaks(void);

int run_memory_tests(void) {
    printf("\nRunning Memory Tests...\n");
    int failures = 0;
    
    failures += TestMemoryAllocation();
    failures += TestMemoryLeaks();
    
    return failures;
}

static int TestMemoryAllocation(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    // Test world creation and destruction
    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test entity pool allocation
    EntityPool* pool = CreateEntityPool(100);
    TEST_NOT_NULL(pool);
    
    // Cleanup
    DestroyWorld(world);
    DestroyEntityPool(pool);
    
    return TEST_PASSED;
}

static int TestMemoryLeaks(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    // Create and immediately destroy objects to check for leaks
    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Create and destroy multiple entities
    EntityPool* pool = CreateEntityPool(10);
    TEST_NOT_NULL(pool);
    
    for (int i = 0; i < 5; i++) {
        Entity* entity = CreateEntity(pool, ENTITY_TYPE_NPC, (Vector2){0, 0});
        TEST_NOT_NULL(entity);
    }
    
    // Cleanup
    DestroyEntityPool(pool);
    DestroyWorld(world);
    
    return TEST_PASSED;
} 