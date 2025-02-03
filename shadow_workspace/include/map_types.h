#ifndef MAP_TYPES_H
#define MAP_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include "constants.h"

// Forward declarations
struct World;

// Object types that can be placed in the world
typedef enum {
    OBJECT_NONE = 0,
    OBJECT_TREE,
    OBJECT_BUSH,
    OBJECT_FLOWER,
    OBJECT_FOUNTAIN,
    OBJECT_COUNT
} ObjectType;

// Properties for each tile type
typedef struct {
    bool isWalkable;
    bool isDestructible;
    bool isInteractive;
    float friction;
    float resonance;
    Color color;
} TileProperties;

// Tile structure
typedef struct {
    TileType type;
    ObjectType objectType;
    TileProperties properties;
} Tile;

// Map layer for rendering
typedef struct {
    Texture2D background;
    Texture2D foreground;
    Texture2D objects;
} RenderLayer;

// Collision grid for efficient collision detection
typedef struct {
    int width;
    int height;
    int cellSize;
    bool* cells;
} CollisionGrid;

// Map structure
typedef struct {
    int width;
    int height;
    Tile* tiles;
    TileProperties* properties;
} TileMap;

#endif // MAP_TYPES_H 