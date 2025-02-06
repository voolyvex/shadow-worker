#include "utils/warning_suppression.h"
#include "world/map_system.h"
#include "world/world.h"
#include "utils/debug.h"
#include "utils/constants.h"
#include "utils/object_types.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Add size type safety
#define SAFE_SIZE_T(x) ((x) > SIZE_MAX ? SIZE_MAX : (x))
#define SAFE_ARRAY_INDEX(x, max) ((x) >= (max) ? ((max) - 1) : (x))

// Macro for unused parameters
#define UNUSED(x) (void)(x)

MapSystem* CreateMapSystem(void) {
    MapSystem* mapSystem = (MapSystem*)malloc(sizeof(MapSystem));
    if (!mapSystem) return NULL;

    mapSystem->currentMap = NULL;
    mapSystem->collisionGrid = NULL;
    mapSystem->layers = NULL;
    mapSystem->layerCount = 0;
    return mapSystem;
}

void DestroyMapSystem(MapSystem* mapSystem) {
    if (!mapSystem) return;

    // Unload current map with boundary check
    if (mapSystem->currentMap) {
        if (mapSystem->currentMap->tiles) {
            free(mapSystem->currentMap->tiles);
        }
        if (mapSystem->currentMap->properties) {
            free(mapSystem->currentMap->properties);
        }
        free(mapSystem->currentMap);
    }

    // Free collision grid
    if (mapSystem->collisionGrid) {
        if (mapSystem->collisionGrid->cells) {
            free(mapSystem->collisionGrid->cells);
        }
        free(mapSystem->collisionGrid);
    }

    // Free render layers
    if (mapSystem->layers) {
        for (int i = 0; i < mapSystem->layerCount; i++) {
            UnloadTexture(mapSystem->layers[i].background);
            UnloadTexture(mapSystem->layers[i].foreground);
            UnloadTexture(mapSystem->layers[i].objects);
        }
        free(mapSystem->layers);
    }

    free(mapSystem);
}

bool LoadMap(MapSystem* mapSystem, const char* fileName) {
    if (!mapSystem || !fileName) return false;

    // Create new map
    mapSystem->currentMap = (TileMap*)malloc(sizeof(TileMap));
    if (!mapSystem->currentMap) return false;

    // Initialize map properties
    mapSystem->currentMap->width = ESTATE_WIDTH;
    mapSystem->currentMap->height = ESTATE_HEIGHT;
    mapSystem->currentMap->tiles = (Tile*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(Tile));
    mapSystem->currentMap->properties = (TileProperties*)calloc(TILE_COUNT, sizeof(TileProperties));

    if (!mapSystem->currentMap->tiles || !mapSystem->currentMap->properties) {
        DestroyMapSystem(mapSystem);
        return false;
    }

    // Initialize collision grid
    mapSystem->collisionGrid = (CollisionGrid*)malloc(sizeof(CollisionGrid));
    if (!mapSystem->collisionGrid) {
        DestroyMapSystem(mapSystem);
        return false;
    }

    mapSystem->collisionGrid->width = ESTATE_WIDTH;
    mapSystem->collisionGrid->height = ESTATE_HEIGHT;
    mapSystem->collisionGrid->cellSize = TILE_SIZE;
    mapSystem->collisionGrid->cells = (bool*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(bool));

    if (!mapSystem->collisionGrid->cells) {
        DestroyMapSystem(mapSystem);
        return false;
    }

    // Initialize render layers
    mapSystem->layers = (RenderLayer*)calloc(MAX_LAYERS, sizeof(RenderLayer));
    if (!mapSystem->layers) {
        DestroyMapSystem(mapSystem);
        return false;
    }
    mapSystem->layerCount = MAX_LAYERS;

    return true;
}

void UpdateMapSystem(World* world, float deltaTime) {
    UNUSED(deltaTime);
    if (!world) return;
    
    // Update map state if needed
}

void RenderMapLayers(MapSystem* mapSystem) {
    if (!mapSystem || !mapSystem->currentMap || !mapSystem->layers) return;

    for (int i = 0; i < mapSystem->layerCount; i++) {
        RenderLayer* layer = &mapSystem->layers[i];
        if (layer->background.id > 0) {
            DrawTexture(layer->background, 0, 0, WHITE);
        }
        if (layer->foreground.id > 0) {
            DrawTexture(layer->foreground, 0, 0, WHITE);
        }
        if (layer->objects.id > 0) {
            DrawTexture(layer->objects, 0, 0, WHITE);
        }
    }
} 