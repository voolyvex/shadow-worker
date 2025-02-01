#include "../include/world.h"
#include "../core/map.h"
#include <stdlib.h>
#include <string.h>
#include <raymath.h>

// Camera configuration
#define CAMERA_OFFSET_X (WINDOW_WIDTH / 2.0f)
#define CAMERA_OFFSET_Y (WINDOW_HEIGHT / 2.0f)
#define CAMERA_LERP_SPEED 0.1f

// Initialize tile properties
static TileProperties defaultTileProperties[TILE_COUNT] = {
    [TILE_GRASS] = {false, false, false, {0, 0, 32, 32}, "tiles/grass"},
    [TILE_PATH] = {false, false, false, {32, 0, 32, 32}, "tiles/path"},
    [TILE_WATER] = {true, true, false, {64, 0, 32, 32}, "tiles/water"},
    [TILE_WALL] = {true, false, false, {96, 0, 32, 32}, "tiles/wall"},
    [TILE_FLOOR] = {false, false, false, {128, 0, 32, 32}, "tiles/floor"},
    [TILE_COLUMN] = {true, false, false, {160, 0, 32, 32}, "buildings/columns"},
    [TILE_TREE] = {true, false, false, {0, 32, 64, 64}, "objects/tree"},
    [TILE_BUSH] = {true, false, false, {64, 32, 32, 32}, "objects/bush"},
    [TILE_FLOWER] = {false, false, false, {96, 32, 32, 32}, "objects/flower"},
    [TILE_FOUNTAIN] = {true, false, true, {128, 32, 64, 64}, "objects/fountain"},
    [TILE_STATUE] = {true, false, true, {0, 0, 32, 32}, "objects/statue"}
};

