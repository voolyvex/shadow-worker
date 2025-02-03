#include <assert.h>
#include <stdio.h>
#include "../src/core/map.h"
#include "../include/resource_manager.h"
#include "../include/world.h"
#include "../test_env.c"

// Test helper functions
void test_map_creation(void) {
    printf("Testing map creation...\n");
    
    ResourceManager* resources = GetResourceManager();
    World* world = InitWorld(resources);
    assert(world != NULL);
    assert(world->tiles != NULL);
    assert(world->entityPool != NULL);
    
    UnloadWorld(world);
    printf("Map creation tests passed!\n");
}

void test_map_generation(void) {
    printf("Testing map generation...\n");
    ResourceManager* resources = GetResourceManager();
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    GenerateEstateMap(world);
    
    // Verify courtyard generation
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    bool hasCourtyard = false;
    bool hasWalls = false;
    bool hasPaths = false;
    bool hasFountain = false;
    bool hasGardens = false;
    
    // Check courtyard area
    for (int y = centerY - COURTYARD_SIZE/2; y <= centerY + COURTYARD_SIZE/2; y++) {
        for (int x = centerX - COURTYARD_SIZE/2; x <= centerX + COURTYARD_SIZE/2; x++) {
            TileType tile = GetTileAt(world, x, y);
            if (tile == TILE_FLOOR) hasCourtyard = true;
            if (tile == TILE_WALL) hasWalls = true;
            if (tile == TILE_PATH) hasPaths = true;
            
            ObjectType obj = GetObjectAt(world, x, y);
            if (obj == OBJECT_FOUNTAIN) hasFountain = true;
            if (obj == OBJECT_FLOWER || obj == OBJECT_BUSH) hasGardens = true;
        }
    }
    
    assert(hasCourtyard && "Courtyard floor not found");
    assert(hasWalls && "Courtyard walls not found");
    assert(hasPaths && "Paths not found");
    assert(hasFountain && "Central fountain not found");
    assert(hasGardens && "Garden decorations not found");
    
    printf("Map generation tests passed!\n");
    UnloadWorld(world);
}

void test_spawn_points(void) {
    printf("Testing spawn points...\n");
    ResourceManager* resources = GetResourceManager();
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    GenerateEstateMap(world);
    
    // Verify spawn points
    assert(world->spawnPointCount > 0 && "No spawn points generated");
    assert(world->spawnPointCount <= MAX_SPAWN_POINTS && "Too many spawn points");
    
    // Verify each spawn point
    for (int i = 0; i < world->spawnPointCount; i++) {
        Vector2 pos = world->spawnPoints[i];
        
        // Convert to tile coordinates
        int tileX = (int)(pos.x / TILE_SIZE);
        int tileY = (int)(pos.y / TILE_SIZE);
        
        // Verify spawn point is on a path
        assert(GetTileAt(world, tileX, tileY) == TILE_PATH && 
               "Spawn point not on path");
        
        // Verify clear space around spawn point
        bool hasSpace = true;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (tileX + dx >= 0 && tileX + dx < ESTATE_WIDTH &&
                    tileY + dy >= 0 && tileY + dy < ESTATE_HEIGHT) {
                    ObjectType obj = GetObjectAt(world, tileX + dx, tileY + dy);
                    if (obj != OBJECT_NONE) {
                        hasSpace = false;
                        break;
                    }
                }
            }
            if (!hasSpace) break;
        }
        assert(hasSpace && "Spawn point does not have clear space");
    }
    
    printf("Spawn point tests passed!\n");
    UnloadWorld(world);
}

void test_map_tile_access(void) {
    printf("Testing map tile access...\n");
    ResourceManager* resources = GetResourceManager();
    World* world = InitWorld(resources);
    assert(world != NULL);
    
    // Test initial state
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            assert(GetTileAt(world, x, y) == TILE_GRASS);
            assert(GetObjectAt(world, x, y) == OBJECT_NONE);
        }
    }
    
    // Test setting and getting tiles
    SetTileAt(world, 5, 5, TILE_PATH);
    assert(GetTileAt(world, 5, 5) == TILE_PATH);
    
    SetObjectAt(world, 5, 5, OBJECT_TREE);
    assert(GetObjectAt(world, 5, 5) == OBJECT_TREE);
    
    // Test bounds checking
    assert(GetTileAt(world, -1, 5) == TILE_GRASS);
    assert(GetTileAt(world, 5, -1) == TILE_GRASS);
    assert(GetTileAt(world, ESTATE_WIDTH + 1, 5) == TILE_GRASS);
    assert(GetTileAt(world, 5, ESTATE_HEIGHT + 1) == TILE_GRASS);
    
    printf("Map tile access tests passed!\n");
    UnloadWorld(world);
}

int passed_tests = 0;
int failed_tests = 0;

void run_map_tests(void) {
    setup_test_environment();
    LOG_INFO(LOG_CORE, "=== Starting Map Tests ===");
    
    if (test_map_creation()) passed_tests++; else failed_tests++;
    if (test_map_generation()) passed_tests++; else failed_tests++;
    if (test_spawn_points()) passed_tests++; else failed_tests++;
    if (test_map_tile_access()) passed_tests++; else failed_tests++;
    
    LOG_INFO(LOG_CORE, "=== Map Tests Completed ===");
    LOG_INFO(LOG_CORE, "Tests Passed: %d", passed_tests);
    LOG_INFO(LOG_CORE, "Tests Failed: %d", failed_tests);
    teardown_test_environment();
} 