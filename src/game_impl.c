#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "../include/game.h"
#include "../include/resource_manager.h"
#include "../include/world.h"
#include "../include/entity_pool.h"
#include "../include/entity_types.h"
#include "../include/entities/player.h"
#include "../include/entities/npc.h"
#include "../include/sound_manager.h"
#include "../include/constants.h"
#include <stdlib.h>

// Forward declarations
static void UpdateMenu(Game* game);
static void UpdateEntities(Game* game);
static void DrawEntities(Game* game);
static void UpdateCamera(Game* game);
static void UpdateGame(Game* game);
static void DrawGame(Game* game);
static void UnloadGame(Game* game);

static Entity* player = NULL;  // Store player entity globally for easy access
static Entity* npc = NULL;     // Store NPC entity globally for easy access

// Global game instance for easy access
static Game* g_game = NULL;

// Game state
static struct {
    GameState state;
    World* world;
    Camera2D camera;
    Entity* player;
    ResourceManager* resources;
    float deltaTime;
} g_gameState = {0};

// Get world instance
struct World* GetWorld(void) {
    return g_game ? g_game->world : NULL;
}

Game* Game_Init(void) {
    TraceLog(LOG_INFO, "Starting game initialization...");
    
    // Allocate game structure
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) {
        TraceLog(LOG_ERROR, "Failed to allocate game structure");
        return NULL;
    }
    TraceLog(LOG_INFO, "Game structure allocated successfully");
    
    // Initialize game state
    game->state = GAME_STATE_MENU;
    game->isRunning = true;
    game->deltaTime = 0.0f;
    
    // Initialize resource manager
    game->resources = CreateResourceManager();
    if (!game->resources) {
        TraceLog(LOG_ERROR, "Failed to create resource manager");
        free(game);
        return NULL;
    }
    
    // Create initial world
    game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to create world");
        DestroyResourceManager(game->resources);
        free(game);
        return NULL;
    }
    
    // Initialize camera
    game->camera.target = (Vector2){ 0, 0 };
    game->camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;
    
    // Store global instance
    g_game = game;
    
    TraceLog(LOG_INFO, "Game initialization completed successfully");
    return game;
}

static void UpdateMenu(Game* game) {
    if (!game) return;
    
    if (IsKeyPressed(KEY_ENTER)) {
        Game_ChangeState(game, GAME_STATE_PLAYING);
    } else if (IsKeyPressed(KEY_ESCAPE)) {
        game->isRunning = false;
    }
}

void Game_Update(void) {
    if (!g_game) return;
    
    g_game->deltaTime = GetFrameTime();
    
    switch (g_game->state) {
        case GAME_STATE_MENU:
            UpdateMenu(g_game);
            break;
            
        case GAME_STATE_PLAYING:
            if (g_game->world) {
                UpdateWorld(g_game->world, g_game->deltaTime);
                if (g_game->entityPool) {
                    UpdateEntityPool(g_game->entityPool, g_game->world, g_game->deltaTime);
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                Game_TogglePause(g_game);
            }
            break;
            
        case GAME_STATE_PAUSED:
            // Handle pause state
            if (IsKeyPressed(KEY_ESCAPE)) {
                Game_TogglePause(g_game);
            } else if (IsKeyPressed(KEY_Q)) {
                g_game->isRunning = false;
            }
            break;
            
        case GAME_STATE_DIALOGUE:
            // Handle dialogue state
            break;
    }

    // Update camera if playing
    if (g_game->state == GAME_STATE_PLAYING) {
        Game_UpdateCamera(g_game);
    }
}

void Game_Draw(void) {
    if (!g_game) return;
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch (g_game->state) {
        case GAME_STATE_MENU:
            // Draw menu
            DrawText("SHADOW WORKER", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 30, 30, WHITE);
            DrawText("Press ENTER to start", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20, 20, GRAY);
            DrawText("Press ESC to quit", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 50, 20, GRAY);
            break;
            
        case GAME_STATE_PLAYING:
        case GAME_STATE_PAUSED:
            // Draw world
            BeginMode2D(g_game->camera);
            if (g_game->world) {
                DrawWorld(g_game->world);
                if (g_game->entityPool) {
                    DrawEntityPool(g_game->entityPool);
                }
            }
            EndMode2D();
            
            // Draw pause overlay if paused
            if (g_game->state == GAME_STATE_PAUSED) {
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 128});
                DrawText("PAUSED", WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 - 30, 30, WHITE);
                DrawText("Press ESC to resume", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20, 20, GRAY);
                DrawText("Press Q to quit", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 50, 20, GRAY);
            }
            break;
            
        case GAME_STATE_DIALOGUE:
            // Draw dialogue overlay
            break;
    }
    
    EndDrawing();
}

