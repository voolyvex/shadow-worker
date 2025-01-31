#ifndef SHADOW_WORKER_GAME_H
#define SHADOW_WORKER_GAME_H

#include <raylib.h>
#include <stdbool.h>
#include "resource_manager.h"
#include "entity.h"
#include "world.h"

// Game window configuration
#define MAX_ENTITIES 1000
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define GAME_TITLE "Shadow Worker"
#define TARGET_FPS 60

// Forward declarations
typedef struct Scene Scene;
typedef struct EntityPool EntityPool;
typedef struct World World;

// Game states
typedef enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_DIALOGUE,
    STATE_GAME_OVER
} GameState;

// Core game structure
typedef struct Game {
    GameState state;
    bool isRunning;
    float deltaTime;
    Camera2D camera;
    Scene* currentScene;
    ResourceManager* resources;
    EntityPool* entityPool;
    World* world;
} Game;

// Function declarations
Game* Game_Init(void);
void Game_Update(Game* game);
void Game_Draw(Game* game);
void Game_Unload(Game* game);

// Utility functions
void Game_TogglePause(Game* game);
void Game_ChangeState(Game* game, GameState newState);
void Game_UpdateCamera(Game* game);

#endif // SHADOW_WORKER_GAME_H 