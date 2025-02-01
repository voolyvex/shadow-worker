#include "../include/game.h"
#include "../include/resource_manager.h"
#include "../include/world.h"
#include "../include/entities/player.h"
#include "../include/entities/npc.h"
#include "../include/sound_manager.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

// Forward declarations
static void UpdateMenu(Game* game);
static Entity* player = NULL;  // Store player entity globally for easy access
static Entity* npc = NULL;     // Store NPC entity globally for easy access

// Global game instance for easy access
static Game* currentGame = NULL;

// Game state
static struct {
    GameState state;
    World* world;
    Camera2D camera;
    Entity* player;
    ResourceManager* resources;
    float deltaTime;
} game = {0};

// Get world instance
World* GetWorld(void) {
    return currentGame ? currentGame->world : NULL;
}

Game* Game_Init(void) {
    // Allocate game structure
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) {
        TraceLog(LOG_ERROR, "Failed to allocate game structure");
        return NULL;
    }

    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(TARGET_FPS);
    
    // Initialize game structure
    game->state = GAME_STATE_MENU;
    game->isRunning = true;
    game->deltaTime = 0.0f;
    game->currentScene = NULL;
    game->entityPool = NULL;
    game->world = NULL;
    
    // Initialize camera
    game->camera.target = (Vector2){0, 0};
    game->camera.offset = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;
    
    // Initialize systems
    if (!InitResourceManager()) {
        TraceLog(LOG_ERROR, "Failed to initialize resource manager");
        free(game);
        return NULL;
    }
    
    if (!InitSoundManager()) {
        TraceLog(LOG_ERROR, "Failed to initialize sound manager");
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Get resource manager instance
    game->resources = GetResourceManager();
    
    // Initialize world
    game->world = InitWorld(game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to initialize world");
        UnloadSoundManager();
        UnloadResourceManager();
        free(game);
        return NULL;
    }

    // Store global instance
    currentGame = game;
    
    return game;
}

static void UpdateMenu(Game* game) {
    if (!game) return;
    
    if (IsKeyPressed(KEY_ENTER)) {
        game->state = GAME_STATE_PLAYING;
    }
}

void Game_Update(Game* game) {
    if (!game) return;
    
    game->deltaTime = GetFrameTime();
    
    switch (game->state) {
        case GAME_STATE_MENU:
            // Handle menu state
            if (IsKeyPressed(KEY_ENTER)) {
                Game_ChangeState(game, GAME_STATE_PLAYING);
            }
            break;
            
        case GAME_STATE_PLAYING:
            // Update world
            UpdateWorld(game->world, game->deltaTime);
            
            // Update entities
            if (game->entityPool) {
                UpdateEntityPool(game->entityPool, game->deltaTime);
            }
            
            // Handle pause
            if (IsKeyPressed(KEY_ESCAPE)) {
                Game_TogglePause(game);
            }
            break;
            
        case GAME_STATE_PAUSED:
            // Handle pause state
            if (IsKeyPressed(KEY_ESCAPE)) {
                Game_TogglePause(game);
            }
            break;
            
        case GAME_STATE_DIALOGUE:
            // Handle dialogue state
            break;
    }
}

void Game_Draw(Game* game) {
    if (!game) return;
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch (game->state) {
        case GAME_STATE_MENU:
            // Draw menu
            DrawText("SHADOW WORKER", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 30, 30, WHITE);
            DrawText("Press ENTER to start", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20, 20, GRAY);
            break;
            
        case GAME_STATE_PLAYING:
        case GAME_STATE_PAUSED:
            // Draw world
            DrawWorld(game->world);
            
            // Draw entities
            if (game->entityPool) {
                DrawEntityPool(game->entityPool);
            }
            
            // Draw pause overlay if paused
            if (game->state == GAME_STATE_PAUSED) {
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 128});
                DrawText("PAUSED", WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 - 30, 30, WHITE);
                DrawText("Press ESC to resume", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20, 20, GRAY);
            }
            break;
            
        case GAME_STATE_DIALOGUE:
            // Draw dialogue overlay
            break;
    }
    
    EndDrawing();
}

void Game_Unload(Game* game) {
    if (!game) return;
    
    // Unload world
    if (game->world) {
        UnloadWorld(game->world);
    }
    
    // Unload entity pool
    if (game->entityPool) {
        UnloadEntityPool(game->entityPool);
    }
    
    // Unload systems
    UnloadSoundManager();
    UnloadResourceManager();
    
    // Clear global instance
    if (game == currentGame) {
        currentGame = NULL;
    }
    
    // Free game structure
    free(game);
    
    // Close window
    CloseWindow();
}

void Game_TogglePause(Game* game) {
    if (!game) return;
    
    if (game->state == GAME_STATE_PLAYING) {
        Game_ChangeState(game, GAME_STATE_PAUSED);
    } else if (game->state == GAME_STATE_PAUSED) {
        Game_ChangeState(game, GAME_STATE_PLAYING);
    }
}

void Game_ChangeState(Game* game, GameState newState) {
    if (!game) return;
    game->state = newState;
}

void Game_UpdateCamera(Game* game) {
    if (!game || !player) return;
    
    // Make camera follow player
    game->camera.target = player->position;
    
    // Camera zoom controls
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        game->camera.zoom += wheel * 0.1f;
        if (game->camera.zoom < 0.1f) game->camera.zoom = 0.1f;
        if (game->camera.zoom > 3.0f) game->camera.zoom = 3.0f;
    }
    
    // Reset camera zoom
    if (IsKeyPressed(KEY_R)) {
        game->camera.zoom = 1.0f;
    }
} 