#include "map_system.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

// Add size type safety
#define SAFE_SIZE_T(x) ((x) > SIZE_MAX ? SIZE_MAX : (x))
#define SAFE_ARRAY_INDEX(x, max) ((x) >= (max) ? ((max) - 1) : (x))

MapSystem* CreateMapSystem(void) {
    MapSystem* mapSystem = (MapSystem*)malloc(sizeof(MapSystem));
    if (!mapSystem) return NULL;

    memset(mapSystem, 0, sizeof(MapSystem));
    return mapSystem;
}

void DestroyMapSystem(MapSystem* mapSystem) {
    if (!mapSystem) return;

    // Unload current map with boundary check
    if (mapSystem->currentMap) {
        UnloadTexture(mapSystem->currentMap->texture);
        free(mapSystem->currentMap);
    }

    free(mapSystem);
}

bool LoadMap(MapSystem* mapSystem, const char* fileName) {
    if (!mapSystem || !fileName) return false;

    // Load map texture with boundary check
    mapSystem->currentMap = (TileMap*)malloc(sizeof(TileMap));
    if (!mapSystem->currentMap) return false;

    mapSystem->currentMap->texture = LoadTexture(fileName);
    if (!mapSystem->currentMap->texture.id) {
        free(mapSystem->currentMap);
        return false;
    }

    // Initialize collision grid and render layers with safe size
    memset(&mapSystem->collisionGrid, 0, sizeof(CollisionGrid));
    memset(mapSystem->layers, 0, SAFE_SIZE_T(sizeof(RenderLayer) * MAX_LAYERS));

    return true;
}

void UpdateMapState(MapSystem* mapSystem, float deltaTime) {
    if (!mapSystem) return;

    // Update map logic with boundary checks
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        if (mapSystem->layers[i].visible) {
            // Update layer logic here
        }
    }
}

void RenderMapLayers(MapSystem* mapSystem) {
    if (!mapSystem) return;

    // Render map layers with boundary checks
    for (size_t i = 0; i < MAX_LAYERS; i++) {
        if (mapSystem->layers[i].visible) {
            DrawTexture(mapSystem->currentMap->texture, 0, 0, WHITE);
        }
    }
} 