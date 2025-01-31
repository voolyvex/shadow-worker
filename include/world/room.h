#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>
#include <stdbool.h>
#include "../world.h"

// Room types
typedef enum RoomType {
    ROOM_NORMAL,
    ROOM_CORRIDOR,
    ROOM_SPECIAL,
    ROOM_BOSS
} RoomType;

// Tile types
typedef enum TileType {
    TILE_EMPTY,
    TILE_WALL,
    TILE_FLOOR,
    TILE_DOOR,
    TILE_OBSTACLE
} TileType;

// Room structure
typedef struct Room {
    RoomType type;
    Rectangle bounds;
    TileType* tiles;
    Vector2* spawnPoints;
    int spawnCount;
    bool isGenerated;
    float resonanceLevel;
    struct Room* connections[4]; // North, East, South, West
} Room;

// Function declarations
void InitRoom(Room* room, RoomType type, Vector2 position);
void UpdateRoom(Room* room, float deltaTime);
void DrawRoom(Room* room);
void UnloadRoom(Room* room);

// Tile management
TileType GetTileAt(Room* room, int x, int y);
void SetTileAt(Room* room, int x, int y, TileType type);
bool IsTileSolid(TileType type);
Vector2 GetTilePosition(int x, int y);

#endif // ROOM_H 