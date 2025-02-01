#ifndef SHADOW_WORKER_MAP_H
#define SHADOW_WORKER_MAP_H

#include <raylib.h>
#include <stdbool.h>
#include "../include/world.h"

// Essential dimensions and constraints
#define TILE_SIZE 32
#define ESTATE_WIDTH 64
#define ESTATE_HEIGHT 48
#define MAX_OBJECTS 256
#define MAX_SPAWN_POINTS 32
#define COURTYARD_SIZE 16

// Core tile enumeration
typedef enum {
    TILE_GRASS = 0,
    TILE_PATH = 1,
    TILE_WATER = 2,
    TILE_WALL = 3,
    TILE_FLOOR = 4
} TileType;

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
    TileType* tiles;           // Base tile layer
    ObjectType* objects;       // Decorative layer
    Rectangle bounds;          // World boundaries
    Vector2* spawnPoints;      // NPC spawn locations
    int spawnPointCount;
    Texture2D tileset;        // Main tileset texture
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
bool IsInBounds(int x, int y);
int GetIndex(int x, int y);
bool IsValidSpawnPoint(const EstateMap* map, Vector2 position);
Vector2 GetRandomSpawnPoint(const EstateMap* map);

// Helper macros
#define GET_TILE(map, x, y) ((map)->tiles[GetIndex((x), (y))])
#define SET_TILE(map, x, y, type) ((map)->tiles[GetIndex((x), (y))] = (type))
#define GET_OBJECT(map, x, y) ((map)->objects[GetIndex((x), (y))])
#define SET_OBJECT(map, x, y, type) ((map)->objects[GetIndex((x), (y))] = (type))

#endif // SHADOW_WORKER_MAP_H 