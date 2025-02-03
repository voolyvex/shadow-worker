#include "../include/test_suites.h"
#include "../../include/world.h"
#include "../../include/entity_pool.h"
#include "../../include/resource_manager.h"
#include <assert.h>
#include <stdio.h>

void test_world_entity_integration(void) {
    printf("Testing world and entity integration...\n");
    
    // Initialize resource manager
    assert(InitResourceManager());
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL);
    
    // Create world
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    // Create entity pool
    EntityPool* entityPool = InitEntityPool(100);
    assert(entityPool != NULL);
    
    // Integrate entities into the world
    world->entityPool = entityPool;
    
    // Test entity integration
    // (Add specific integration test cases here)
    
    // Cleanup
    UnloadWorld(world);
    UnloadEntityPool(entityPool);
    printf("World and entity integration test completed\n");
}

void run_integration_tests(void) {
    printf("\nRunning integration tests...\n");
    test_world_entity_integration();
    printf("All integration tests passed successfully!\n\n");
} 