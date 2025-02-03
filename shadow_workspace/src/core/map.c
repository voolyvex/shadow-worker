#include "map.h"
#include "estate_map.h"
#include <stdlib.h>
#include <string.h>
#include <raymath.h>
#include <raylib.h>
#include "../../include/world.h"
#include "../../include/map_types.h"

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

void SetMapTileAt(World* world, int x, int y, TileType type) {
    if (!world || !IsInBounds(x, y)) return;
    world->tiles[GetIndex(x, y)].type = type;
}

void SetMapObjectAt(World* world, int x, int y, ObjectType type) {
    if (!world || !IsInBounds(x, y)) return;
    world->tiles[GetIndex(x, y)].objectType = type;
}

TileType GetMapTileAt(World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return TILE_NONE;
    return world->tiles[GetIndex(x, y)].type;
}

ObjectType GetMapObjectAt(World* world, int x, int y) {
    if (!world || !IsInBounds(x, y)) return OBJECT_NONE;
    return world->tiles[GetIndex(x, y)].objectType;
}

// Core map functions
EstateMap* CreateEstateMap(void) {
    EstateMap* map = (EstateMap*)malloc(sizeof(EstateMap));
    if (!map) return NULL;
    
    ResourceManager* resourceManager = GetResourceManager();
    map->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resourceManager);
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
    DestroyWorld(map->world);
    free(map);
}

