#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS
#include <raylib.h>
#include <raymath.h>
END_EXTERNAL_WARNINGS

#include "../../include/estate_map.h"
#include "../../include/world.h"
#include "../../include/map_types.h"
#include "../../include/resource_manager.h"
#include <stdlib.h>

// Internal helper functions
static void CreateGardens(World* world, int centerX, int centerY, int courtyardSize);
static void SetSpawnPoints(EstateMap* map);

// Core estate map functions
EstateMap* CreateEstateMap(void) {
    EstateMap* map = (EstateMap*)malloc(sizeof(EstateMap));
    if (!map) return NULL;
    
    ResourceManager* resourceManager = GetResourceManager();
    if (!resourceManager) {
        free(map);
        return NULL;
    }
    
    map->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resourceManager);
    if (!map->world) {
        free(map);
        return NULL;
    }
    
    map->spawnPointCount = 0;
    
    // Load tileset through resource manager
    const Texture2D* tileset = GetGameTexture(resourceManager, "tileset_main");
    if (!tileset) {
        // Try to load it if not already loaded
        if (!LoadGameTexture(resourceManager, "resources/maps/tileset_main.png", "tileset_main")) {
            DestroyWorld(map->world);
            free(map);
            TraceLog(LOG_ERROR, "Failed to load tileset texture");
            return NULL;
        }
        tileset = GetGameTexture(resourceManager, "tileset_main");
    }
    
    // Store reference to tileset
    map->tileset = *tileset;
    
    return map;
}

void DestroyEstateMap(EstateMap* map) {
    if (!map) return;
    
    // No need to unload tileset here as it's managed by ResourceManager
    DestroyWorld(map->world);
    free(map);
}

void GenerateEstateMap(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Initialize all tiles to grass
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SetTile(map->world, x, y, TILE_GRASS);
            SetMapObjectAt(map->world, x, y, OBJECT_NONE);
        }
    }
    
    // Create central courtyard
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    int courtyardSize = COURTYARD_SIZE;
    
    // Generate courtyard floor
    for (int y = centerY - courtyardSize/2; y <= centerY + courtyardSize/2; y++) {
        for (int x = centerX - courtyardSize/2; x <= centerX + courtyardSize/2; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTile(map->world, x, y, TILE_FLOOR);
            }
        }
    }
    
    // Add courtyard walls
    for (int y = centerY - courtyardSize/2 - 1; y <= centerY + courtyardSize/2 + 1; y++) {
        for (int x = centerX - courtyardSize/2 - 1; x <= centerX + courtyardSize/2 + 1; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                if (x == centerX - courtyardSize/2 - 1 || x == centerX + courtyardSize/2 + 1 ||
                    y == centerY - courtyardSize/2 - 1 || y == centerY + courtyardSize/2 + 1) {
                    SetTile(map->world, x, y, TILE_WALL);
                }
            }
        }
    }
    
    // Create main pathways
    // North path
    for (int y = 0; y < centerY - courtyardSize/2; y++) {
        for (int x = centerX - 1; x <= centerX + 1; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTile(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // South path
    for (int y = centerY + courtyardSize/2 + 1; y < ESTATE_HEIGHT; y++) {
        for (int x = centerX - 1; x <= centerX + 1; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTile(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // East path
    for (int x = centerX + courtyardSize/2 + 1; x < ESTATE_WIDTH; x++) {
        for (int y = centerY - 1; y <= centerY + 1; y++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTile(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // West path
    for (int x = 0; x < centerX - courtyardSize/2; x++) {
        for (int y = centerY - 1; y <= centerY + 1; y++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTile(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // Place central fountain
    SetMapObjectAt(map->world, centerX, centerY, OBJECT_FOUNTAIN);
    
    // Create gardens
    CreateGardens(map->world, centerX, centerY, courtyardSize);
    
    // Set spawn points
    SetSpawnPoints(map);
}

void DrawEstateMap(const EstateMap* map) {
    if (!map || !map->world) return;
    
    // Draw tiles
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            // Calculate screen position
            Vector2 position = { x * TILE_SIZE, y * TILE_SIZE };
            
            // Get tile and object types
            TileType tileType = GetTile(map->world, x, y);
            ObjectType objectType = GetMapObjectAt(map->world, x, y);
            
            // Draw tile
            Rectangle tileSource = { tileType * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
            Rectangle tileDest = { position.x, position.y, TILE_SIZE, TILE_SIZE };
            DrawTexturePro(map->tileset, tileSource, tileDest, (Vector2){0, 0}, 0.0f, WHITE);
            
            // Draw object if present
            if (objectType != OBJECT_NONE) {
                Rectangle objectSource = { objectType * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE };
                DrawTexturePro(map->tileset, objectSource, tileDest, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}

// Internal helper functions
static void CreateGardens(World* world, int centerX, int centerY, int courtyardSize) {
    if (!world) return;
    
    // Create four garden areas in corners
    int gardenSize = courtyardSize / 3;
    
    // Garden positions relative to courtyard
    Vector2 gardenPositions[] = {
        {centerX - courtyardSize/2 + gardenSize/2, centerY - courtyardSize/2 + gardenSize/2},
        {centerX + courtyardSize/2 - gardenSize/2, centerY - courtyardSize/2 + gardenSize/2},
        {centerX - courtyardSize/2 + gardenSize/2, centerY + courtyardSize/2 - gardenSize/2},
        {centerX + courtyardSize/2 - gardenSize/2, centerY + courtyardSize/2 - gardenSize/2}
    };
    
    // Create each garden
    for (int i = 0; i < 4; i++) {
        int gx = (int)gardenPositions[i].x;
        int gy = (int)gardenPositions[i].y;
        
        // Fill garden area with grass and decorative objects
        for (int y = gy - gardenSize/2; y <= gy + gardenSize/2; y++) {
            for (int x = gx - gardenSize/2; x <= gx + gardenSize/2; x++) {
                if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                    SetTile(world, x, y, TILE_GRASS);
                    
                    // Add random decorative objects
                    if (rand() % 100 < 30) {
                        ObjectType objects[] = {OBJECT_TREE, OBJECT_BUSH, OBJECT_FLOWER};
                        SetMapObjectAt(world, x, y, objects[rand() % 3]);
                    }
                }
            }
        }
    }
}

static void SetSpawnPoints(EstateMap* map) {
    if (!map) return;
    
    // Clear existing spawn points
    map->spawnPointCount = 0;
    
    // Add spawn points at path entrances
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    // North entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){ (float)centerX * TILE_SIZE, 0.0f };
    
    // South entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){ (float)centerX * TILE_SIZE, (float)(ESTATE_HEIGHT - 1) * TILE_SIZE };
    
    // East entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){ (float)(ESTATE_WIDTH - 1) * TILE_SIZE, (float)centerY * TILE_SIZE };
    
    // West entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){ 0.0f, (float)centerY * TILE_SIZE };
} 