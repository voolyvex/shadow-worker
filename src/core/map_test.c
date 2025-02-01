#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Mock world for testing
static World* CreateMockWorld(void) {
    World* world = (World*)calloc(1, sizeof(World));
    world->tiles = (TileType*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(TileType));
    world->tileProperties = (TileProperties*)calloc(TILE_COUNT, sizeof(TileProperties));
    return world;
}

static void DestroyMockWorld(World* world) {
    if (!world) return;
    if (world->tiles) free(world->tiles);
    if (world->tileProperties) free(world->tileProperties);
    free(world);
}

// Test helper functions
static void ValidateMapCreation(void) {
    printf("Testing map creation...\n");
    
    World* world = CreateMockWorld();
    EstateMap* map = CreateEstateMap(world);
    
    assert(map != NULL);
    assert(map->world == world);
    assert(map->spawnPoints != NULL);
    assert(map->spawnPointCount == 0);
    
    UnloadEstateMap(map);
    DestroyMockWorld(world);
    printf("Map creation tests passed!\n");
}

static void ValidateMapGeneration(void) {
    printf("Testing map generation...\n");
    
    World* world = CreateMockWorld();
    EstateMap* map = CreateEstateMap(world);
    GenerateEstate(map);
    
    // Validate courtyard exists
    bool hasCourtyard = false;
    bool hasPaths = false;
    bool hasFountain = false;
    
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            TileType tile = GetTileAt(world, x, y);
            if (tile == TILE_FLOOR) {
                hasCourtyard = true;
            }
            if (tile == TILE_PATH) {
                hasPaths = true;
            }
            if (tile == TILE_FOUNTAIN) {
                hasFountain = true;
                // Fountain should be at center
                assert(x == centerX);
                assert(y == centerY);
            }
        }
    }
    
    assert(hasCourtyard);
    assert(hasPaths);
    assert(hasFountain);
    assert(map->spawnPointCount > 0);
    assert(map->spawnPointCount <= MAX_SPAWN_POINTS);
    
    UnloadEstateMap(map);
    DestroyMockWorld(world);
    printf("Map generation tests passed!\n");
}

static void ValidateSpawnPoints(void) {
    printf("Testing spawn points...\n");
    
    World* world = CreateMockWorld();
    EstateMap* map = CreateEstateMap(world);
    GenerateEstate(map);
    
    // Test random spawn point
    Vector2 spawnPoint = GetRandomSpawnPoint(map);
    assert(IsValidSpawnPoint(map, spawnPoint));
    
    // Test invalid spawn point
    Vector2 invalidPoint = {-1, -1};
    assert(!IsValidSpawnPoint(map, invalidPoint));
    
    UnloadEstateMap(map);
    DestroyMockWorld(world);
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