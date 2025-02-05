#include "../include/test_suites.h"
#include "../../include/world.h"
#include "../../include/entity.h"
#include "../../include/map_types.h"
#include "../../include/entity_pool.h"
#include "../../include/resource_manager.h"
#include <assert.h>
#include <stdio.h>

static int test_world_entity_integration(void) {
    printf("Testing world and entity integration...\n");
    
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);
    
    // Create world
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Create entity pool
    EntityPool* entityPool = CreateEntityPool(100);
    TEST_NOT_NULL(entityPool);
    
    // Integrate entities into the world
    world->entityPool = entityPool;
    
    // Test entity creation and placement
    Vector2 pos = {5, 5};
    Entity* entity = CreateEntity(entityPool, ENTITY_TYPE_NPC, pos);
    TEST_NOT_NULL(entity);
    TEST_EQUAL_ENUM(entity->type, ENTITY_TYPE_NPC);
    TEST_FLOAT_EQUAL(entity->position.x, pos.x);
    TEST_FLOAT_EQUAL(entity->position.y, pos.y);
    
    // Test entity-world interaction
    TEST_TRUE(IsWalkable(world, pos));  // Entity should be placed on walkable tile
    
    // Cleanup
    DestroyWorld(world);
    DestroyEntityPool(entityPool);
    printf("World and entity integration test completed\n");
    return TEST_PASSED;
}

int run_integration_tests(void) {
    printf("\nRunning Integration Tests...\n");
    int failures = 0;
    
    failures += test_world_entity_integration();
    
    return failures;
} 