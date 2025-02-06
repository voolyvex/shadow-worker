#ifndef SHADOW_WORKER_MAP_SYSTEM_H
#define SHADOW_WORKER_MAP_SYSTEM_H

#include "map_types.h"
#include "utils/object_types.h"
#include <raylib.h>
#include <raymath.h>

// Forward declarations
struct World;

// Collision grid structure
typedef struct {
    bool* cells;
    int width;
    int height;
    int cellSize;
} CollisionGrid;

// Render layer structure
typedef struct {
    Texture2D background;
    Texture2D foreground;
    Texture2D objects;
    Rectangle sourceRect;
    Rectangle destRect;
    float opacity;
    bool isVisible;
} RenderLayer;

// Map System structure
typedef struct {
    TileMap* currentMap;
    CollisionGrid* collisionGrid;
    RenderLayer* layers;
    int layerCount;
} MapSystem;

// Map system management functions
MapSystem* CreateMapSystem(void);
void DestroyMapSystem(MapSystem* mapSystem);
bool LoadMap(MapSystem* mapSystem, const char* fileName);
void UpdateMapState(MapSystem* mapSystem, float deltaTime);
void RenderMapLayers(MapSystem* mapSystem);

// Additional map system functions
void UpdateMapSystem(struct World* world, float deltaTime);
void DrawMapSystem(MapSystem* mapSystem);
void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position);
void RemoveMapObject(MapSystem* mapSystem, Vector2 position);
void UpdateMapObjects(MapSystem* mapSystem, float deltaTime);
void SaveMapSystem(MapSystem* mapSystem, const char* filename);
void LoadMapSystem(MapSystem* mapSystem, const char* filename);

// Tile management functions
void SetTile(MapSystem* mapSystem, Vector2 position, TileType type);
TileType GetTile(const MapSystem* mapSystem, Vector2 position);
bool IsTileWalkable(const MapSystem* mapSystem, Vector2 position);
bool IsTileBlocking(const MapSystem* mapSystem, Vector2 position);

#endif // SHADOW_WORKER_MAP_SYSTEM_H 