void Game_Unload(void) {
    if (!g_game) return;
    
    // Unload world
    if (g_game->world) {
        UnloadWorld(g_game->world);
    }
    
    // Unload entity pool
    if (g_game->entityPool) {
        DestroyEntityPool(g_game->entityPool);
    }
    
    // Unload systems
    UnloadSoundManager();
    UnloadResourceManager();
    
    // Free game structure
    free(g_game);
    g_game = NULL;
    
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
    
    // Clean up old state if needed
    if (game->world) {
        UnloadWorld(game->world);
        game->world = NULL;
    }
    
    // Initialize new state
    game->state = newState;
    
    // Create new world for the new state
    game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to create world for new state");
        return;
    }
}

void Game_UpdateCamera(Game* game) {
    if (!game) return;
    
    // Create world if not exists
    if (!game->world) {
        game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
        if (!game->world) {
            TraceLog(LOG_ERROR, "Failed to create world for camera update");
            return;
        }
    }
    
    // Find the player entity
    Entity* player = NULL;
    for (int i = 0; i < game->world->entityPool->count; i++) {
        Entity* entity = &game->world->entityPool->entities[i];
        if (entity->type == ENTITY_TYPE_PLAYER) {
            player = entity;
            break;
        }
    }
    
    // Update camera to follow player
    if (player) {
        game->camera.target = player->position;
    }
}

void Game_ResetCamera(Game* game) {
    if (!game) return;
    
    // Create world if not exists
    if (!game->world) {
        game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
        if (!game->world) {
            TraceLog(LOG_ERROR, "Failed to create world for camera reset");
            return;
        }
    }
    
    // Reset camera position and zoom
    game->camera.target = (Vector2){ 0, 0 };
    game->camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;
}

void Game_ResetState(Game* game) {
    if (!game) return;
    
    // Clean up existing world if any
    if (game->world) {
        UnloadWorld(game->world);
    }
    
    // Create new world with default settings
    game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to create world during reset");
        return;
    }
    
    // Reset camera
    game->camera.target = (Vector2){ 0, 0 };
    game->camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;
    
    // Reset game state
    game->state = GAME_STATE_MENU;
    game->deltaTime = 0.0f;
}

void UpdateGame(Game* game) {
    if (!game) return;
    
    // Update game state
    game->deltaTime = GetFrameTime();
    
    // Create world if not exists
    if (!game->world) {
        game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
        if (!game->world) {
            TraceLog(LOG_ERROR, "Failed to create world");
            return;
        }
    }
    
    // Update world and all entities
    UpdateWorld(game->world, game->deltaTime);

    // Handle input
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->state = GAME_STATE_PAUSED;
    }

    // Update game state
    switch (game->state) {
        case GAME_STATE_PLAYING:
            // Game logic updates here
            break;

        case GAME_STATE_PAUSED:
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->state = GAME_STATE_PLAYING;
            }
            break;

        default:
            break;
    }
}

void DrawGame(Game* game) {
    if (!game || !game->world) return;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw world and all entities
    DrawWorld(game->world);

    // Draw UI based on game state
    switch (game->state) {
        case GAME_STATE_PLAYING:
            // Draw HUD here
            break;

        case GAME_STATE_PAUSED:
            DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 40, 40, GRAY);
            break;

        default:
            break;
    }

    EndDrawing();
}

void UnloadGame(Game* game) {
    if (!game) return;

    // Unload world
    if (game->world) {
        UnloadWorld(game->world);
    }

    // Free game structure
    free(game);
}

Game* CreateGame(void) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) return NULL;

    // Initialize game state
    game->state = GAME_STATE_PLAYING;
    game->world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, game->resources);
    if (!game->world) {
        free(game);
        return NULL;
    }

    return game;
} 