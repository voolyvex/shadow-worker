#include "game.h"
#include "resource_manager.h"
#include "world.h"
#include <stdlib.h>

// Forward declaration for menu update function
static void UpdateMenu(Game* game);

Game* InitGame(void) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for game");
        return NULL;
    }
    
    // Initialize game state
    game->state = STATE_MENU;
    game->isRunning = true;
    game->deltaTime = 0.0f;
    game->currentScene = NULL;
    game->resources = NULL;
    game->entityPool = NULL;
    game->world = NULL;
    
    // Initialize camera
    game->camera.target = (Vector2){ 0, 0 };
    game->camera.offset = (Vector2){ WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;
    
    // Initialize resource manager
    if (!InitResourceManager()) {
        TraceLog(LOG_ERROR, "Failed to initialize resource manager");
        free(game);
        return NULL;
    }
    
    // Initialize entity pool
    game->entityPool = InitEntityPool(MAX_ENTITIES);
    if (!game->entityPool) {
        TraceLog(LOG_ERROR, "Failed to initialize entity pool");
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Initialize world
    game->world = InitWorld(game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to initialize world");
        UnloadEntityPool(game->entityPool);
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    return game;
}

static void UpdateMenu(Game* game) {
    if (!game) return;
    
    if (IsKeyPressed(KEY_ENTER)) {
        game->state = STATE_PLAYING;
    }
}

void UpdateGame(Game* game) {
    if (!game) return;
    
    // Update delta time
    game->deltaTime = GetFrameTime();
    
    // Handle global input
    if (IsKeyPressed(KEY_ESCAPE)) {
        switch (game->state) {
            case STATE_PLAYING:
                game->state = STATE_PAUSED;
                break;
            case STATE_PAUSED:
                game->state = STATE_PLAYING;
                break;
            default:
                break;
        }
    }
    
    switch (game->state) {
        case STATE_MENU:
            UpdateMenu(game);
            break;
        case STATE_PLAYING:
            if (game->world) {
                UpdateWorld(game->world, game->deltaTime);
                UpdateGameCamera(game);
            }
            break;
        case STATE_PAUSED:
            // In pause state, still update camera but freeze other systems
            UpdateGameCamera(game);
            break;
        case STATE_DIALOGUE:
            // Handle dialogue state
            if (IsKeyPressed(KEY_SPACE)) {
                game->state = STATE_PLAYING;
            }
            break;
        case STATE_GAME_OVER:
            // Handle game over state
            if (IsKeyPressed(KEY_ENTER)) {
                game->state = STATE_MENU;
            }
            break;
    }
}

void DrawGame(Game* game) {
    if (!game) return;
    
    switch (game->state) {
        case STATE_MENU:
            ClearBackground(BLACK);
            DrawText("SHADOW WORKER", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/3, 30, DARKGRAY);
            DrawText("Press ENTER to start", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2, 20, GRAY);
            break;
            
        case STATE_PLAYING:
            ClearBackground(BLACK);
            BeginMode2D(game->camera);
            if (game->world) {
                DrawWorld(game->world);
            }
            EndMode2D();
            break;
            
        case STATE_PAUSED:
            ClearBackground(ColorAlpha(BLACK, 0.5f));
            BeginMode2D(game->camera);
            if (game->world) {
                DrawWorld(game->world);
            }
            EndMode2D();
            DrawText("PAUSED", WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2, 30, GRAY);
            DrawText("Press ESC to resume", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 40, 20, GRAY);
            break;
            
        case STATE_DIALOGUE:
            ClearBackground(BLACK);
            BeginMode2D(game->camera);
            if (game->world) {
                DrawWorld(game->world);
            }
            EndMode2D();
            DrawRectangle(50, WINDOW_HEIGHT - 200, WINDOW_WIDTH - 100, 150, ColorAlpha(BLACK, 0.8f));
            DrawRectangleLines(50, WINDOW_HEIGHT - 200, WINDOW_WIDTH - 100, 150, GRAY);
            DrawText("Press SPACE to continue", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT - 30, 20, GRAY);
            break;
            
        case STATE_GAME_OVER:
            ClearBackground(BLACK);
            DrawText("GAME OVER", WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/3, 30, RED);
            DrawText("Press ENTER to restart", WINDOW_WIDTH/2 - 110, WINDOW_HEIGHT/2, 20, GRAY);
            break;
    }
}

void UnloadGame(Game* game) {
    if (!game) return;
    
    if (game->world) {
        UnloadWorld(game->world);
    }
    if (game->entityPool) {
        UnloadEntityPool(game->entityPool);
    }
    UnloadResourceManager();
    
    free(game);
}

void TogglePause(Game* game) {
    if (!game) return;
    
    if (game->state == STATE_PLAYING) {
        game->state = STATE_PAUSED;
    } else if (game->state == STATE_PAUSED) {
        game->state = STATE_PLAYING;
    }
}

void ChangeGameState(Game* game, GameState newState) {
    if (!game) return;
    game->state = newState;
}

void UpdateGameCamera(Game* game) {
    if (!game) return;
    
    // Camera zoom controls
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        game->camera.zoom += wheel * 0.1f;
        if (game->camera.zoom < 0.1f) game->camera.zoom = 0.1f;
        if (game->camera.zoom > 3.0f) game->camera.zoom = 3.0f;
    }
    
    // Camera movement controls
    if (IsKeyDown(KEY_W)) game->camera.target.y -= 200.0f * game->deltaTime;
    if (IsKeyDown(KEY_S)) game->camera.target.y += 200.0f * game->deltaTime;
    if (IsKeyDown(KEY_A)) game->camera.target.x -= 200.0f * game->deltaTime;
    if (IsKeyDown(KEY_D)) game->camera.target.x += 200.0f * game->deltaTime;
    
    // Reset camera
    if (IsKeyPressed(KEY_R)) {
        game->camera.zoom = 1.0f;
        game->camera.target = (Vector2){ 0, 0 };
    }
} 