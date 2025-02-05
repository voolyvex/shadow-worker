#include "../../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "world_gen.h"
#include "room_loader.h"
#include <stdlib.h>
#include <time.h>
#include "../../include/world.h"
#include "../../include/resource_manager.h"

#define MAX_GENERATION_ATTEMPTS 100

typedef struct GenerationNode {
    RoomTemplate* room;
    Vector2 position;
    struct GenerationNode* connections[4]; // North, East, South, West
} GenerationNode;

static World* GenerateBaseWorld(int width, int height) {
    World* world = NULL;
    ResourceManager* resourceManager = GetResourceManager();
    if (!resourceManager) {
        TraceLog(LOG_ERROR, "Failed to get resource manager");
        return NULL;
    }

    world = CreateWorld(width, height, 9.81f, resourceManager);
    if (!world) {
        TraceLog(LOG_ERROR, "Failed to create world");
        return NULL;
    }
    
    // Set dimensions - use explicit casting to avoid warnings
    world->dimensions.x = (float)width * (float)TILE_SIZE;
    world->dimensions.y = (float)height * (float)TILE_SIZE;
    
    // Initialize with empty tiles
    size_t tileCount = (size_t)(width * height);  // Count in tiles, not pixels
    world->tiles = (Tile*)calloc(tileCount, sizeof(Tile));
    if (!world->tiles) {
        DestroyWorld(world);
        return NULL;
    }
    
    // Set all tiles to empty
    for (size_t i = 0; i < tileCount; i++) {
        world->tiles[i].type = TILE_EMPTY;
        world->tiles[i].objectType = OBJECT_NONE;
        world->tiles[i].properties = (TileProperties){
            .isWalkable = true,
            .isDestructible = false,
            .isInteractive = false,
            .friction = 1.0f,
            .resonance = 0.0f,
            .color = WHITE
        };
    }
    
    return world;
}

static bool PlaceRoom(World* world, const RoomTemplate* room, int startX, int startY) {
    if (!world || !room) return false;
    
    // Get world dimensions in tiles - use explicit casting
    int worldWidth = (int)(world->dimensions.x / (float)TILE_SIZE);
    int worldHeight = (int)(world->dimensions.y / (float)TILE_SIZE);
    
    // Validate placement bounds
    if (startX < 0 || startY < 0 || 
        startX + room->width > worldWidth || 
        startY + room->height > worldHeight) {
        return false;
    }
    
    // Copy tiles
    for (int y = 0; y < room->height; y++) {
        for (int x = 0; x < room->width; x++) {
            int index = (startY + y) * worldWidth + (startX + x);
            world->tiles[index].type = room->tiles[y][x];
            if (room->objects[y][x] != OBJECT_NONE) {
                PlaceObject(world, startX + x, startY + y, room->objects[y][x]);
            }
        }
    }
    
    // Add spawn points
    for (int i = 0; i < room->spawnCount; i++) {
        Vector2 spawnPos = {
            (startX + room->spawnPoints[i].x) * TILE_SIZE,
            (startY + room->spawnPoints[i].y) * TILE_SIZE
        };
        AddSpawnPoint(world, spawnPos);
    }
    
    return true;
}

World* GenerateFirstLevel(void) {
    // Load the starting room template
    RoomTemplate* startRoom = LoadRoomTemplate("resources/maps/room_templates/start_room.json");
    if (!startRoom) return NULL;
    
    // Create world with space for multiple rooms
    World* world = GenerateBaseWorld(LEVEL_WIDTH, LEVEL_HEIGHT);
    if (!world) {
        UnloadRoomTemplate(startRoom);
        return NULL;
    }
    
    // Place starting room in center
    int centerX = (LEVEL_WIDTH - startRoom->width) / 2;
    int centerY = (LEVEL_HEIGHT - startRoom->height) / 2;
    
    if (!PlaceRoom(world, startRoom, centerX, centerY)) {
        UnloadWorld(world);
        UnloadRoomTemplate(startRoom);
        return NULL;
    }
    
    // Add connecting rooms
    RoomTemplate* templates[] = {
        LoadRoomTemplate("resources/maps/room_templates/basic_room.json"),
        LoadRoomTemplate("resources/maps/room_templates/combat_room.json"),
        LoadRoomTemplate("resources/maps/room_templates/treasure_room.json")
    };
    
    // Place connected rooms
    for (int i = 0; i < startRoom->connectionCount; i++) {
        Rectangle conn = startRoom->connections[i];
        int roomIndex = rand() % 3;
        
        // Calculate position based on connection
        int roomX = centerX;
        int roomY = centerY;
        
        if (conn.x == 0) roomX -= templates[roomIndex]->width; // West connection
        if (conn.x == startRoom->width - 1) roomX += startRoom->width; // East connection
        if (conn.y == 0) roomY -= templates[roomIndex]->height; // North connection
        if (conn.y == startRoom->height - 1) roomY += startRoom->height; // South connection
        
        PlaceRoom(world, templates[roomIndex], roomX, roomY);
    }
    
    // Cleanup
    UnloadRoomTemplate(startRoom);
    for (int i = 0; i < 3; i++) {
        UnloadRoomTemplate(templates[i]);
    }
    
    return world;
}

World* GenerateWorld(int width, int height, float gravity) {
    ResourceManager* resourceManager = GetResourceManager();
    if (!resourceManager) {
        TraceLog(LOG_ERROR, "Failed to get resource manager");
        return NULL;
    }

    World* world = CreateWorld(width, height, gravity, resourceManager);
    if (!world) {
        TraceLog(LOG_ERROR, "Failed to create world");
        return NULL;
    }

    // Initialize world properties
    world->gravity = gravity;
    world->width = width;
    world->height = height;
    world->resourceManager = resourceManager;

    return world;
} 