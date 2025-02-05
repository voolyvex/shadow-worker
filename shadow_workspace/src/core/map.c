#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "map.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/world.h"
#include "../../include/map_types.h"
#include "../../include/map_system.h"
#include "../../include/resource_manager.h"

// Internal functions
static void* SafeAlloc(size_t size) {
    void* ptr = calloc(1, size);
    if (!ptr) {
        TraceLog(LOG_ERROR, "Memory allocation failed");
        exit(1);
    }
    return ptr;
}

static bool IsInBounds(int x, int y) {
    return x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT;
}

static int GetIndex(int x, int y) {
    return y * ESTATE_WIDTH + x;
}

// Map tile and object access functions
void SetTile(World* world, int x, int y, TileType type) {
    if (!world || !IsInBounds(x, y)) return;
    world->tiles[GetIndex(x, y)].type = type;
}

void SetMapObjectAt(World* world, int x, int y, ObjectType type) {
    if (!world || !IsInBounds(x, y)) return;
    world->tiles[GetIndex(x, y)].objectType = type;
}

TileType GetTile(const World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return TILE_NONE;
    return world->tiles[GetIndex(x, y)].type;
}

ObjectType GetMapObjectAt(const World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return OBJECT_NONE;
    return world->tiles[GetIndex(x, y)].objectType;
}

// Helper functions
bool IsWalkableGrid(const World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return false;
    TileType type = world->tiles[GetIndex(x, y)].type;
    return type != TILE_WALL && type != TILE_WATER;
}

bool IsWalkable(const World* world, Vector2 position) {
    if (!world) return false;
    
    // Convert world coordinates to grid coordinates
    int gridX = (int)(position.x / TILE_SIZE);
    int gridY = (int)(position.y / TILE_SIZE);
    
    return IsWalkableGrid(world, gridX, gridY);
}

// Core map functions
bool InitMap(World* world) {
    if (!world) return false;
    
    // Initialize map tiles
    world->tiles = (Tile*)SafeAlloc(ESTATE_WIDTH * ESTATE_HEIGHT * sizeof(Tile));
    if (!world->tiles) return false;
    
    // Set default tiles
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SetTile(world, x, y, TILE_GRASS);
            SetMapObjectAt(world, x, y, OBJECT_NONE);
            
            // Initialize tile properties
            int index = GetIndex(x, y);
            world->tiles[index].properties = (TileProperties){
                .isWalkable = true,
                .isDestructible = false,
                .isInteractive = false,
                .friction = 1.0f,
                .resonance = 0.0f,
                .color = GREEN,
                .customProperties = NULL
            };
        }
    }
    
    return true;
}

void UnloadMap(World* world) {
    if (!world || !world->tiles) return;
    
    // Free any custom properties
    for (int i = 0; i < ESTATE_WIDTH * ESTATE_HEIGHT; i++) {
        if (world->tiles[i].properties.customProperties) {
            free((void*)world->tiles[i].properties.customProperties);
        }
    }
    
    free(world->tiles);
    world->tiles = NULL;
}

// Helper function to set custom properties for a tile
void SetTileCustomProperties(World* world, int x, int y, const char* properties) {
    if (!world || !IsInBounds(x, y)) return;
    
    int index = GetIndex(x, y);
    Tile* tile = &world->tiles[index];
    
    // Free existing custom properties if any
    if (tile->properties.customProperties) {
        free((void*)tile->properties.customProperties);
    }
    
    // Set new custom properties
    char* key = _strdup(properties);
    tile->properties.customProperties = key;
    
    // Mark chunk as dirty
    int chunkX = x / CACHE_CHUNK_SIZE;
    int chunkY = y / CACHE_CHUNK_SIZE;
    Vector2 gridPos = {(float)chunkX, (float)chunkY};
    
    CachedChunk* chunk = GetChunk(&world->mapSystem->currentMap->cache, gridPos);
    if (chunk) {
        chunk->isDirty = true;
    }
}