#include "../../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

END_EXTERNAL_WARNINGS

#include "map_system.h"
#include <stdlib.h>
#include <string.h>
#include "../include/world.h"

// Add size type safety
#define SAFE_SIZE_T(x) ((x) > SIZE_MAX ? SIZE_MAX : (x))
#define SAFE_ARRAY_INDEX(x, max) ((x) >= (max) ? ((max) - 1) : (x))

// Macro for unused parameters
#define UNUSED(x) (void)(x)

// Function declarations
static void RenderMapLayers(MapSystem* mapSystem);
static void UpdateMapSystem(World* world, float deltaTime);

// Helper functions for chunk management
CachedChunk* GetChunk(ChunkCache* cache, Vector2 gridPos) {
    for (int i = 0; i < cache->chunkCount; i++) {
        if (cache->chunks[i].gridPosition.x == gridPos.x &&
            cache->chunks[i].gridPosition.y == gridPos.y) {
            cache->chunks[i].lastAccessTime = cache->frameCounter;
            return &cache->chunks[i];
        }
    }
    return NULL;
}

static void UpdateChunkTexture(MapSystem* mapSystem, CachedChunk* chunk) {
    if (!chunk->isDirty) return;

    BeginTextureMode(chunk->texture);
    ClearBackground(BLANK);

    // Calculate chunk bounds in tile coordinates
    int startX = (int)chunk->gridPosition.x * CACHE_CHUNK_SIZE;
    int startY = (int)chunk->gridPosition.y * CACHE_CHUNK_SIZE;
    int endX = startX + CACHE_CHUNK_SIZE;
    int endY = startY + CACHE_CHUNK_SIZE;

    // Render tiles within chunk
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            if (x >= 0 && x < mapSystem->currentMap->width &&
                y >= 0 && y < mapSystem->currentMap->height) {
                
                int index = y * mapSystem->currentMap->width + x;
                Tile* tile = &mapSystem->currentMap->tiles[index];
                
                // Draw tile
                Rectangle destRect = {
                    (float)((x - startX) * TILE_SIZE),
                    (float)((y - startY) * TILE_SIZE),
                    (float)TILE_SIZE,
                    (float)TILE_SIZE
                };
                
                DrawRectangleRec(destRect, tile->properties.color);
                
                // Draw objects if present
                if (tile->objectType != OBJECT_NONE) {
                    Color objColor = DARKGREEN;
                    float objSize = TILE_SIZE * 0.6f;
                    Vector2 objPos = {
                        destRect.x + (TILE_SIZE - objSize) / 2,
                        destRect.y + (TILE_SIZE - objSize) / 2
                    };
                    DrawRectangleV(objPos, (Vector2){objSize, objSize}, objColor);
                }
            }
        }
    }
    
    EndTextureMode();
    chunk->isDirty = false;
}

static void CreateChunk(MapSystem* mapSystem, Vector2 gridPos) {
    ChunkCache* cache = &mapSystem->currentMap->cache;
    
    // Evict oldest chunk if cache is full
    if (cache->chunkCount >= MAX_CACHED_CHUNKS) {
        int oldestIdx = 0;
        int oldestTime = cache->chunks[0].lastAccessTime;
        
        for (int i = 1; i < cache->chunkCount; i++) {
            if (cache->chunks[i].lastAccessTime < oldestTime) {
                oldestTime = cache->chunks[i].lastAccessTime;
                oldestIdx = i;
            }
        }
        
        UnloadRenderTexture(cache->chunks[oldestIdx].texture);
        cache->chunks[oldestIdx] = cache->chunks[cache->chunkCount - 1];
        cache->chunkCount--;
    }
    
    // Create new chunk
    CachedChunk chunk = {0};
    chunk.gridPosition = gridPos;
    chunk.texture = LoadRenderTexture(CACHE_CHUNK_SIZE * TILE_SIZE, 
                                    CACHE_CHUNK_SIZE * TILE_SIZE);
    chunk.bounds = (Rectangle){
        gridPos.x * CACHE_CHUNK_SIZE * TILE_SIZE,
        gridPos.y * CACHE_CHUNK_SIZE * TILE_SIZE,
        CACHE_CHUNK_SIZE * TILE_SIZE,
        CACHE_CHUNK_SIZE * TILE_SIZE
    };
    chunk.isDirty = true;
    chunk.lastAccessTime = cache->frameCounter;
    
    cache->chunks[cache->chunkCount++] = chunk;
    UpdateChunkTexture(mapSystem, &cache->chunks[cache->chunkCount - 1]);
}