void GenerateEstateMap(EstateMap* map) {
    if (!map || !map->world) return;
    
    // Initialize all tiles to grass
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SetMapTileAt(map->world, x, y, TILE_GRASS);
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
            if (IsInBounds(x, y)) {
                SetMapTileAt(map->world, x, y, TILE_FLOOR);
            }
        }
    }
    
    // Add courtyard walls
    for (int y = centerY - courtyardSize/2 - 1; y <= centerY + courtyardSize/2 + 1; y++) {
        for (int x = centerX - courtyardSize/2 - 1; x <= centerX + courtyardSize/2 + 1; x++) {
            if (IsInBounds(x, y)) {
                if (x == centerX - courtyardSize/2 - 1 || x == centerX + courtyardSize/2 + 1 ||
                    y == centerY - courtyardSize/2 - 1 || y == centerY + courtyardSize/2 + 1) {
                    SetMapTileAt(map->world, x, y, TILE_WALL);
                }
            }
        }
    }
    
    // Create main pathways
    // North path
    for (int y = 0; y < centerY - courtyardSize/2; y++) {
        for (int x = centerX - 1; x <= centerX + 1; x++) {
            if (IsInBounds(x, y)) {
                SetMapTileAt(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // South path
    for (int y = centerY + courtyardSize/2 + 1; y < ESTATE_HEIGHT; y++) {
        for (int x = centerX - 1; x <= centerX + 1; x++) {
            if (IsInBounds(x, y)) {
                SetMapTileAt(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // East path
    for (int x = centerX + courtyardSize/2 + 1; x < ESTATE_WIDTH; x++) {
        for (int y = centerY - 1; y <= centerY + 1; y++) {
            if (IsInBounds(x, y)) {
                SetMapTileAt(map->world, x, y, TILE_PATH);
            }
        }
    }
    
    // West path
    for (int x = 0; x < centerX - courtyardSize/2; x++) {
        for (int y = centerY - 1; y <= centerY + 1; y++) {
            if (IsInBounds(x, y)) {
                SetMapTileAt(map->world, x, y, TILE_PATH);
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

// Helper function to create garden areas
void CreateGardens(World* world, int centerX, int centerY, int courtyardSize) {
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
                if (IsInBounds(x, y)) {
                    SetMapTileAt(world, x, y, TILE_GRASS);
                    
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

void SetSpawnPoints(EstateMap* map) {
    if (!map) return;
    
    // Clear existing spawn points
    map->spawnPointCount = 0;
    
    // Add spawn points at path entrances
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    int courtyardSize = COURTYARD_SIZE;
    
    // North entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){centerX, 0};
    
    // South entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){centerX, ESTATE_HEIGHT - 1};
    
    // East entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){ESTATE_WIDTH - 1, centerY};
    
    // West entrance
    map->spawnPoints[map->spawnPointCount++] = (Vector2){0, centerY};
}

void DrawEstateMap(const EstateMap* map) {
    if (!map) return;

    // Calculate visible area
    Camera2D camera = map->world->camera;
    float screenLeft = camera.target.x - (GetScreenWidth() / 2.0f) / camera.zoom;
    float screenTop = camera.target.y - (GetScreenHeight() / 2.0f) / camera.zoom;
    float screenRight = screenLeft + GetScreenWidth() / camera.zoom;
    float screenBottom = screenTop + GetScreenHeight() / camera.zoom;

    // Convert to tile coordinates
    int startX = (int)(screenLeft / TILE_SIZE);
    int startY = (int)(screenTop / TILE_SIZE);
    int endX = (int)(screenRight / TILE_SIZE) + 1;
    int endY = (int)(screenBottom / TILE_SIZE) + 1;

    // Clamp to map bounds
    startX = Clamp(startX, 0, ESTATE_WIDTH - 1);
    startY = Clamp(startY, 0, ESTATE_HEIGHT - 1);
    endX = Clamp(endX, 0, ESTATE_WIDTH);
    endY = Clamp(endY, 0, ESTATE_HEIGHT);

    // Draw visible tiles
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            float drawX = (float)(x * TILE_SIZE);
            float drawY = (float)(y * TILE_SIZE);

            // Draw tile
            TileType tile = GetMapTileAt(map->world, x, y);
            Color tileColor = WHITE;  // Default color
            if (tile == TILE_WALL) tileColor = GRAY;
            else if (tile == TILE_PATH) tileColor = BEIGE;
            else if (tile == TILE_GRASS) tileColor = GREEN;
            DrawRectangle((int)drawX, (int)drawY, TILE_SIZE, TILE_SIZE, tileColor);

            // Draw object if present
            ObjectType object = GetMapObjectAt(map->world, x, y);
            if (object != OBJECT_NONE) {
                Color objectColor = DARKGREEN;  // Default color for objects
                switch (object) {
                    case OBJECT_TREE: objectColor = DARKGREEN; break;
                    case OBJECT_BUSH: objectColor = GREEN; break;
                    case OBJECT_FLOWER: objectColor = PINK; break;
                    case OBJECT_FOUNTAIN: objectColor = BLUE; break;
                    default: break;
                }
                float objectSize = TILE_SIZE * 0.6f;
                float objectX = drawX + (TILE_SIZE - objectSize) / 2;
                float objectY = drawY + (TILE_SIZE - objectSize) / 2;
                DrawRectangle((int)objectX, (int)objectY, (int)objectSize, (int)objectSize, objectColor);
            }
        }
    }
}

bool InitMap(World* world) {
    if (!world) return false;
    
    // Allocate memory for tiles
    size_t tileCount = world->width * world->height;
    world->tiles = (Tile*)SafeAlloc(tileCount * sizeof(Tile));
    
    // Initialize tiles with default values
    for (size_t i = 0; i < tileCount; i++) {
        world->tiles[i].type = TILE_FLOOR;
        world->tiles[i].objectType = OBJECT_NONE;
    }
    
    return true;
}

void UnloadMap(World* world) {
    if (!world) return;
    
    if (world->tiles) {
        free(world->tiles);
        world->tiles = NULL;
    }
}

bool IsWalkable(World* world, Vector2 position) {
    if (!world || !world->tiles) return false;
    
    int tileX = (int)position.x;
    int tileY = (int)position.y;
    
    if (!IsInBounds(tileX, tileY)) return false;
    
    Tile tile = world->tiles[GetIndex(tileX, tileY)];
    return tile.properties.isWalkable;
}