#include "../include/world.h"
#include "../include/map_types.h"

void GenerateEstateMap(World* world) {
    if (!world) return;
    
    // Fill with basic tiles for testing
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Create a simple pattern for testing
            Tile tile;
            if (x == 0 || x == world->width - 1 || y == 0 || y == world->height - 1) {
                tile = CreateTile(TILE_WALL, OBJECT_NONE);
            } else if (x == world->width/2 && y == world->height/2) {
                tile = CreateTile(TILE_FLOOR, OBJECT_FOUNTAIN);
            } else if ((x + y) % 3 == 0) {
                tile = CreateTile(TILE_PATH, OBJECT_NONE);
            } else {
                tile = CreateTile(TILE_GRASS, OBJECT_NONE);
            }
            SetTileAt(world, x, y, tile);
        }
    }
    
    // Add a spawn point for testing
    Vector2 spawnPoint = {world->width/2.0f * TILE_SIZE, world->height/2.0f * TILE_SIZE};
    AddSpawnPoint(world, spawnPoint);
} 