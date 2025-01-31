#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include "../entities/player.h"
#include "../entities/npc.h"
#include "../world/room.h"

#define MAX_ROOMS 32
#define MAX_NPCS_PER_ROOM 8

typedef struct {
    // Core systems
    Camera2D camera;
    bool isPaused;
    float gameTime;
    
    // World state
    Room* currentRoom;
    Room rooms[MAX_ROOMS];
    int roomCount;
    
    // Entities
    Player player;
    
    // UI state
    bool showDialogue;
    char* currentDialogue;
    
    // Debug info
    bool showDebug;
} GameState;

void InitGameState(GameState* state);
void UpdateGameState(GameState* state);
void CleanupGameState(GameState* state);

#endif // GAME_STATE_H 