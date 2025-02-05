#include "../include/test_suites.h"
#include "../../include/world.h"
#include "../../include/map_types.h"
#include "../include/resource_manager.h"
#include "../include/entity_pool.h"
#include "../include/logger.h"
#include <assert.h>
#include <stdio.h>

static bool verify_resources(void) {
    // Check if resource files exist
    const char* required_files[] = {
        "resources/maps/tileset_main.png",
        "resources/fonts/dialogue.ttf"
    };
    
    bool all_exist = true;
    for (int i = 0; i < sizeof(required_files) / sizeof(required_files[0]); i++) {
        FILE* f = fopen(required_files[i], "rb");
        if (!f) {
            LOG_ERROR(LOG_CORE, "Required resource missing: %s", required_files[i]);
            all_exist = false;
        } else {
            fclose(f);
            LOG_INFO(LOG_CORE, "Found resource: %s", required_files[i]);
        }
    }
    return all_exist;
}

static int TestWorldCreation(void);
static int TestEntityManagement(void);
static int TestWorldInteractions(void);

int run_world_tests(void) {
    printf("\nRunning World Tests...\n");
    int failures = 0;
    
    failures += TestWorldCreation();
    failures += TestEntityManagement();
    failures += TestWorldInteractions();
    
    return failures;
}

static int TestWorldCreation(void) {
    printf("\nTesting World Creation...\n");
    
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);
    
    // Create world with the resource manager
    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test world properties
    TEST_EQUAL(world->width, 10);
    TEST_EQUAL(world->height, 10);
    TEST_FLOAT_EQUAL(world->gravity, 9.81f);
    TEST_NOT_NULL(world->resourceManager);
    
    // Clean up
    DestroyWorld(world);
    // Don't destroy the global resource manager
    
    return 0;
}

static int TestEntityManagement(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test entity pool
    TEST_NOT_NULL(world->entityPool);
    
    // Test entity creation
    Entity* entity = CreateEntity(world->entityPool, ENTITY_TYPE_NPC, (Vector2){5, 5});
    TEST_NOT_NULL(entity);
    TEST_EQUAL_ENUM(entity->type, ENTITY_TYPE_NPC);
    
    DestroyWorld(world);
    return TEST_PASSED;
}

static int TestWorldInteractions(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test tile interactions
    Tile wallTile = CreateTile(TILE_WALL, OBJECT_NONE);
    SetTileAt(world, 5, 5, wallTile);
    Tile resultTile = GetTileAt(world, 5, 5);
    TEST_EQUAL_ENUM(resultTile.type, TILE_WALL);
    
    // Test walkability
    Vector2 wallPos = {5, 5};
    Vector2 emptyPos = {0, 0};
    TEST_ASSERT(!IsWalkable(world, wallPos));  // Wall tile should not be walkable
    TEST_ASSERT(IsWalkable(world, emptyPos));   // Empty tile should be walkable
    
    DestroyWorld(world);
    return TEST_PASSED;
}

void test_world_tile_operations(void) {
    LOG_INFO(LOG_CORE, "Starting tile operations test");
    
    // Initialize resource manager
    assert(InitResourceManager());
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL);
    
    // Create world
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    assert(world != NULL);
    
    // Test tile operations
    const int testX = 5;
    const int testY = 5;
    
    // Set and verify tile
    Tile grassTile = CreateTile(TILE_GRASS, OBJECT_NONE);
    SetTileAt(world, testX, testY, grassTile);
    Tile resultTile = GetTileAt(world, testX, testY);
    assert(resultTile.type == TILE_GRASS && "Tile set/get operation failed");
    LOG_INFO(LOG_CORE, "Tile operations working correctly");
    
    // Test bounds checking
    Tile outOfBoundsTile = GetTileAt(world, -1, -1);
    assert(outOfBoundsTile.type == TILE_WALL && "Out-of-bounds tile check failed");
    outOfBoundsTile = GetTileAt(world, ESTATE_WIDTH + 1, ESTATE_HEIGHT + 1);
    assert(outOfBoundsTile.type == TILE_WALL && "Out-of-bounds tile check failed");
    LOG_INFO(LOG_CORE, "Tile bounds checking working correctly");
    
    // Cleanup
    DestroyWorld(world);
    LOG_INFO(LOG_CORE, "Tile operations test completed");
}

void test_world_object_operations(void) {
    LOG_INFO(LOG_CORE, "Starting object operations test");
    
    // Initialize resource manager
    assert(InitResourceManager());
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL);
    
    // Create world
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    assert(world != NULL);
    
    // Test object operations
    const int testX = 5;
    const int testY = 5;
    
    // Set and verify object
    Tile objectTile = CreateTile(TILE_EMPTY, OBJECT_TREE);
    SetTileAt(world, testX, testY, objectTile);
    Tile resultTile = GetTileAt(world, testX, testY);
    assert(resultTile.objectType == OBJECT_TREE && "Object set/get operation failed");
    LOG_INFO(LOG_CORE, "Object operations working correctly");
    
    // Test bounds checking
    Tile outOfBoundsTile = GetTileAt(world, -1, -1);
    assert(outOfBoundsTile.objectType == OBJECT_NONE && "Out-of-bounds object check failed");
    outOfBoundsTile = GetTileAt(world, ESTATE_WIDTH + 1, ESTATE_HEIGHT + 1);
    assert(outOfBoundsTile.objectType == OBJECT_NONE && "Out-of-bounds object check failed");
    LOG_INFO(LOG_CORE, "Object bounds checking working correctly");
    
    // Cleanup
    DestroyWorld(world);
    LOG_INFO(LOG_CORE, "Object operations test completed");
} 