#include "map.h"
#include <assert.h>
#include <stdio.h>

// Test helper functions
static void AssertInBounds(void) {
    printf("Testing IsInBounds...\n");
    
    // Test valid coordinates
    assert(IsInBounds(0, 0));
    assert(IsInBounds(ESTATE_WIDTH - 1, ESTATE_HEIGHT - 1));
    
    // Test invalid coordinates
    assert(!IsInBounds(-1, 0));
    assert(!IsInBounds(0, -1));
    assert(!IsInBounds(ESTATE_WIDTH, 0));
    assert(!IsInBounds(0, ESTATE_HEIGHT));
    
    printf("IsInBounds tests passed!\n");
}

static void AssertGetIndex(void) {
    printf("Testing GetIndex...\n");
    
    // Test various coordinates
    assert(GetIndex(0, 0) == 0);
    assert(GetIndex(1, 0) == 1);
    assert(GetIndex(0, 1) == ESTATE_WIDTH);
    assert(GetIndex(1, 1) == ESTATE_WIDTH + 1);
    
    printf("GetIndex tests passed!\n");
}

static void ValidateMapCreation(void) {
    printf("Testing map creation...\n");
    
    EstateMap* map = CreateEstateMap();
    assert(map != NULL);
    assert(map->tiles != NULL);
    assert(map->objects != NULL);
    assert(map->spawnPoints != NULL);
    assert(map->spawnPointCount == 0);
    
    UnloadEstateMap(map);
    printf("Map creation tests passed!\n");
}

static void ValidateMapGeneration(void) {
    printf("Testing map generation...\n");
    
    EstateMap* map = CreateEstateMap();
    GenerateEstate(map);
    
    // Validate courtyard exists
    bool hasCourtyard = false;
    bool hasPaths = false;
    bool hasFountain = false;
    
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            if (GET_TILE(map, x, y) == TILE_FLOOR) {
                hasCourtyard = true;
            }
            if (GET_TILE(map, x, y) == TILE_PATH) {
                hasPaths = true;
            }
            if (GET_OBJECT(map, x, y) == OBJECT_FOUNTAIN) {
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
    printf("Map generation tests passed!\n");
}

int main(void) {
    printf("Starting estate map tests...\n\n");
    
    AssertInBounds();
    AssertGetIndex();
    ValidateMapCreation();
    ValidateMapGeneration();
    
    printf("\nAll estate map tests passed successfully!\n");
    return 0;
} 