World* InitWorld(ResourceManager* resources) {
    if (!resources) return NULL;
    
    World* world = (World*)malloc(sizeof(World));
    if (!world) return NULL;
    
    // Initialize world structure
    world->tiles = (TileType*)calloc(ESTATE_WIDTH * ESTATE_HEIGHT, sizeof(TileType));
    if (!world->tiles) {
        free(world);
        return NULL;
    }
    
    world->tileProperties = (TileProperties*)malloc(TILE_COUNT * sizeof(TileProperties));
    if (!world->tileProperties) {
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    // Copy default tile properties
    memcpy(world->tileProperties, defaultTileProperties, TILE_COUNT * sizeof(TileProperties));
    
    // Initialize camera
    world->camera = (Camera2D*)malloc(sizeof(Camera2D));
    if (!world->camera) {
        free(world->tileProperties);
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    world->camera->target = (Vector2){ 0, 0 };
    world->camera->offset = (Vector2){ CAMERA_OFFSET_X, CAMERA_OFFSET_Y };
    world->camera->rotation = 0.0f;
    world->camera->zoom = 1.0f;
    
    world->dimensions = (Vector2){ESTATE_WIDTH * TILE_SIZE, ESTATE_HEIGHT * TILE_SIZE};
    world->bounds = (Rectangle){0, 0, world->dimensions.x, world->dimensions.y};
    world->resources = resources;
    world->globalResonance = 0.0f;
    world->instabilityLevel = 0.0f;
    world->isStable = true;
    world->entityPool = NULL;
    world->deltaTime = 0.0f;
    
    // Create and generate estate map
    EstateMap* estateMap = CreateEstateMap(world);
    if (!estateMap) {
        free(world->camera);
        free(world->tileProperties);
        free(world->tiles);
        free(world);
        return NULL;
    }
    
    // Generate the estate layout
    GenerateEstate(estateMap);
    
    // Store spawn points for later use
    world->spawnPoints = estateMap->spawnPoints;
    world->spawnPointCount = estateMap->spawnPointCount;
    estateMap->spawnPoints = NULL; // Transfer ownership
    
    // Clean up estate map (but keep spawn points)
    UnloadEstateMap(estateMap);
    
    return world;
}

// Helper function to create circular/organic shapes
static bool IsInCircle(int centerX, int centerY, int x, int y, float radius) {
    float dx = x - centerX;
    float dy = y - centerY;
    return (dx * dx + dy * dy) <= radius * radius;
}

void GenerateEstateMap(World* world) {
    if (!world) return;
    
    // Fill with grass by default
    for (int y = 0; y < ESTATE_HEIGHT; y++) {
        for (int x = 0; x < ESTATE_WIDTH; x++) {
            SetTileAt(world, x, y, TILE_GRASS);
        }
    }
    
    // Create main paths
    int centerX = ESTATE_WIDTH / 2;
    int centerY = ESTATE_HEIGHT / 2;
    
    // Create central circular courtyard
    float courtyardRadius = 8.0f;
    for (int y = centerY - 10; y <= centerY + 10; y++) {
        for (int x = centerX - 10; x <= centerX + 10; x++) {
            if (IsInCircle(centerX, centerY, x, y, courtyardRadius)) {
                if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                    SetTileAt(world, x, y, TILE_PATH);
                }
            }
        }
    }
    
    // Add fountain in center
    SetTileAt(world, centerX, centerY, TILE_FOUNTAIN);
    
    // Create winding paths leading to courtyard
    // North path
    for (int y = centerY - 15; y < centerY - 3; y++) {
        int pathWidth = 3;
        int xOffset = (int)(sinf((y - centerY) * 0.2f) * 2.0f);
        for (int x = centerX - pathWidth + xOffset; x <= centerX + pathWidth + xOffset; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTileAt(world, x, y, TILE_PATH);
            }
        }
    }
    
    // South path
    for (int y = centerY + 3; y < centerY + 15; y++) {
        int pathWidth = 3;
        int xOffset = (int)(sinf((y - centerY) * 0.2f) * 2.0f);
        for (int x = centerX - pathWidth + xOffset; x <= centerX + pathWidth + xOffset; x++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTileAt(world, x, y, TILE_PATH);
            }
        }
    }
    
    // East path
    for (int x = centerX + 3; x < centerX + 15; x++) {
        int pathWidth = 3;
        int yOffset = (int)(sinf((x - centerX) * 0.2f) * 2.0f);
        for (int y = centerY - pathWidth + yOffset; y <= centerY + pathWidth + yOffset; y++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTileAt(world, x, y, TILE_PATH);
            }
        }
    }
    
    // West path
    for (int x = centerX - 15; x < centerX - 3; x++) {
        int pathWidth = 3;
        int yOffset = (int)(sinf((x - centerX) * 0.2f) * 2.0f);
        for (int y = centerY - pathWidth + yOffset; y <= centerY + pathWidth + yOffset; y++) {
            if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                SetTileAt(world, x, y, TILE_PATH);
            }
        }
    }
    
    // Add decorative elements
    // Trees in organic clusters
    for (int i = 0; i < 8; i++) {
        float angle = (float)i * (2 * PI / 8);
        int treeX = centerX + (int)(cosf(angle) * 12);
        int treeY = centerY + (int)(sinf(angle) * 12);
        
        if (treeX >= 0 && treeX < ESTATE_WIDTH && treeY >= 0 && treeY < ESTATE_HEIGHT) {
            // Create small cluster of trees
            SetTileAt(world, treeX, treeY, TILE_TREE);
            if (rand() % 2 == 0 && treeX + 1 < ESTATE_WIDTH) {
                SetTileAt(world, treeX + 1, treeY, TILE_TREE);
            }
            if (rand() % 2 == 0 && treeY + 1 < ESTATE_HEIGHT) {
                SetTileAt(world, treeX, treeY + 1, TILE_TREE);
            }
        }
    }
    
    // Add statues at key points
    SetTileAt(world, centerX - 8, centerY, TILE_STATUE);
    SetTileAt(world, centerX + 8, centerY, TILE_STATUE);
    SetTileAt(world, centerX, centerY - 8, TILE_STATUE);
    SetTileAt(world, centerX, centerY + 8, TILE_STATUE);
    
    // Create flower and bush gardens
    for (int i = 0; i < 4; i++) {
        float angle = (float)i * (PI / 2) + PI / 4;
        int gardenCenterX = centerX + (int)(cosf(angle) * 15);
        int gardenCenterY = centerY + (int)(sinf(angle) * 15);
        
        // Create organic garden shapes
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int x = gardenCenterX + dx;
                int y = gardenCenterY + dy;
                
                if (x >= 0 && x < ESTATE_WIDTH && y >= 0 && y < ESTATE_HEIGHT) {
                    if (IsInCircle(gardenCenterX, gardenCenterY, x, y, 4.0f)) {
                        // Alternate between bushes and flowers
                        if ((dx + dy + 8) % 2 == 0) {
                            SetTileAt(world, x, y, TILE_BUSH);
                        } else {
                            SetTileAt(world, x, y, TILE_FLOWER);
                        }
                    }
                }
            }
        }
    }
    
    // Add water features
    for (int i = 0; i < 4; i++) {
        float angle = (float)i * (PI / 2);
        int poolX = centerX + (int)(cosf(angle) * 20);
        int poolY = centerY + (int)(sinf(angle) * 20);
        
        if (poolX >= 3 && poolX < ESTATE_WIDTH - 3 && poolY >= 3 && poolY < ESTATE_HEIGHT - 3) {
            // Create organic pool shape
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    if (IsInCircle(poolX, poolY, poolX + dx, poolY + dy, 2.5f)) {
                        SetTileAt(world, poolX + dx, poolY + dy, TILE_WATER);
                    }
                }
            }
        }
    }
    
    // Add columns along the main paths
    for (int i = 0; i < 8; i++) {
        float angle = (float)i * (2 * PI / 8);
        int columnX = centerX + (int)(cosf(angle) * 25);
        int columnY = centerY + (int)(sinf(angle) * 25);
        
        if (columnX >= 0 && columnX < ESTATE_WIDTH && columnY >= 0 && columnY < ESTATE_HEIGHT) {
            SetTileAt(world, columnX, columnY, TILE_COLUMN);
        }
    }
}

