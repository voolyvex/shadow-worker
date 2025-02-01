#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Mock world for testing
static World* CreateMockWorld(void) {
    World* world = (World*)calloc(1, sizeof(World));
    world->tiles = (TileType*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(TileType));
    world->tileProperties = (TileProperties*)calloc(TILE_COUNT, sizeof(TileProperties));
    world->objects = (ObjectType*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(ObjectType));
    return world;
}

static void DestroyMockWorld(World* world) {
    if (!world) return;
    if (world->tiles) free(world->tiles);
    if (world->tileProperties) free(world->tileProperties);
    if (world->objects) free(world->objects);
    free(world);
}

// Test helper functions
static void ValidateMapCreation(void) {
    printf("Testing map creation...\n");
    
    EstateMap* map = CreateEstateMap();
    
    assert(map != NULL);
    assert(map->world != NULL);
    assert(map->spawnPointCount == 0);
    
    DestroyEstateMap(map);
    printf("Map creation tests passed!\n");
}

static void ValidateMapGeneration(void) {
    printf("Testing map generation...\n");
    
    EstateMap* map = CreateEstateMap();
    GenerateEstateMap(map);
    
    // Validate paths and objects exist
    bool hasPaths = false;
    bool hasObjects = false;
    bool hasWater = false;
    
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            TileType tile = GET_TILE(map, x, y);
            ObjectType obj = GET_OBJECT(map, x, y);
            
            if (tile == TILE_PATH) {
                hasPaths = true;
            }
            if (tile == TILE_WATER) {
                hasWater = true;
            }
            if (obj != OBJECT_NONE) {
                hasObjects = true;
            }
        }
    }
    
    assert(hasPaths);
    assert(hasWater);
    assert(hasObjects);
    assert(map->spawnPointCount > 0);
    assert(map->spawnPointCount <= MAX_SPAWN_POINTS);
    
    DestroyEstateMap(map);
    printf("Map generation tests passed!\n");
}

static void ValidateSpawnPoints(void) {
    printf("Testing spawn points...\n");
    
    EstateMap* map = CreateEstateMap();
    GenerateEstateMap(map);
    
    // Test random spawn point
    Vector2 spawnPoint = GetRandomSpawnPoint(map);
    assert(IsValidSpawnPoint(map, spawnPoint));
    
    // Test invalid spawn point
    Vector2 invalidPoint = {-1, -1};
    assert(!IsValidSpawnPoint(map, invalidPoint));
    
    DestroyEstateMap(map);
    printf("Spawn point tests passed!\n");
}

int main(void) {
    printf("Starting estate map tests...\n\n");
    
    ValidateMapCreation();
    ValidateMapGeneration();
    ValidateSpawnPoints();
    
    printf("\nAll estate map tests passed successfully!\n");
    return 0;
} 