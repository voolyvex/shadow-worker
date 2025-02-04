#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
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
#include "../include/warning_suppression.h"
#include "../include/entity_pool.h"
#include "../include/resource_manager.h"

END_EXTERNAL_WARNINGS

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

BEGIN_EXTERNAL_WARNINGS

// Function declarations with proper parameter lists
static void UpdateWorldState(World* world, float deltaTime);
static void RenderWorld(const World* world);
EntityPool* CreateEntityPool(size_t initialCapacity);

World* CreateWorld(int width, int height, float gravity, ResourceManager* resourceManager) {
    World* world = (World*)calloc(1, sizeof(World));
    if (!world) return NULL;
    
    // Initialize basic properties
    world->width = width;
    world->height = height;
    world->gravity = gravity;
    world->friction = 0.8f;
    world->tileSize = TILE_SIZE;
    world->dimensions = (Vector2){ width * TILE_SIZE, height * TILE_SIZE };
    world->resourceManager = resourceManager;
    
    // Allocate tiles
    world->tiles = (Tile*)calloc(width * height, sizeof(Tile));
    if (!world->tiles) {
        free(world);
        return NULL;
    }
    
    // Initialize tile properties
    world->tileProperties = (TileProperties*)calloc(width * height, sizeof(TileProperties));
    if (!world->tileProperties) {
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    // Initialize entity pool
    world->entityPool = CreateEntityPool((size_t)MAX_ENTITIES);
    if (!world->entityPool) {
        free(world->tileProperties);
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    // Initialize camera
    world->camera = (Camera2D){
        .target = (Vector2){ 0, 0 },
        .offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f },
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    
    // Load textures
    world->textures.tilesetTexture = LoadTexture("resources/tileset.png");
    world->textures.playerTexture = LoadTexture("resources/player.png");
    world->textures.npcTexture = LoadTexture("resources/npc.png");
    
    return world;
}

void DestroyWorld(World* world) {
    if (!world) return;
    
    // Unload textures
    UnloadTexture(world->textures.tilesetTexture);
    UnloadTexture(world->textures.playerTexture);
    UnloadTexture(world->textures.npcTexture);
    
    // Free memory
    if (world->entityPool) DestroyEntityPool(world->entityPool);
    if (world->tileProperties) free(world->tileProperties);
    if (world->tiles) free(world->tiles);
    
    free(world);
}

void SetTileAt(World* world, int x, int y, Tile tile) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) return;
    world->tiles[y * world->width + x] = tile;
}

Tile GetTileAt(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return (Tile){ TILE_NONE, OBJECT_NONE };
    }
    return world->tiles[y * world->width + x];
}

void SetTile(World* world, int x, int y, TileType tileType) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) return;
    world->tiles[y * world->width + x].type = tileType;
}

TileType GetTile(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return TILE_NONE;
    }
    return world->tiles[y * world->width + x].type;
}

bool IsWalkable(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return false;
    }
    
    TileType type = world->tiles[y * world->width + x].type;
    return type != TILE_WALL && type != TILE_WATER;
}

void AddSpawnPoint(World* world, Vector2 position) {
    if (!world || world->spawnCount >= MAX_SPAWN_POINTS) return;
    world->spawnPoints[world->spawnCount++] = position;
}

Vector2 GetRandomSpawnPoint(World* world) {
    if (!world || world->spawnCount == 0) {
        return (Vector2){ 0, 0 };
    }
    return world->spawnPoints[rand() % world->spawnCount];
}

void DrawWorldDebug(World* world) {
    if (!world) return;
    
    // Draw tile grid
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            Rectangle rect = {
                x * world->tileSize,
                y * world->tileSize,
                world->tileSize,
                world->tileSize
            };
            DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, GRAY);
        }
    }
    
    // Draw spawn points
    for (int i = 0; i < world->spawnCount; i++) {
        DrawCircle(world->spawnPoints[i].x, world->spawnPoints[i].y, 5, RED);
    }
}

void DrawCollisionDebug(World* world) {
    if (!world || !world->entityPool) return;
    
    // Draw entity collision boxes
    for (size_t i = 0; i < world->entityPool->count; i++) {
        Entity* entity = &world->entityPool->entities[i];
        if (!entity->active) continue;
        
        DrawRectangleLinesEx(entity->collider, 1, GREEN);
    }
}

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
    state->entityPool = CreateEntityPool((size_t)MAX_ENTITIES);
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

void UnloadWorld(World* world) {
    if (!world) return;
    
    // Unload map system
    if (world->mapSystem) {
        DestroyMapSystem(world->mapSystem);
        world->mapSystem = NULL;
    }
    
    // Unload entity pool
    if (world->entityPool) {
        DestroyEntityPool(world->entityPool);
        world->entityPool = NULL;
    }
    
    // Unload resource manager
    if (world->resourceManager) {
        DestroyResourceManager(world->resourceManager);
        world->resourceManager = NULL;
    }
    
    // Free world structure
    free(world);
}

END_EXTERNAL_WARNINGS