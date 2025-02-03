#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdbool.h>

// Forward declarations
struct World;
struct EntityPool;
struct ResourceManager;

// Game configuration
#define GAME_TITLE "Shadow Worker"
#define TARGET_FPS 60
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Game states
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_DIALOGUE
} GameState;

// Game structure
typedef struct Game {
    GameState state;
    bool isRunning;
    float deltaTime;
    void* currentScene;
    struct EntityPool* entityPool;
    struct World* world;
    struct ResourceManager* resources;
    Camera2D camera;
} Game;

// Core game functions
Game* Game_Init(void);
void Game_Update(void);
void Game_Draw(void);
void Game_Unload(void);

// Game state management
void Game_TogglePause(Game* game);
void Game_ChangeState(Game* game, GameState newState);
void Game_ResetState(Game* game);
void Game_UpdateCamera(Game* game);

// World access
struct World* GetWorld(void);

#endif // GAME_H 