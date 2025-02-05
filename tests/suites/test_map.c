#include <assert.h>
#include <stdio.h>
#include "../src/core/map.h"
#include "../include/resource_manager.h"
#include "../include/world.h"
#include "../include/test_suites.h"
#include "../../include/map_types.h"

// Test helper functions
int test_map_creation(void) {
    printf("Testing map creation...\n");
    
    ResourceManager* resources = GetResourceManager();
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    TEST_NOT_NULL(world->tiles);
    TEST_NOT_NULL(world->entityPool);
    
    DestroyWorld(world);
    printf("Map creation tests passed!\n");
    return TEST_PASSED;
}

static int test_map_generation(void) {
    printf("Testing map generation...\n");
    
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);
    
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    
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
            Tile tile = GetTileAt(world, x, y);
            if (tile.type == TILE_FLOOR) hasCourtyard = true;
            if (tile.type == TILE_WALL) hasWalls = true;
            if (tile.type == TILE_PATH) hasPaths = true;
            
            if (tile.objectType == OBJECT_FOUNTAIN) hasFountain = true;
            if (tile.objectType == OBJECT_FLOWER || tile.objectType == OBJECT_BUSH) hasGardens = true;
        }
    }
    
    TEST_TRUE(hasCourtyard);  // Courtyard floor not found
    TEST_TRUE(hasWalls);      // Courtyard walls not found
    TEST_TRUE(hasPaths);      // Paths not found
    TEST_TRUE(hasFountain);   // Central fountain not found
    TEST_TRUE(hasGardens);    // Garden decorations not found
    
    printf("Map generation tests passed!\n");
    DestroyWorld(world);
    return TEST_PASSED;
}

static int test_spawn_points(void) {
    printf("Testing spawn points...\n");
    
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);
    
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    GenerateEstateMap(world);
    
    // Verify spawn points
    TEST_TRUE(world->spawnPointCount > 0);  // No spawn points generated
    TEST_TRUE(world->spawnPointCount <= MAX_SPAWN_POINTS);  // Too many spawn points
    
    // Verify each spawn point
    for (int i = 0; i < world->spawnPointCount; i++) {
        Vector2 pos = world->spawnPoints[i];
        
        // Convert to tile coordinates
        int tileX = (int)(pos.x / TILE_SIZE);
        int tileY = (int)(pos.y / TILE_SIZE);
        
        // Verify spawn point is on a path
        Tile tile = GetTileAt(world, tileX, tileY);
        TEST_EQUAL_ENUM(tile.type, TILE_PATH);  // Spawn point not on path
        
        // Verify clear space around spawn point
        bool hasSpace = true;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (tileX + dx >= 0 && tileX + dx < ESTATE_WIDTH &&
                    tileY + dy >= 0 && tileY + dy < ESTATE_HEIGHT) {
                    Tile neighborTile = GetTileAt(world, tileX + dx, tileY + dy);
                    if (neighborTile.objectType != OBJECT_NONE) {
                        hasSpace = false;
                        break;
                    }
                }
            }
            if (!hasSpace) break;
        }
        TEST_TRUE(hasSpace);  // Spawn point does not have clear space
    }
    
    printf("Spawn point tests passed!\n");
    DestroyWorld(world);
    return TEST_PASSED;
}

static int test_map_tile_access(void) {
    printf("Testing map tile access...\n");
    
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);
    
    World* world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test initial state
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            Tile tile = GetTileAt(world, x, y);
            TEST_EQUAL_ENUM(tile.type, TILE_GRASS);
            TEST_EQUAL_ENUM(tile.objectType, OBJECT_NONE);
        }
    }
    
    // Test setting and getting tiles
    Tile pathTile = CreateTile(TILE_PATH, OBJECT_NONE);
    SetTileAt(world, 5, 5, pathTile);
    Tile resultTile = GetTileAt(world, 5, 5);
    TEST_EQUAL_ENUM(resultTile.type, TILE_PATH);
    
    Tile treeTile = CreateTile(resultTile.type, OBJECT_TREE);
    SetTileAt(world, 5, 5, treeTile);
    resultTile = GetTileAt(world, 5, 5);
    TEST_EQUAL_ENUM(resultTile.objectType, OBJECT_TREE);
    
    // Test bounds checking
    Tile outOfBoundsTile = GetTileAt(world, -1, 5);
    TEST_EQUAL_ENUM(outOfBoundsTile.type, TILE_GRASS);
    outOfBoundsTile = GetTileAt(world, 5, -1);
    TEST_EQUAL_ENUM(outOfBoundsTile.type, TILE_GRASS);
    outOfBoundsTile = GetTileAt(world, ESTATE_WIDTH + 1, 5);
    TEST_EQUAL_ENUM(outOfBoundsTile.type, TILE_GRASS);
    outOfBoundsTile = GetTileAt(world, 5, ESTATE_HEIGHT + 1);
    TEST_EQUAL_ENUM(outOfBoundsTile.type, TILE_GRASS);
    
    printf("Map tile access tests passed!\n");
    DestroyWorld(world);
    return TEST_PASSED;
}

static int TestTileOperations(void);
static int TestMapGeneration(void);
static int TestObjectPlacement(void);

int run_map_tests(void) {
    printf("\nRunning Map Tests...\n");
    int failures = 0;
    
    failures += test_map_creation();
    failures += test_map_generation();
    failures += test_spawn_points();
    failures += test_map_tile_access();
    failures += TestTileOperations();
    failures += TestMapGeneration();
    failures += TestObjectPlacement();
    
    return failures;
}

static int TestTileOperations(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test setting and getting tiles
    Tile wallTile = CreateTile(TILE_WALL, OBJECT_NONE);
    SetTileAt(world, 0, 0, wallTile);
    Tile resultTile = GetTileAt(world, 0, 0);
    TEST_EQUAL_ENUM(resultTile.type, TILE_WALL);
    
    // Test tile properties
    Vector2 pos = {0, 0};
    TEST_ASSERT(!IsWalkable(world, pos));  // Wall should not be walkable
    
    DestroyWorld(world);
    return TEST_PASSED;
}

static int TestMapGeneration(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test map dimensions
    TEST_EQUAL(world->width, 10);
    TEST_EQUAL(world->height, 10);
    
    // Test initial tile state
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            Tile tile = GetTileAt(world, x, y);
            TEST_EQUAL_ENUM(tile.type, TILE_EMPTY);  // Default tile type should be empty
        }
    }
    
    DestroyWorld(world);
    return TEST_PASSED;
}

static int TestObjectPlacement(void) {
    // Get the global resource manager
    ResourceManager* resources = GetResourceManager();
    TEST_NOT_NULL(resources);

    World* world = CreateWorld(10, 10, 9.81f, resources);
    TEST_NOT_NULL(world);
    
    // Test placing and removing objects
    Vector2 pos = {5, 5};
    AddObject(world, OBJECT_TORCH, pos);
    
    Tile tile = GetTileAt(world, 5, 5);
    TEST_EQUAL_ENUM(tile.objectType, OBJECT_TORCH);
    
    DestroyWorld(world);
    return TEST_PASSED;
} 