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
TileType GetMapTileAt(const World* world, int x, int y);
void SetMapTileAt(World* world, int x, int y, TileType type);
ObjectType GetMapObjectAt(const World* world, int x, int y);
void SetMapObjectAt(World* world, int x, int y, ObjectType type);

#endif // MAP_H 