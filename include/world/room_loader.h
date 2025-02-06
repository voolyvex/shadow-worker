#ifndef SHADOW_WORKER_ROOM_LOADER_H
#define SHADOW_WORKER_ROOM_LOADER_H

#include "utils/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS
#include <raylib.h>
END_EXTERNAL_WARNINGS

#include "world/world.h"
#include "world/map_types.h"

typedef struct RoomTemplate {
    int width;
    int height;
    TileType** tiles;
    ObjectType** objects;
    Vector2* spawnPoints;
    int spawnCount;
    Rectangle* connections;  // Door/connection positions
    int connectionCount;
} RoomTemplate;

// Function declarations
RoomTemplate* LoadRoomTemplate(const char* templatePath);
void UnloadRoomTemplate(RoomTemplate* template);
bool ValidateRoomTemplate(const RoomTemplate* template);
bool CheckConnectionCompatibility(const RoomTemplate* room1, const RoomTemplate* room2);

#endif // SHADOW_WORKER_ROOM_LOADER_H 