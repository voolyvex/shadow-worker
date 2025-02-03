#ifndef CONSTANTS_H
#define CONSTANTS_H

// Map dimensions
#define ESTATE_WIDTH 100
#define ESTATE_HEIGHT 100
#define LEVEL_WIDTH 128
#define LEVEL_HEIGHT 128
#define COURTYARD_SIZE 16

// Tile size
#define TILE_SIZE 32

// Tile types
#ifdef __cplusplus
extern "C" {
#endif

typedef enum TileType {
    TILE_NONE = 0,
    TILE_EMPTY,
    TILE_FLOOR,
    TILE_WALL,
    TILE_DOOR,
    TILE_GRASS,
    TILE_PATH,
    TILE_WATER,
    TILE_COLUMN,
    TILE_TREE,
    TILE_BUSH,
    TILE_FLOWER,
    TILE_FOUNTAIN,
    TILE_STATUE,
    TILE_COUNT
} TileType;

// Ensure TileType is available for array sizes
#define TILE_TYPE_COUNT (TILE_COUNT - 1)

#ifdef __cplusplus
}
#endif

// Game constants
#define MAX_ENTITIES 1000
#define MAX_SPAWN_POINTS 16
#define MAX_ROOM_TEMPLATES 32

// Window dimensions
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Physics constants
#define GRAVITY 9.81f
#define PLAYER_SPEED 200.0f
#define PLAYER_JUMP_FORCE 400.0f

// Resource paths
#define TILESET_PATH "resources/maps/tileset_main.png"
#define FONT_PATH "resources/fonts/main.ttf"

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100

#endif // CONSTANTS_H 