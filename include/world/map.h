#ifndef SHADOW_WORKER_MAP_H
#define SHADOW_WORKER_MAP_H

#include "utils/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS
#include <raylib.h>
END_EXTERNAL_WARNINGS

#include "world/world.h"
#include "world/map_types.h"
#include "world/map_system.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // SHADOW_WORKER_MAP_H 