#ifndef SHADOW_WORKER_MAP_H
#define SHADOW_WORKER_MAP_H

#include <raylib.h>
#include <stdbool.h>
#include "../../include/world.h"

// Essential dimensions and constraints
#define MAX_OBJECTS 256
#define MAX_SPAWN_POINTS 32
#define COURTYARD_SIZE 16

// Tile access macros
#define GET_TILE(map, x, y) GetTileAt(map->world, x, y)
#define GET_OBJECT(map, x, y) GetObjectAt(map->world, x, y)

// Object placement types
typedef enum {
    OBJECT_NONE = 0,
    OBJECT_TREE = 1,
    OBJECT_BUSH = 2,
    OBJECT_FLOWER = 3,
    OBJECT_FOUNTAIN = 4
} ObjectType;

// Primary map structure
typedef struct {
    World* world;              // Reference to the world system
    Vector2* spawnPoints;      // NPC spawn locations
    int spawnPointCount;
    Texture2D tileset;         // Tileset texture for map rendering
} EstateMap;

// Core map functions
EstateMap* CreateEstateMap(World* world);
void UnloadEstateMap(EstateMap* map);
void GenerateEstate(EstateMap* map);
void DrawEstateMap(const EstateMap* map);

// Generation functions
void InitializeBaseTerrain(EstateMap* map);
void CreateCentralCourtyard(EstateMap* map);
void GenerateMainPaths(EstateMap* map);
void PlaceFountain(EstateMap* map);
void CreateGardens(EstateMap* map);
void AddDecorations(EstateMap* map);
void SetSpawnPoints(EstateMap* map);

// Utility functions
bool IsValidSpawnPoint(const EstateMap* map, Vector2 position);
Vector2 GetRandomSpawnPoint(const EstateMap* map);

#endif // SHADOW_WORKER_MAP_H 