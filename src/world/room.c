#include "../../include/world.h"
#include "../../include/game.h"
#include <stdlib.h>
#include <stdio.h>
#include <raymath.h>

void LoadRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    world->currentRoom = room;
    
    // TODO: Load room resources and initialize entities
}

void UnloadRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    // TODO: Unload room resources and cleanup entities
}

void UpdateRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    // Update entities in the room
    UpdateEntities(world->entityPool, world->deltaTime);
    
    // Process entity interactions
    ProcessEntityInteractions(world->entityPool);
}

void DrawRoom(World* world, Room* room) {
    if (!world || !room) return;
    
    // Draw room tiles
    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x++) {
            TileType tile = GetTileAt(room, x, y);
            Vector2 pos = GetTilePosition(x, y);
            
            Color tileColor;
            switch (tile) {
                case TILE_WALL:
                    tileColor = DARKGRAY;
                    break;
                case TILE_FLOOR:
                    tileColor = GRAY;
                    break;
                case TILE_DOOR:
                    tileColor = BROWN;
                    break;
                case TILE_OBSTACLE:
                    tileColor = RED;
                    break;
                default:
                    tileColor = BLACK;
            }
            
            DrawRectangle((int)pos.x, (int)pos.y, TILE_SIZE, TILE_SIZE, tileColor);
        }
    }
    
    // Draw spawn points (for debugging)
    if (room->spawnPoints) {
        for (int i = 0; i < room->spawnCount; i++) {
            DrawCircle(
                (int)room->spawnPoints[i].x,
                (int)room->spawnPoints[i].y,
                5.0f,
                GREEN
            );
        }
    }
    
    // Draw entities
    DrawEntities(world->entityPool);
    
    // Draw resonance effect (placeholder)
    float resonanceAlpha = room->resonanceLevel * 0.3f;
    DrawRectangle(
        (int)room->bounds.x,
        (int)room->bounds.y,
        (int)room->bounds.width,
        (int)room->bounds.height,
        (Color){ 0, 255, 255, (unsigned char)(resonanceAlpha * 255) }
    );
    
    // Draw room type indicator
    const char* roomTypeText = "";
    Color roomTypeColor = WHITE;
    switch (room->type) {
        case ROOM_NORMAL:
            roomTypeText = "NORMAL";
            roomTypeColor = WHITE;
            break;
        case ROOM_CORRIDOR:
            roomTypeText = "CORRIDOR";
            roomTypeColor = GRAY;
            break;
        case ROOM_SPECIAL:
            roomTypeText = "SPECIAL";
            roomTypeColor = YELLOW;
            break;
        case ROOM_BOSS:
            roomTypeText = "BOSS";
            roomTypeColor = RED;
            break;
    }
    
    Vector2 textPos = {
        room->bounds.x + room->bounds.width / 2 - MeasureText(roomTypeText, 20) / 2,
        room->bounds.y + room->bounds.height / 2 - 10
    };
    DrawText(roomTypeText, (int)textPos.x, (int)textPos.y, 20, roomTypeColor);
    
    // Draw room connections
    for (int i = 0; i < 4; i++) {
        if (room->connections[i]) {
            Vector2 start = {
                room->bounds.x + room->bounds.width / 2,
                room->bounds.y + room->bounds.height / 2
            };
            
            Vector2 end = {
                room->connections[i]->bounds.x + room->connections[i]->bounds.width / 2,
                room->connections[i]->bounds.y + room->connections[i]->bounds.height / 2
            };
            
            DrawLineEx(start, end, 2.0f, (Color){ 255, 255, 0, 128 });
        }
    }
}

TileType GetTileAt(Room* room, int x, int y) {
    if (!room || !room->tiles || x < 0 || x >= ROOM_WIDTH || y < 0 || y >= ROOM_HEIGHT) {
        return TILE_EMPTY;
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
    return (Vector2){
        (float)(x * TILE_SIZE),
        (float)(y * TILE_SIZE)
    };
}

bool FindPath(Room* room, Vector2 start, Vector2 end, Vector2* path, int* pathLength) {
    // TODO: Implement A* pathfinding
    (void)room;        // Suppress unused parameter warning
    (void)start;       // Suppress unused parameter warning
    (void)end;         // Suppress unused parameter warning
    (void)path;        // Suppress unused parameter warning
    (void)pathLength;  // Suppress unused parameter warning
    return false;
}

float GetPathCost(Room* room, Vector2 start, Vector2 end) {
    // TODO: Implement path cost calculation
    (void)room;  // Suppress unused parameter warning
    return Vector2Distance(start, end);
}

void ConnectRooms(Room* room1, Room* room2, int direction) {
    if (!room1 || !room2 || direction < 0 || direction > 3) return;
    
    // Connect rooms in both directions
    room1->connections[direction] = room2;
    room2->connections[(direction + 2) % 4] = room1;
    
    // Add doors
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;  // Initialize to avoid warnings
    
    switch (direction) {
        case 0: // North
            x1 = ROOM_WIDTH / 2;
            y1 = 0;
            x2 = ROOM_WIDTH / 2;
            y2 = ROOM_HEIGHT - 1;
            break;
        case 1: // East
            x1 = ROOM_WIDTH - 1;
            y1 = ROOM_HEIGHT / 2;
            x2 = 0;
            y2 = ROOM_HEIGHT / 2;
            break;
        case 2: // South
            x1 = ROOM_WIDTH / 2;
            y1 = ROOM_HEIGHT - 1;
            x2 = ROOM_WIDTH / 2;
            y2 = 0;
            break;
        case 3: // West
            x1 = 0;
            y1 = ROOM_HEIGHT / 2;
            x2 = ROOM_WIDTH - 1;
            y2 = ROOM_HEIGHT / 2;
            break;
    }
    
    // Set door tiles
    if (room1->tiles) room1->tiles[y1 * ROOM_WIDTH + x1] = TILE_DOOR;
    if (room2->tiles) room2->tiles[y2 * ROOM_WIDTH + x2] = TILE_DOOR;
}

void DebugDrawWorld(World* world) {
    if (!world || !world->currentRoom) return;
    
    // Draw room boundaries
    DrawRectangleLinesEx(world->currentRoom->bounds, 1, RED);
    
    // Draw resonance field visualization
    int fieldWidth = (int)world->fieldDimensions.x;
    int fieldHeight = (int)world->fieldDimensions.y;
    
    for (int y = 0; y < fieldHeight; y += TILE_SIZE) {
        for (int x = 0; x < fieldWidth; x += TILE_SIZE) {
            int index = (y * fieldWidth + x);
            float resonance = world->resonanceField[index];
            
            if (resonance > 0.0f) {
                Color color = {0, 255, 255, (unsigned char)(resonance * 128.0f)};
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, color);
            }
        }
    }
    
    char debugText[128];
#ifdef _WIN32
    sprintf_s(debugText, sizeof(debugText), "Instability: %.2f\nResonance: %.2f",
             world->instabilityLevel, world->globalResonance);
#else
    snprintf(debugText, sizeof(debugText), "Instability: %.2f\nResonance: %.2f",
             world->instabilityLevel, world->globalResonance);
#endif
    DrawText(debugText, 10, 30, 20, WHITE);
} 