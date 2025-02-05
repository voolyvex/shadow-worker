#ifndef MAP_H
#define MAP_H

#include <raylib.h>
#include "../../include/world.h"
#include "../../include/map_types.h"
#include "../../include/estate_map.h"

// Core map functions
bool InitMap(World* world);
void UnloadMap(World* world);

// Map tile and object access functions
TileType GetTile(const World* world, int x, int y);
void SetTile(World* world, int x, int y, TileType type);
ObjectType GetMapObjectAt(const World* world, int x, int y);
void SetMapObjectAt(World* world, int x, int y, ObjectType type);

// Helper functions
bool IsWalkable(const World* world, Vector2 position);
bool IsWalkableGrid(const World* world, int x, int y);

// Internal functions - not exposed in header
static bool IsInBounds(int x, int y);
static int GetIndex(int x, int y);
static void* SafeAlloc(size_t size);

#endif // MAP_H 