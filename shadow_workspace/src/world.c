#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/constants.h"
#include "../include/world.h"
#include "../include/resource_manager.h"
#include "../include/entity.h"
#include "../include/entity_types.h"
#include "../include/logger.h"
#include "core/map.h"
#include "map_system.h"

// Camera configuration
#define CAMERA_OFFSET_X (WINDOW_WIDTH / 2.0f)
#define CAMERA_OFFSET_Y (WINDOW_HEIGHT / 2.0f)
#define CAMERA_LERP_SPEED 0.1f

// Initialize tile properties
static const TileProperties defaultTileProperties[8] = {
    {true, false, false, 1.0f, 0.0f, {255, 255, 255, 255}},   // Empty
    {true, false, false, 1.0f, 0.0f, {128, 128, 128, 255}},   // Floor
    {false, true, false, 0.0f, 0.0f, {64, 64, 64, 255}},      // Wall
    {true, false, true, 1.0f, 0.5f, {139, 69, 19, 255}},      // Door
    {true, false, false, 0.8f, 0.2f, {0, 255, 0, 255}},       // Grass
    {true, false, false, 1.0f, 0.0f, {245, 245, 220, 255}},   // Path
    {false, false, false, 0.0f, 1.0f, {0, 0, 255, 255}},      // Water
    {false, true, false, 0.0f, 0.0f, {128, 128, 128, 255}}    // Column
};

// Forward declarations
static bool IsInCircle(int centerX, int centerY, int x, int y, float radius);

WorldState* CreateWorldState(void) {
    WorldState* state = (WorldState*)calloc(1, sizeof(WorldState));
    if (!state) return NULL;
    
    // Initialize world
    state->world = (World*)calloc(1, sizeof(World));
    if (!state->world) {
        free(state);
        return NULL;
    }
    
    // Initialize world properties
    state->world->dimensions = (Vector2){WORLD_WIDTH * TILE_SIZE, WORLD_HEIGHT * TILE_SIZE};
    state->world->gravity = GRAVITY;
    state->world->friction = 0.8f;
    state->world->spawnCount = 0;
    
    // Initialize camera
    state->camera = (Camera2D){
        .offset = (Vector2){CAMERA_OFFSET_X, CAMERA_OFFSET_Y},
        .target = (Vector2){0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    
    // Initialize resource manager
    state->textureManager = CreateResourceManager();
    if (!state->textureManager) {
        free(state->world);
        free(state);
        return NULL;
    }
    
    // Initialize entity pool
    state->entityPool = CreateEntityPool();
    if (!state->entityPool) {
        DestroyResourceManager(state->textureManager);
        free(state->world);
        free(state);
        return NULL;
    }
    
    // Initialize component registry
    state->registry = CreateComponentRegistry();
    if (!state->registry) {
        DestroyEntityPool(state->entityPool);
        DestroyResourceManager(state->textureManager);
        free(state->world);
        free(state);
        return NULL;
    }
    
    // Initialize map system
    state->mapSystem = CreateMapSystem();
    if (!state->mapSystem) {
        DestroyComponentRegistry(state->registry);
        DestroyEntityPool(state->entityPool);
        DestroyResourceManager(state->textureManager);
        free(state->world);
        free(state);
        return NULL;
    }
    
    return state;
}

void DestroyWorldState(WorldState* state) {
    if (!state) return;
    
    if (state->mapSystem) {
        DestroyMapSystem(state->mapSystem);
    }
    
    if (state->registry) {
        DestroyComponentRegistry(state->registry);
    }
    
    if (state->entityPool) {
        DestroyEntityPool(state->entityPool);
    }
    
    if (state->textureManager) {
        DestroyResourceManager(state->textureManager);
    }
    
    if (state->world) {
        free(state->world);
    }
    
    free(state);
}

void UnloadWorldState(WorldState* state) {
    if (!state) return;
    
    if (state->mapSystem) {
        DestroyMapSystem(state->mapSystem);
        state->mapSystem = NULL;
    }
    
    if (state->registry) {
        DestroyComponentRegistry(state->registry);
        state->registry = NULL;
    }
    
    if (state->entityPool) {
        DestroyEntityPool(state->entityPool);
        state->entityPool = NULL;
    }
    
    if (state->textureManager) {
        DestroyResourceManager(state->textureManager);
        state->textureManager = NULL;
    }
    
    if (state->world) {
        free(state->world);
        state->world = NULL;
    }
}

void DestroyWorldStateAndResources(WorldState* state) {
    if (!state) return;
    
    if (state->mapSystem) {
        DestroyMapSystem(state->mapSystem);
    }
    
    if (state->registry) {
        DestroyComponentRegistry(state->registry);
    }
    
    if (state->entityPool) {
        DestroyEntityPool(state->entityPool);
    }
    
    if (state->textureManager) {
        UnloadAllResources(state->textureManager);
        DestroyResourceManager(state->textureManager);
    }
    
    if (state->world) {
        free(state->world);
    }
    
    free(state);
}

void UpdateWorld(WorldState* state, float deltaTime) {
    if (!state || !state->world) return;

    // Update entity pool
    UpdateEntityPool(state->entityPool, state->world, deltaTime);

    // Update map system
    UpdateMapSystem(state->mapSystem, deltaTime);
}

void DrawWorld(WorldState* state) {
    if (!state || !state->world) return;
    
    // Begin 2D camera mode
    BeginMode2D(state->camera);
    
    // Draw map
    DrawMapSystem(state->mapSystem);
    
    // Draw entities
    DrawEntityPool(state->entityPool);
    
    EndMode2D();
}

void AddObject(WorldState* state, ObjectType type, Vector2 position) {
    if (!state || !state->mapSystem) return;
    AddMapObject(state->mapSystem, type, position);
}

void RemoveObject(WorldState* state, Vector2 position) {
    if (!state || !state->mapSystem) return;
    RemoveMapObject(state->mapSystem, position);
}

void UpdateObjects(WorldState* state, float deltaTime) {
    if (!state || !state->mapSystem) return;
    UpdateMapObjects(state->mapSystem, deltaTime);
}

void SaveWorldState(WorldState* state, const char* filename) {
    if (!state || !filename) return;
    SaveMapSystem(state->mapSystem, filename);
}

void LoadWorldState(WorldState* state, const char* filename) {
    if (!state || !filename) return;
    LoadMapSystem(state->mapSystem, filename);
}

// Helper function
static bool IsInCircle(int centerX, int centerY, int x, int y, float radius) {
    float dx = (float)(x - centerX);
    float dy = (float)(y - centerY);
    return (dx * dx + dy * dy) <= (radius * radius);
}