void UpdateWorld(World* world, float deltaTime) {
    if (!world) return;
    world->deltaTime = deltaTime;
    
    // Update camera to follow player if exists
    if (world->entityPool) {
        Entity* player = GetEntityByType(world->entityPool, ENTITY_PLAYER);
        if (player) {
            // Calculate target camera position
            Vector2 targetPos = player->position;
            
            // Apply smooth camera movement using lerp
            world->camera->target.x = Lerp(world->camera->target.x, targetPos.x, CAMERA_LERP_SPEED);
            world->camera->target.y = Lerp(world->camera->target.y, targetPos.y, CAMERA_LERP_SPEED);
            
            // Clamp camera to world bounds
            float minX = world->camera->offset.x / world->camera->zoom;
            float minY = world->camera->offset.y / world->camera->zoom;
            float maxX = world->dimensions.x - minX;
            float maxY = world->dimensions.y - minY;
            
            world->camera->target.x = Clamp(world->camera->target.x, minX, maxX);
            world->camera->target.y = Clamp(world->camera->target.y, minY, maxY);
        }
    }
}

void DrawWorld(World* world) {
    if (!world) return;
    
    BeginMode2D(*world->camera);
    
    // Calculate visible area
    float leftEdge = world->camera->target.x - (WINDOW_WIDTH / 2.0f) / world->camera->zoom;
    float topEdge = world->camera->target.y - (WINDOW_HEIGHT / 2.0f) / world->camera->zoom;
    float rightEdge = leftEdge + WINDOW_WIDTH / world->camera->zoom;
    float bottomEdge = topEdge + WINDOW_HEIGHT / world->camera->zoom;
    
    // Convert to tile coordinates
    int startX = (int)(leftEdge / TILE_SIZE) - 1;
    int startY = (int)(topEdge / TILE_SIZE) - 1;
    int endX = (int)(rightEdge / TILE_SIZE) + 1;
    int endY = (int)(bottomEdge / TILE_SIZE) + 1;
    
    // Clamp to world bounds
    startX = Clamp(startX, 0, ESTATE_WIDTH - 1);
    startY = Clamp(startY, 0, ESTATE_HEIGHT - 1);
    endX = Clamp(endX, 0, ESTATE_WIDTH);
    endY = Clamp(endY, 0, ESTATE_HEIGHT);
    
    // Draw only visible tiles
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            TileType tile = GetTileAt(world, x, y);
            if (tile >= 0 && tile < TILE_COUNT) {
                TileProperties* props = &world->tileProperties[tile];
                Texture2D* texture = GetTexture(props->textureName);
                if (texture) {
                    DrawTexturePro(
                        *texture,
                        props->textureRect,
                        (Rectangle){
                            x * TILE_SIZE,
                            y * TILE_SIZE,
                            props->textureRect.width,
                            props->textureRect.height
                        },
                        (Vector2){0, 0},
                        0.0f,
                        WHITE
                    );
                }
            }
        }
    }
    
    EndMode2D();
}

