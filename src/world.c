#include "../include/world.h"
#include <stdlib.h>
#include <string.h>
#include <raymath.h>

World* InitWorld(ResourceManager* resources) {
    if (!resources) return NULL;
    
    // Allocate world structure
    World* world = (World*)malloc(sizeof(World));
    if (!world) return NULL;
    
    // Initialize world structure
    world->rooms = (Room*)calloc(MAX_ROOMS, sizeof(Room));
    if (!world->rooms) {
        free(world);
        return NULL;
    }
    
    world->roomCount = 0;
    world->currentRoom = NULL;
    world->isGenerating = false;
    world->fieldDimensions = (Vector2){ROOM_WIDTH * TILE_SIZE, ROOM_HEIGHT * TILE_SIZE};
    world->resonanceField = (float*)calloc(ROOM_WIDTH * ROOM_HEIGHT, sizeof(float));
    if (!world->resonanceField) {
        free(world->rooms);
        free(world);
        return NULL;
    }
    
    world->globalResonance = 0.0f;
    world->instabilityLevel = 0.0f;
    world->isStable = true;
    world->entityPool = NULL;
    world->deltaTime = 0.0f;
    world->resources = resources;
    
    // Create initial room
    Room* startRoom = CreateRoom(world, ROOM_NORMAL, (Vector2){0, 0});
    if (!startRoom) {
        free(world->resonanceField);
        free(world->rooms);
        free(world);
        return NULL;
    }
    
    world->currentRoom = startRoom;
    
    return world;
}

void UpdateWorld(World* world, float deltaTime) {
    if (!world) return;
    
    world->deltaTime = deltaTime;
    
    // Update current room
    if (world->currentRoom) {
        UpdateRoom(world, world->currentRoom);
    }
    
    // Update resonance field
    for (int i = 0; i < ROOM_WIDTH * ROOM_HEIGHT; i++) {
        if (world->resonanceField[i] > 0.0f) {
            world->resonanceField[i] = fmaxf(0.0f, world->resonanceField[i] - deltaTime);
        }
    }
    
    // Update global resonance
    float totalResonance = 0.0f;
    for (int i = 0; i < world->roomCount; i++) {
        totalResonance += world->rooms[i].resonanceLevel;
    }
    world->globalResonance = totalResonance / (float)world->roomCount;
    
    // Update instability
    if (world->globalResonance > 0.8f) {
        world->instabilityLevel = fminf(1.0f, world->instabilityLevel + deltaTime * 0.1f);
        if (world->instabilityLevel > 0.9f) {
            world->isStable = false;
        }
    } else {
        world->instabilityLevel = fmaxf(0.0f, world->instabilityLevel - deltaTime * 0.05f);
        if (world->instabilityLevel < 0.1f) {
            world->isStable = true;
        }
    }
}

void DrawWorld(World* world) {
    if (!world || !world->currentRoom) return;
    
    // Draw current room
    DrawRoom(world, world->currentRoom);
    
    // Draw resonance field visualization
    if (world->resonanceField) {
        for (int y = 0; y < ROOM_HEIGHT; y++) {
            for (int x = 0; x < ROOM_WIDTH; x++) {
                float resonance = world->resonanceField[y * ROOM_WIDTH + x];
                if (resonance > 0.0f) {
                    Color color = ColorAlpha(SKYBLUE, resonance * 0.3f);
                    DrawRectangle(
                        (int)(x * TILE_SIZE),
                        (int)(y * TILE_SIZE),
                        TILE_SIZE,
                        TILE_SIZE,
                        color
                    );
                }
            }
        }
    }
    
    // Draw instability effects
    if (world->instabilityLevel > 0.0f) {
        Color instabilityColor = ColorAlpha(RED, world->instabilityLevel * 0.2f);
        DrawRectangle(
            0,
            0,
            (int)world->fieldDimensions.x,
            (int)world->fieldDimensions.y,
            instabilityColor
        );
    }
}

void UnloadWorld(World* world) {
    if (!world) return;
    
    // Free rooms
    if (world->rooms) {
        for (int i = 0; i < world->roomCount; i++) {
            UnloadRoom(world, &world->rooms[i]);
        }
        free(world->rooms);
    }
    
    // Free resonance field
    if (world->resonanceField) {
        free(world->resonanceField);
    }
}

Room* CreateRoom(World* world, RoomType type, Vector2 position) {
    if (!world || world->roomCount >= MAX_ROOMS) return NULL;
    
    Room* room = &world->rooms[world->roomCount++];
    room->type = type;
    room->bounds = (Rectangle){
        position.x,
        position.y,
        ROOM_WIDTH * TILE_SIZE,
        ROOM_HEIGHT * TILE_SIZE
    };
    
    // Allocate tiles
    room->tiles = (TileType*)calloc(ROOM_WIDTH * ROOM_HEIGHT, sizeof(TileType));
    if (!room->tiles) {
        world->roomCount--;
        return NULL;
    }
    
    // Initialize tiles (simple room layout for now)
    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x++) {
            if (x == 0 || x == ROOM_WIDTH-1 || y == 0 || y == ROOM_HEIGHT-1) {
                room->tiles[y * ROOM_WIDTH + x] = TILE_WALL;
            } else {
                room->tiles[y * ROOM_WIDTH + x] = TILE_FLOOR;
            }
        }
    }
    
    room->spawnPoints = NULL;
    room->spawnCount = 0;
    room->isGenerated = true;
    room->resonanceLevel = 0.0f;
    memset(room->connections, 0, sizeof(room->connections));
    
    return room;
}

void LoadRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    // Load room resources here
}

void UnloadRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    if (room->tiles) {
        free(room->tiles);
        room->tiles = NULL;
    }
    
    if (room->spawnPoints) {
        free(room->spawnPoints);
        room->spawnPoints = NULL;
    }
}

void UpdateRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    // Update room state here
}

void DrawRoom(World* world, Room* room) {
    if (!world || !room || !room->tiles) return;
    
    // Draw tiles
    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x++) {
            TileType tile = room->tiles[y * ROOM_WIDTH + x];
            Color color;
            
            switch (tile) {
                case TILE_WALL:
                    color = DARKGRAY;
                    break;
                case TILE_FLOOR:
                    color = LIGHTGRAY;
                    break;
                case TILE_DOOR:
                    color = BROWN;
                    break;
                case TILE_OBSTACLE:
                    color = GRAY;
                    break;
                default:
                    color = BLACK;
                    break;
            }
            
            DrawRectangle(
                (int)(x * TILE_SIZE + room->bounds.x),
                (int)(y * TILE_SIZE + room->bounds.y),
                TILE_SIZE,
                TILE_SIZE,
                color
            );
        }
    }
    
    // Draw resonance level indicator
    if (room->resonanceLevel > 0.0f) {
        Color resonanceColor = ColorAlpha(SKYBLUE, room->resonanceLevel * 0.5f);
        DrawRectangleLinesEx(room->bounds, 2, resonanceColor);
    }
}

TileType GetTileAt(Room* room, int x, int y) {
    if (!room || !room->tiles || x < 0 || x >= ROOM_WIDTH || y < 0 || y >= ROOM_HEIGHT) {
        return TILE_WALL;
    }
    return room->tiles[y * ROOM_WIDTH + x];
}

void SetTileAt(Room* room, int x, int y, TileType type) {
    if (!room || !room->tiles || x < 0 || x >= ROOM_WIDTH || y < 0 || y >= ROOM_HEIGHT) {
        return;
    }
    room->tiles[y * ROOM_WIDTH + x] = type;
}

bool IsTileSolid(TileType type) {
    return type == TILE_WALL || type == TILE_OBSTACLE;
}

Vector2 GetTilePosition(int x, int y) {
    return (Vector2){x * TILE_SIZE, y * TILE_SIZE};
}

void ConnectRooms(Room* room1, Room* room2, int direction) {
    if (!room1 || !room2 || direction < 0 || direction > 3) return;
    
    room1->connections[direction] = room2;
    room2->connections[(direction + 2) % 4] = room1;
    
    // Create doors
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    switch (direction) {
        case 0: // North
            x1 = ROOM_WIDTH/2;
            y1 = 0;
            x2 = ROOM_WIDTH/2;
            y2 = ROOM_HEIGHT-1;
            break;
        case 1: // East
            x1 = ROOM_WIDTH-1;
            y1 = ROOM_HEIGHT/2;
            x2 = 0;
            y2 = ROOM_HEIGHT/2;
            break;
        case 2: // South
            x1 = ROOM_WIDTH/2;
            y1 = ROOM_HEIGHT-1;
            x2 = ROOM_WIDTH/2;
            y2 = 0;
            break;
        case 3: // West
            x1 = 0;
            y1 = ROOM_HEIGHT/2;
            x2 = ROOM_WIDTH-1;
            y2 = ROOM_HEIGHT/2;
            break;
    }
    
    SetTileAt(room1, x1, y1, TILE_DOOR);
    SetTileAt(room2, x2, y2, TILE_DOOR);
}

bool FindPath(Room* room, Vector2 start, Vector2 end, Vector2* path, int* pathLength) {
    // Simple direct path for now
    if (!room || !path || !pathLength) return false;
    
    path[0] = start;
    path[1] = end;
    *pathLength = 2;
    
    return true;
}

float GetPathCost(Room* room, Vector2 start, Vector2 end) {
    if (!room) return INFINITY;
    return Vector2Distance(start, end);
}

void DebugDrawWorld(World* world) {
    if (!world) return;
    
    const int debugInfoX = 10;
    int debugInfoY = 10;
    const int lineHeight = 20;
    
    DrawText(TextFormat("Rooms: %d/%d", world->roomCount, MAX_ROOMS),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Global Resonance: %.2f", world->globalResonance),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Instability: %.2f", world->instabilityLevel),
             debugInfoX, debugInfoY, 10, WHITE);
    debugInfoY += lineHeight;
    
    DrawText(TextFormat("Stable: %s", world->isStable ? "Yes" : "No"),
             debugInfoX, debugInfoY, 10, WHITE);
} 