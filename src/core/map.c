#include "map.h"
#include <stdlib.h>
#include <string.h>
#include <raymath.h>

// Memory management helper
static void* SafeAlloc(size_t size) {
    void* ptr = calloc(1, size);
    if (!ptr) {
        TraceLog(LOG_ERROR, "Memory allocation failed");
        exit(1);
    }
    return ptr;
}

// Helper functions
static bool IsInBounds(int x, int y) {
    return x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT;
}

static int GetIndex(int x, int y) {
    return y * ESTATE_WIDTH + x;
}

// Core map functions
EstateMap* CreateEstateMap(void) {
    EstateMap* map = (EstateMap*)malloc(sizeof(EstateMap));
    if (!map) return NULL;
    
    map->world = InitWorld(GetResourceManager());
    if (!map->world) {
        free(map);
        return NULL;
    }
    
    map->spawnPointCount = 0;
    map->tileset = LoadTexture("assets/tileset.png");
    
    return map;
}

void DestroyEstateMap(EstateMap* map) {
    if (!map) return;
    
    UnloadTexture(map->tileset);
    UnloadWorld(map->world);
    free(map);
}

void GenerateEstateMap(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Initialize all tiles to grass
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SET_TILE(map, x, y, TILE_GRASS);
            SET_OBJECT(map, x, y, OBJECT_NONE);
        }
    }
    
    // Create a central path
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    // Horizontal path
    for (int x = centerX - 5; x <= centerX + 5; x++) {
        SET_TILE(map, x, centerY, TILE_PATH);
        SET_TILE(map, x, centerY - 1, TILE_PATH);
        SET_TILE(map, x, centerY + 1, TILE_PATH);
    }
    
    // Vertical path
    for (int y = centerY - 5; y <= centerY + 5; y++) {
        SET_TILE(map, centerX, y, TILE_PATH);
        SET_TILE(map, centerX - 1, y, TILE_PATH);
        SET_TILE(map, centerX + 1, y, TILE_PATH);
    }
    
    // Add some water features
    for (int i = 0; i < 4; i++) {
        int x = rand() % (ESTATE_WIDTH - 4) + 2;
        int y = rand() % (ESTATE_HEIGHT - 4) + 2;
        
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (IsInBounds(x + dx, y + dy)) {
                    SET_TILE(map, x + dx, y + dy, TILE_WATER);
                }
            }
        }
    }
    
    // Add some decorative objects
    for (int i = 0; i < 20; i++) {
        int x = rand() % ESTATE_WIDTH;
        int y = rand() % ESTATE_HEIGHT;
        
        if (GET_TILE(map, x, y) == TILE_GRASS) {
            ObjectType obj = (ObjectType)(rand() % (OBJECT_FOUNTAIN - OBJECT_TREE + 1) + OBJECT_TREE);
            SET_OBJECT(map, x, y, obj);
        }
    }
    
    // Set spawn points along the paths
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            if (GET_TILE(map, x, y) == TILE_PATH && map->spawnPointCount < MAX_SPAWN_POINTS) {
                map->spawnPoints[map->spawnPointCount] = (Vector2){
                    x * TILE_SIZE + TILE_SIZE/2,
                    y * TILE_SIZE + TILE_SIZE/2
                };
                map->spawnPointCount++;
            }
        }
    }
}

void DrawEstateMap(const EstateMap* map) {
    if (!map) return;
    
    // Draw base tiles
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            Rectangle destRect = {
                x * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };
            
            // Calculate source rectangle based on tile type
            TileType tile = GET_TILE(map, x, y);
            Rectangle sourceRect = {
                tile * TILE_SIZE,
                0,
                TILE_SIZE,
                TILE_SIZE
            };
            
            Vector2 origin = {0.0f, 0.0f};
            DrawTexturePro(map->tileset, sourceRect, destRect, origin, 0.0f, WHITE);
        }
    }
    
    // Draw objects
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            ObjectType obj = GET_OBJECT(map, x, y);
            if (obj != OBJECT_NONE) {
                Rectangle destRect = {
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
                
                Rectangle sourceRect = {
                    obj * TILE_SIZE,
                    TILE_SIZE, // Objects are on the second row of the tileset
                    TILE_SIZE,
                    TILE_SIZE
                };
                
                Vector2 origin = {0.0f, 0.0f};
                DrawTexturePro(map->tileset, sourceRect, destRect, origin, 0.0f, WHITE);
            }
        }
    }
}

ObjectType GetObjectAt(const World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return OBJECT_NONE;
    return world->objects[GetIndex(x, y)];
} 