static void UpdateMapSystem(World* world, float deltaTime) {
    UNUSED(deltaTime);
    if (!world || !world->mapSystem || !world->mapSystem->currentMap) return;
    
    MapSystem* mapSystem = world->mapSystem;
    TileMap* map = mapSystem->currentMap;
    
    // Update viewport bounds based on camera
    Camera2D camera = world->camera;
    map->viewport.bounds = (Rectangle){
        camera.target.x - GetScreenWidth() / (2.0f * camera.zoom),
        camera.target.y - GetScreenHeight() / (2.0f * camera.zoom),
        GetScreenWidth() / camera.zoom,
        GetScreenHeight() / camera.zoom
    };
    
    // Calculate visible chunks
    map->viewport.chunkMin.x = floorf(map->viewport.bounds.x / (CACHE_CHUNK_SIZE * TILE_SIZE));
    map->viewport.chunkMin.y = floorf(map->viewport.bounds.y / (CACHE_CHUNK_SIZE * TILE_SIZE));
    map->viewport.chunkMax.x = ceilf((map->viewport.bounds.x + map->viewport.bounds.width) / 
                                   (CACHE_CHUNK_SIZE * TILE_SIZE));
    map->viewport.chunkMax.y = ceilf((map->viewport.bounds.y + map->viewport.bounds.height) / 
                                   (CACHE_CHUNK_SIZE * TILE_SIZE));
    
    // Update or create visible chunks
    for (float y = map->viewport.chunkMin.y; y <= map->viewport.chunkMax.y; y++) {
        for (float x = map->viewport.chunkMin.x; x <= map->viewport.chunkMax.x; x++) {
            Vector2 gridPos = {x, y};
            CachedChunk* chunk = GetChunk(&map->cache, gridPos);
            
            if (!chunk) {
                CreateChunk(mapSystem, gridPos);
            } else if (chunk->isDirty) {
                UpdateChunkTexture(mapSystem, chunk);
            }
        }
    }
    
    // Increment frame counter
    map->cache.frameCounter++;
}

static void RenderMapLayers(MapSystem* mapSystem) {
    if (!mapSystem || !mapSystem->currentMap) return;
    
    TileMap* map = mapSystem->currentMap;
    
    // Only render chunks within viewport
    for (int i = 0; i < map->cache.chunkCount; i++) {
        CachedChunk* chunk = &map->cache.chunks[i];
        
        if (CheckCollisionRecs(chunk->bounds, map->viewport.bounds)) {
            DrawTextureRec(chunk->texture.texture,
                          (Rectangle){0, 0, chunk->texture.texture.width, -chunk->texture.texture.height},
                          (Vector2){chunk->bounds.x, chunk->bounds.y},
                          WHITE);
        }
    }
}

void DrawMapSystem(MapSystem* mapSystem) {
    if (!mapSystem) return;
    RenderMapLayers(mapSystem);
}

void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position) {
    if (!mapSystem || !mapSystem->currentMap) return;
    
    // Convert position to tile coordinates
    int tileX = (int)(position.x / TILE_SIZE);
    int tileY = (int)(position.y / TILE_SIZE);
    
    // Ensure coordinates are within bounds
    if (tileX < 0 || tileX >= mapSystem->currentMap->width ||
        tileY < 0 || tileY >= mapSystem->currentMap->height) {
        return;
    }
    
    // Add object to tile
    int index = tileY * mapSystem->currentMap->width + tileX;
    mapSystem->currentMap->tiles[index].objectType = type;
    
    // Mark affected chunk as dirty
    Vector2 chunkPos = {
        floorf(tileX / (float)CACHE_CHUNK_SIZE),
        floorf(tileY / (float)CACHE_CHUNK_SIZE)
    };
    CachedChunk* chunk = GetChunk(&mapSystem->currentMap->cache, chunkPos);
    if (chunk) {
        chunk->isDirty = true;
    }
}

