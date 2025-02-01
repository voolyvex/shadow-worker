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

// Utility functions implementation
bool IsInBounds(int x, int y) {
    return x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT;
}

int GetIndex(int x, int y) {
    return y * ESTATE_WIDTH + x;
}

// Core map functions implementation
EstateMap* CreateEstateMap(World* world) {
    if (!world) return NULL;
    
    EstateMap* map = SafeAlloc(sizeof(EstateMap));
    map->world = world;
    map->spawnPoints = SafeAlloc(MAX_SPAWN_POINTS * sizeof(Vector2));
    map->spawnPointCount = 0;
    
    return map;
}

void UnloadEstateMap(EstateMap* map) {
    if (!map) return;
    
    if (map->spawnPoints) free(map->spawnPoints);
    free(map);
}

void GenerateEstate(EstateMap* map) {
    if (!map || !map->world) return;
    
    InitializeBaseTerrain(map);
    CreateCentralCourtyard(map);
    GenerateMainPaths(map);
    PlaceFountain(map);
    CreateGardens(map);
    AddDecorations(map);
    SetSpawnPoints(map);
}

void InitializeBaseTerrain(EstateMap* map) {
    if (!map || !map->world) return;
    
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SetTileAt(map->world, x, y, TILE_GRASS);
        }
    }
}

void CreateCentralCourtyard(EstateMap* map) {
    if (!map || !map->world) return;
    
    int startX = (ESTATE_WIDTH - COURTYARD_SIZE) / 2;
    int startY = (ESTATE_HEIGHT - COURTYARD_SIZE) / 2;
    
    for (int y = startY; y < startY + COURTYARD_SIZE; y++) {
        for (int x = startX; x < startX + COURTYARD_SIZE; x++) {
            SetTileAt(map->world, x, y, TILE_FLOOR);
        }
    }
}

void GenerateMainPaths(EstateMap* map) {
    if (!map || !map->world) return;
    
    const int pathWidth = 2;
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    // Generate vertical path
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = centerX - pathWidth/2; x <= centerX + pathWidth/2; x++) {
            if (x >= 0 && x < ESTATE_WIDTH) {
                SetTileAt(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // Generate horizontal path
    for (int x = 0; x < ESTATE_WIDTH; x++) {
        for (int y = centerY - pathWidth/2; y <= centerY + pathWidth/2; y++) {
            if (y >= 0 && y < ESTATE_HEIGHT) {
                SetTileAt(map->world, x, y, TILE_PATH);
            }
        }
    }
}

void PlaceFountain(EstateMap* map) {
    if (!map || !map->world) return;
    
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    SetTileAt(map->world, centerX, centerY, TILE_FOUNTAIN);
}

void CreateGardens(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Create four garden areas in the corners of the courtyard
    int startX = (ESTATE_WIDTH - COURTYARD_SIZE) / 2;
    int startY = (ESTATE_HEIGHT - COURTYARD_SIZE) / 2;
    int gardenSize = COURTYARD_SIZE / 4;
    
    // Place water features in each garden
    for (int i = 0; i < 4; i++) {
        int offsetX = (i % 2) * (COURTYARD_SIZE - gardenSize);
        int offsetY = (i / 2) * (COURTYARD_SIZE - gardenSize);
        
        for (int y = 0; y < gardenSize; y++) {
            for (int x = 0; x < gardenSize; x++) {
                int mapX = startX + offsetX + x;
                int mapY = startY + offsetY + y;
                SetTileAt(map->world, mapX, mapY, TILE_WATER);
            }
        }
    }
}

void AddDecorations(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Add trees along the paths
    for (int y = 1; y < ESTATE_HEIGHT - 1; y += 4) {
        for (int x = 1; x < ESTATE_WIDTH - 1; x += 4) {
            if (GetTileAt(map->world, x, y) == TILE_GRASS) {
                SetTileAt(map->world, x, y, TILE_TREE);
            }
        }
    }
    
    // Add bushes and flowers near the courtyard
    int startX = (ESTATE_WIDTH - COURTYARD_SIZE) / 2;
    int startY = (ESTATE_HEIGHT - COURTYARD_SIZE) / 2;
    
    for (int y = startY - 2; y < startY + COURTYARD_SIZE + 2; y++) {
        for (int x = startX - 2; x < startX + COURTYARD_SIZE + 2; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                if (GetTileAt(map->world, x, y) == TILE_GRASS) {
                    SetTileAt(map->world, x, y, (rand() % 2) ? TILE_BUSH : TILE_FLOWER);
                }
            }
        }
    }
}

void SetSpawnPoints(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Clear existing spawn points
    map->spawnPointCount = 0;
    
    // Add spawn points at path intersections
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    // Cardinal directions
    Vector2 spawnOffsets[] = {
        {-COURTYARD_SIZE, 0},
        {COURTYARD_SIZE, 0},
        {0, -COURTYARD_SIZE},
        {0, COURTYARD_SIZE}
    };
    
    for (int i = 0; i < 4 && map->spawnPointCount < MAX_SPAWN_POINTS; i++) {
        int x = centerX + spawnOffsets[i].x;
        int y = centerY + spawnOffsets[i].y;
        
        if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
            if (GetTileAt(map->world, x, y) == TILE_PATH) {
                map->spawnPoints[map->spawnPointCount] = (Vector2){
                    x * TILE_SIZE + TILE_SIZE/2,
                    y * TILE_SIZE + TILE_SIZE/2
                };
                map->spawnPointCount++;
            }
        }
    }
}

bool IsValidSpawnPoint(const EstateMap* map, Vector2 position) {
    if (!map || !map->world) return false;
    
    // Convert world position to tile coordinates
    int tileX = (int)(position.x / TILE_SIZE);
    int tileY = (int)(position.y / TILE_SIZE);
    
    // Check if the tile is a valid spawn point (path tile)
    return GetTileAt(map->world, tileX, tileY) == TILE_PATH;
}

Vector2 GetRandomSpawnPoint(const EstateMap* map) {
    if (!map || map->spawnPointCount == 0) {
        return (Vector2){0, 0};
    }
    
    int index = rand() % map->spawnPointCount;
    return map->spawnPoints[index];
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