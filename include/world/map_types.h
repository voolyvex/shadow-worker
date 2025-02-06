#ifndef SHADOW_WORKER_MAP_TYPES_H
#define SHADOW_WORKER_MAP_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include "utils/constants.h"

// Map dimensions
#define MAP_WIDTH 100
#define MAP_HEIGHT 100
#define TILE_SIZE 32

// Tile properties
typedef struct {
    bool isWalkable;
    bool isBlocking;
    bool isInteractive;
    float friction;
    float resonance;
    Color color;
} TileProperties;

// Basic tile structure
typedef struct {
    TileType type;
    int objectId;  // ID of object on this tile, if any
} Tile;

// Map structure
typedef struct {
    int width;
    int height;
    Tile* tiles;
    TileProperties* properties;
    bool* collisionMap;
} TileMap;

#endif // SHADOW_WORKER_MAP_TYPES_H 