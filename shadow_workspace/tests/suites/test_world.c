#include "../include/world.h"
#include "../include/resource_manager.h"
#include "../include/entity_pool.h"
#include "../include/logger.h"
#include <assert.h>
#include <stdio.h>
#include "../test_env.c"

static bool verify_resources(void) {
    // Check if resource files exist
    const char* required_files[] = {
        "resources/maps/tileset_main.png",
        "resources/fonts/main.ttf"
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

void test_world_creation(void) {
    LOG_INFO(LOG_CORE, "Starting world creation test");
    
    // Verify resources first
    if (!verify_resources()) {
        LOG_WARN(LOG_CORE, "Some resources missing, tests may use fallbacks");
    }
    
    // Initialize resource manager
    bool rm_init = InitResourceManager();
    assert(rm_init && "Resource manager initialization failed");
    LOG_INFO(LOG_CORE, "Resource manager initialized");
    
    // Get resource manager instance
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL && "Failed to get resource manager instance");
    
    // Create world
    World* world = InitWorld(resources);
    assert(world != NULL && "World initialization failed");
    LOG_INFO(LOG_CORE, "World initialized successfully");
    
    // Verify world properties
    assert(world->dimensions.x > 0 && "Invalid world width");
    assert(world->dimensions.y > 0 && "Invalid world height");
    
    // Cleanup
    UnloadWorld(world);
    LOG_INFO(LOG_CORE, "World creation test completed");
}

void test_world_tile_operations(void) {
    LOG_INFO(LOG_CORE, "Starting tile operations test");
    
    // Initialize resource manager
    assert(InitResourceManager());
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL);
    
    // Create world
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    // Test tile operations
    const int testX = 5;
    const int testY = 5;
    
    // Set and verify tile
    SetTileAt(world, testX, testY, TILE_GRASS);
    TileType tile = GetTileAt(world, testX, testY);
    assert(tile == TILE_GRASS && "Tile set/get operation failed");
    LOG_INFO(LOG_CORE, "Tile operations working correctly");
    
    // Test bounds checking
    assert(GetTileAt(world, -1, -1) == TILE_WALL && "Out-of-bounds tile check failed");
    assert(GetTileAt(world, ESTATE_WIDTH + 1, ESTATE_HEIGHT + 1) == TILE_WALL && "Out-of-bounds tile check failed");
    LOG_INFO(LOG_CORE, "Tile bounds checking working correctly");
    
    // Cleanup
    UnloadWorld(world);
    LOG_INFO(LOG_CORE, "Tile operations test completed");
}

void test_world_object_operations(void) {
    LOG_INFO(LOG_CORE, "Starting object operations test");
    
    // Initialize resource manager
    assert(InitResourceManager());
    ResourceManager* resources = GetResourceManager();
    assert(resources != NULL);
    
    // Create world
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    // Test object operations
    const int testX = 5;
    const int testY = 5;
    
    // Set and verify object
    SetObjectAt(world, testX, testY, OBJECT_TREE);
    ObjectType obj = GetObjectAt(world, testX, testY);
    assert(obj == OBJECT_TREE && "Object set/get operation failed");
    LOG_INFO(LOG_CORE, "Object operations working correctly");
    
    // Test bounds checking
    assert(GetObjectAt(world, -1, -1) == OBJECT_NONE && "Out-of-bounds object check failed");
    assert(GetObjectAt(world, ESTATE_WIDTH + 1, ESTATE_HEIGHT + 1) == OBJECT_NONE && "Out-of-bounds object check failed");
    LOG_INFO(LOG_CORE, "Object bounds checking working correctly");
    
    // Cleanup
    UnloadWorld(world);
    LOG_INFO(LOG_CORE, "Object operations test completed");
}

void RunWorldTests(void) {
    setup_test_environment();
    LOG_INFO(LOG_CORE, "=== Starting World Tests ===");
    
    test_world_creation();
    test_world_tile_operations();
    test_world_object_operations();
    
    LOG_INFO(LOG_CORE, "=== World Tests Completed ===");
    teardown_test_environment();
} 