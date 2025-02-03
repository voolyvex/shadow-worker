#ifndef ROOM_LOADER_H
#define ROOM_LOADER_H

#include "world.h"
#include <raylib.h>

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

#endif // ROOM_LOADER_H 