void RemoveMapObject(MapSystem* mapSystem, Vector2 position) {
    if (!mapSystem || !mapSystem->currentMap) return;
    
    // Convert position to tile coordinates
    int tileX = (int)(position.x / TILE_SIZE);
    int tileY = (int)(position.y / TILE_SIZE);
    
    // Ensure coordinates are within bounds
    if (tileX < 0 || tileX >= mapSystem->currentMap->width ||
        tileY < 0 || tileY >= mapSystem->currentMap->height) {
        return;
    }
    
    // Remove object from tile
    int index = tileY * mapSystem->currentMap->width + tileX;
    mapSystem->currentMap->tiles[index].objectType = OBJECT_NONE;
    
    // Mark affected chunk as dirty
    Vector2 chunkPos = {
        floorf(tileX / (float)CACHE_CHUNK_SIZE),
        floorf(tileY / (float)CACHE_CHUNK_SIZE)
    };
    CachedChunk* chunk = GetChunk(&mapSystem->currentMap->cache, chunkPos);
    if (chunk) {
        chunk->isDirty = true;
    }
}

void UpdateMapObjects(MapSystem* mapSystem, float deltaTime) {
    if (!mapSystem || !mapSystem->currentMap) return;
    
    // Update object animations or states if needed
    for (int y = 0; y < mapSystem->currentMap->height; y++) {
        for (int x = 0; x < mapSystem->currentMap->width; x++) {
            int index = y * mapSystem->currentMap->width + x;
            Tile* tile = &mapSystem->currentMap->tiles[index];
            
            // Update object based on type
            switch (tile->objectType) {
                case OBJECT_FOUNTAIN:
                    // Update fountain animation
                    tile->properties.resonance = sinf(GetTime() * 2.0f) * 0.5f + 0.5f;
                    break;
                    
                case OBJECT_TORCH:
                    // Update torch flicker
                    tile->properties.resonance = GetRandomValue(80, 100) / 100.0f;
                    break;
                    
                default:
                    break;
            }
        }
    }
}

void SaveMapSystem(MapSystem* mapSystem, const char* filename) {
    if (!mapSystem || !mapSystem->currentMap || !filename) return;
    
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    // Save map dimensions
    fwrite(&mapSystem->currentMap->width, sizeof(int), 1, file);
    fwrite(&mapSystem->currentMap->height, sizeof(int), 1, file);
    
    // Save tiles
    size_t tileCount = mapSystem->currentMap->width * mapSystem->currentMap->height;
    fwrite(mapSystem->currentMap->tiles, sizeof(Tile), tileCount, file);
    
    // Save tile properties
    fwrite(mapSystem->currentMap->properties, sizeof(TileProperties), TILE_COUNT, file);
    
    fclose(file);
}

void LoadMapSystem(MapSystem* mapSystem, const char* filename) {
    if (!mapSystem || !filename) return;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return;
    
    // Create new map if needed
    if (!mapSystem->currentMap) {
        mapSystem->currentMap = (TileMap*)malloc(sizeof(TileMap));
        if (!mapSystem->currentMap) {
            fclose(file);
            return;
        }
    }
    
    // Load map dimensions
    fread(&mapSystem->currentMap->width, sizeof(int), 1, file);
    fread(&mapSystem->currentMap->height, sizeof(int), 1, file);
    
    // Allocate and load tiles
    size_t tileCount = mapSystem->currentMap->width * mapSystem->currentMap->height;
    mapSystem->currentMap->tiles = (Tile*)realloc(mapSystem->currentMap->tiles, tileCount * sizeof(Tile));
    if (mapSystem->currentMap->tiles) {
        fread(mapSystem->currentMap->tiles, sizeof(Tile), tileCount, file);
    }
    
    // Allocate and load tile properties
    mapSystem->currentMap->properties = (TileProperties*)realloc(mapSystem->currentMap->properties, TILE_COUNT * sizeof(TileProperties));
    if (mapSystem->currentMap->properties) {
        fread(mapSystem->currentMap->properties, sizeof(TileProperties), TILE_COUNT, file);
    }
    
    fclose(file);
    
    // Mark all chunks as dirty to force redraw
    for (int i = 0; i < mapSystem->currentMap->cache.chunkCount; i++) {
        mapSystem->currentMap->cache.chunks[i].isDirty = true;
    }
} 