void UnloadWorld(World* world) {
    if (!world) return;
    
    if (world->tiles) free(world->tiles);
    if (world->tileProperties) free(world->tileProperties);
    if (world->camera) free(world->camera);
    if (world->spawnPoints) free(world->spawnPoints);
    if (world->entityPool) UnloadEntityPool(world->entityPool);
    
    free(world);
}

TileType GetTileAt(World* world, int x, int y) {
    if (!world || !world->tiles || x < 0 || x >= ESTATE_WIDTH || y < 0 || y >= ESTATE_HEIGHT) {
        return TILE_WALL;
    }
    return world->tiles[y * ESTATE_WIDTH + x];
}

void SetTileAt(World* world, int x, int y, TileType type) {
    if (!world || !world->tiles || x < 0 || x >= ESTATE_WIDTH || y < 0 || y >= ESTATE_HEIGHT) {
        return;
    }
    world->tiles[y * ESTATE_WIDTH + x] = type;
}

bool IsTileSolid(World* world, int x, int y) {
    if (!world) return true;
    
    TileType tile = GetTileAt(world, x, y);
    if (tile >= 0 && tile < TILE_COUNT) {
        return world->tileProperties[tile].isSolid;
    }
    return true;
}

Vector2 GetTilePosition(int x, int y) {
    return (Vector2){x * TILE_SIZE, y * TILE_SIZE};
}

void DebugDrawWorld(World* world) {
    if (!world) return;
    
    const int debugInfoX = 10;
    int debugInfoY = 10;
    const int lineHeight = 20;
    
    DrawText(TextFormat("Estate Size: %dx%d", ESTATE_WIDTH, ESTATE_HEIGHT),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Global Resonance: %.2f", world->globalResonance),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Instability: %.2f", world->instabilityLevel),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Stable: %s", world->isStable ? "Yes" : "No"),
             debugInfoX, debugInfoY, 10, WHITE);
}

// Helper function for collision checking
bool CheckCollision(World* world, Rectangle bounds) {
    if (!world) return true;
    
    // Convert bounds to tile coordinates
    int startX = (int)(bounds.x / TILE_SIZE);
    int startY = (int)(bounds.y / TILE_SIZE);
    int endX = (int)((bounds.x + bounds.width) / TILE_SIZE);
    int endY = (int)((bounds.y + bounds.height) / TILE_SIZE);
    
    // Check each tile the bounds intersects with
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (IsTileSolid(world, x, y)) {
                Rectangle tileRect = {
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
                if (CheckCollisionRecs(bounds, tileRect)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool IsValidSpawnPoint(World* world, Vector2 position) {
    if (!world) return false;
    
    // Convert world position to tile coordinates
    int tileX = (int)(position.x / TILE_SIZE);
    int tileY = (int)(position.y / TILE_SIZE);
    
    // Check if the tile is a valid spawn point (path tile)
    return GetTileAt(world, tileX, tileY) == TILE_PATH;
}

Vector2 GetRandomSpawnPoint(World* world) {
    if (!world || world->spawnPointCount == 0) {
        return (Vector2){0, 0};
    }
    
    int index = rand() % world->spawnPointCount;
    return world->spawnPoints[index];
} 