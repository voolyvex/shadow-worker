#ifndef ESTATE_MAP_H
#define ESTATE_MAP_H

#include <raylib.h>
#include "world.h"
#include "map_types.h"

typedef struct EstateMap {
    World* world;
    Texture2D tileset;
    int spawnPointCount;
    Vector2 spawnPoints[MAX_SPAWN_POINTS];
} EstateMap;

// Core map functions
EstateMap* CreateEstateMap(void);
void DestroyEstateMap(EstateMap* map);
void GenerateEstateMap(EstateMap* map);

// Helper functions
void SetMapTileAt(World* world, int x, int y, TileType type);
void SetMapObjectAt(World* world, int x, int y, ObjectType type);
TileType GetMapTileAt(World* world, int x, int y);
ObjectType GetMapObjectAt(World* world, int x, int y);

// Garden generation
void CreateGardens(World* world, int centerX, int centerY, int courtyardSize);
void SetSpawnPoints(EstateMap* map);

#endif // ESTATE_MAP_H 