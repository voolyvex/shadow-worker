#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/entity_types.h"
#include "../include/entity.h"
#include "../include/entity_pool.h"
#include "../include/world.h"
#include "../include/map_types.h"
#include "../include/constants.h"
#include "../src/core/room_loader.h"
#include "../include/resource_manager.h"
#include <assert.h>
#include "../include/test_suites.h"

// Forward declarations
static int TestWorldStructure(void);
static int TestRoomTemplate(void);
static int TestResourceManagement(void);
static int TestTileRendering(void);
static int TestCollision(void);

int run_core_system_tests(void) {
    printf("=== Starting Core System Tests ===\n");
    
    int failures = 0;
    failures += TestWorldStructure();
    failures += TestRoomTemplate();
    failures += TestResourceManagement();
    failures += TestTileRendering();
    failures += TestCollision();
    
    printf("=== Core System Tests Completed ===\n");
    printf("Total failures: %d\n", failures);
    
    return failures;
}

static int TestWorldStructure(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    // Test world initialization
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    TEST_EQUAL((int)world->dimensions.x, ESTATE_WIDTH * TILE_SIZE);
    TEST_EQUAL((int)world->dimensions.y, ESTATE_HEIGHT * TILE_SIZE);
    TEST_NOT_NULL(world->tiles);
    
    // Test tile operations
    Tile wallTile = CreateTile(TILE_WALL, OBJECT_NONE);
    SetTileAt(world, 0, 0, wallTile);
    Tile resultTile = GetTileAt(world, 0, 0);
    TEST_EQUAL_ENUM(resultTile.type, TILE_WALL);
    
    Vector2 pos = {0, 0};
    TEST_ASSERT(!IsWalkable(world, pos));
    
    DestroyWorld(world);
    return TEST_PASSED;
}

static int TestRoomTemplate(void) {
    return TEST_PASSED; // TODO: Implement
}

static int TestResourceManagement(void) {
    return TEST_PASSED; // TODO: Implement
}

static int TestTileRendering(void) {
    return TEST_PASSED; // TODO: Implement
}

static int TestCollision(void) {
    return TEST_PASSED; // TODO: Implement
} 