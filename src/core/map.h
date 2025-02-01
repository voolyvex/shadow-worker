#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "../../include/world.h"

// Macros for accessing tiles and objects
#define GET_TILE(map, x, y) GetTileAt((map)->world, (x), (y))
#define SET_TILE(map, x, y, type) SetTileAt((map)->world, (x), (y), (type))
#define GET_OBJECT(map, x, y) GetObjectAt((map)->world, (x), (y))
#define SET_OBJECT(map, x, y, type) SetObjectAt((map)->world, (x), (y), (type))

// Core map functions
EstateMap* CreateEstateMap(void);
void DestroyEstateMap(EstateMap* map);

// Map generation functions
void GenerateEstateMap(EstateMap* map);

// Drawing functions
void DrawEstateMap(const EstateMap* map);

// Utility functions
#define IsValidSpawnPoint(map, position) IsValidSpawnPoint((map)->world, (position))
#define GetRandomSpawnPoint(map) GetRandomSpawnPoint((map)->world)

#endif // MAP_H 