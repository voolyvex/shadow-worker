#ifndef SHADOW_WORKER_WORLD_H
#define SHADOW_WORKER_WORLD_H

#include <raylib.h>
#include <stdbool.h>
#include "entity.h"
#include "resource_manager.h"

// World constants
#define MAX_ROOMS 100
#define TILE_SIZE 32
#define ROOM_WIDTH 40
#define ROOM_HEIGHT 30

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

// World structure
typedef struct World {
    Room* rooms;
    int roomCount;
    Room* currentRoom;
    bool isGenerating;
    Vector2 fieldDimensions;
    float* resonanceField;
    float globalResonance;
    float instabilityLevel;
    bool isStable;
    EntityPool* entityPool;
    float deltaTime;
    ResourceManager* resources;
} World;

// Function declarations
World* InitWorld(ResourceManager* resources);
void UpdateWorld(World* world, float deltaTime);
void DrawWorld(World* world);
void UnloadWorld(World* world);

// Room management functions
Room* CreateRoom(World* world, RoomType type, Vector2 position);
void LoadRoom(World* world, Room* room);
void UnloadRoom(World* world, Room* room);
void UpdateRoom(World* world, Room* room);
void DrawRoom(World* world, Room* room);

// Tile management functions
TileType GetTileAt(Room* room, int x, int y);
void SetTileAt(Room* room, int x, int y, TileType type);
bool IsTileSolid(TileType type);
Vector2 GetTilePosition(int x, int y);

// Room connection and pathfinding
void ConnectRooms(Room* room1, Room* room2, int direction);
bool FindPath(Room* room, Vector2 start, Vector2 end, Vector2* path, int* pathLength);
float GetPathCost(Room* room, Vector2 start, Vector2 end);

// Debug functions
void DebugDrawWorld(World* world);

#endif // SHADOW_WORKER_WORLD_H 