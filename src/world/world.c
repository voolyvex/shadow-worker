#include "utils/warning_suppression.h"
#include "world/world.h"
#include "world/map_system.h"
#include "utils/debug.h"
#include "utils/constants.h"
#include <stdlib.h>
#include <string.h>

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>
#include <time.h>

// Project includes
#include "../../include/resource/resource_manager.h"
#include "../../include/entities/entity.h"
#include "../../include/entities/entity_types.h"
#include "../../include/utils/logger.h"
#include "../../include/core/map.h"
#include "../../include/resource/texture_atlas.h"

END_EXTERNAL_WARNINGS

#include "../../include/texture_atlas.h"
#include "../../include/resource_manager.h"

#define MAX_TILES_PER_ATLAS 256
#define ATLAS_PADDING 1

// Camera configuration
#define CAMERA_OFFSET_X (WINDOW_WIDTH / 2.0f)
#define CAMERA_OFFSET_Y (WINDOW_HEIGHT / 2.0f)
#define CAMERA_LERP_SPEED 0.1f

typedef struct {
    Rectangle region;
    const char* name;
} AtlasEntry;

typedef struct {
    RenderTexture2D atlas;
    AtlasEntry* entries;
    int entryCount;
    int width;
    int height;
    int tileSize;
} TextureAtlas;

static TextureAtlas* currentAtlas = NULL;

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
    World* world = (World*)calloc(1, sizeof(World));  // Use calloc to zero-initialize all fields
    if (!world) return NULL;
    
    world->width = width;
    world->height = height;
    world->gravity = gravity;
    world->friction = 1.0f;  // Set default friction
    world->tileSize = TILE_SIZE;
    world->resourceManager = resourceManager;
    
    // Initialize tiles array
    world->tiles = (Tile*)calloc(width * height, sizeof(Tile));
    if (!world->tiles) {
        free(world);
        return NULL;
    }
    
    // Initialize all tiles to empty
    for (int i = 0; i < width * height; i++) {
        world->tiles[i] = CreateTile(TILE_EMPTY, OBJECT_NONE);
    }
    
    // Initialize dimensions
    world->dimensions = (Vector2){ width * TILE_SIZE, height * TILE_SIZE };
    
    // Initialize camera
    world->camera = (Camera2D){
        .target = (Vector2){ 0, 0 },
        .offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f },
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    
    // Initialize entity pool
    world->entityPool = CreateEntityPool(MAX_ENTITIES);  // Initialize with default capacity
    if (!world->entityPool) {
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    // Set up entity pool's world and registry references
    world->entityPool->world = world;
    world->entityPool->registry = &world->registry;  // Point to the world's registry
    
    // Initialize component registry
    InitializeComponentRegistry(&world->registry);
    
    // Initialize spawn points
    world->spawnPointCount = 0;
    memset(world->spawnPoints, 0, sizeof(world->spawnPoints));
    
    return world;
}

void DestroyWorld(World* world) {
    if (!world) return;
    
    // Free memory
    if (world->entityPool) DestroyEntityPool(world->entityPool);
    if (world->tileProperties) free(world->tileProperties);
    if (world->tiles) free(world->tiles);
    
    // Note: Don't destroy the resource manager here as it's managed externally
    
    free(world);
}

void SetTileAt(World* world, int x, int y, Tile tile) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) return;
    world->tiles[y * world->width + x] = tile;
}

Tile GetTileAt(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) {
        Tile emptyTile = {TILE_NONE, OBJECT_NONE};
        return emptyTile;
    }
    return world->tiles[y * world->width + x];
}

void SetTile(World* world, int x, int y, TileType tileType) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) return;
    Tile* tile = &world->tiles[y * world->width + x];
    tile->type = tileType;
}

TileType GetTile(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return TILE_NONE;
    }
    return world->tiles[y * world->width + x].type;
}

bool IsWalkable(World* world, Vector2 position) {
    // Convert world coordinates to tile coordinates
    int tileX = (int)(position.x / TILE_SIZE);
    int tileY = (int)(position.y / TILE_SIZE);
    
    if (!world || !world->tiles || tileX < 0 || tileX >= world->width || tileY < 0 || tileY >= world->height) {
        return false;
    }
    
    TileType type = world->tiles[tileY * world->width + tileX].type;
    return type != TILE_WALL && type != TILE_WATER;
}

void AddSpawnPoint(World* world, Vector2 position) {
    if (!world || world->spawnPointCount >= MAX_SPAWN_POINTS) return;
    world->spawnPoints[world->spawnPointCount++] = position;
}

Vector2 GetRandomSpawnPoint(World* world) {
    if (!world || world->spawnPointCount == 0) {
        return (Vector2){ 0, 0 };
    }
    return world->spawnPoints[rand() % world->spawnPointCount];
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
    for (int i = 0; i < world->spawnPointCount; i++) {
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
    state->world->spawnPointCount = 0;
    
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