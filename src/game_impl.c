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

Game* Game_Init(void) {
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
    game->resources = GetResourceManager();
    
    // Initialize sound manager
    if (!InitSoundManager()) {
        TraceLog(LOG_ERROR, "Failed to initialize sound manager");
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Initialize entity pool
    game->entityPool = InitEntityPool(MAX_ENTITIES);
    if (!game->entityPool) {
        TraceLog(LOG_ERROR, "Failed to initialize entity pool");
        UnloadSoundManager();
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Initialize world
    game->world = InitWorld(game->resources);
    if (!game->world) {
        TraceLog(LOG_ERROR, "Failed to initialize world");
        UnloadEntityPool(game->entityPool);
        UnloadSoundManager();
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Create player
    Vector2 roomCenter = {
        game->world->currentRoom->bounds.x + game->world->currentRoom->bounds.width/2,
        game->world->currentRoom->bounds.y + game->world->currentRoom->bounds.height/2
    };
    
    player = CreatePlayer(game->entityPool, roomCenter, game->resources);
    if (!player) {
        TraceLog(LOG_ERROR, "Failed to create player");
        UnloadWorld(game->world);
        UnloadEntityPool(game->entityPool);
        UnloadSoundManager();
        UnloadResourceManager();
        free(game);
        return NULL;
    }
    
    // Set world reference in player data
    ((PlayerData*)player->data)->world = game->world;
    
    // Create NPC slightly to the right of the player
    Vector2 npcPos = {
        roomCenter.x + TILE_SIZE * 3,  // 3 tiles to the right of center
        roomCenter.y
    };
    
    npc = CreateNPC(game->entityPool, npcPos, game->resources);
    if (!npc) {
        TraceLog(LOG_ERROR, "Failed to create NPC");
        UnloadWorld(game->world);
        UnloadEntityPool(game->entityPool);
        UnloadSoundManager();
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

void Game_Update(Game* game) {
    if (!game) return;
    
    // Update delta time
    game->deltaTime = GetFrameTime();
    
    // Update sound manager
    UpdateSoundManager();
    
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
                
                // Update entities
                if (player) {
                    Vector2 oldPos = player->position;
                    player->Update(player, game->deltaTime);
                    
                    // Play footstep sound if moved
                    if (Vector2Distance(oldPos, player->position) > 0.1f) {
                        PlayGameSound(SOUND_FOOTSTEP);
                    }
                    
                    // Check for interaction with NPC
                    if (npc && IsKeyPressed(KEY_E) && 
                        Vector2Distance(player->position, npc->position) < 64.0f) {
                        PlayGameSound(SOUND_INTERACT);
                        npc->OnInteraction(npc, player);
                    }
                }
                
                if (npc) {
                    npc->Update(npc, game->deltaTime);
                }
                
                Game_UpdateCamera(game);
            }
            break;
        case STATE_PAUSED:
            // In pause state, still update camera but freeze other systems
            Game_UpdateCamera(game);
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

void Game_Draw(Game* game) {
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
                if (player) {
                    player->Draw(player);
                }
                if (npc) {
                    npc->Draw(npc);
                }
            }
            EndMode2D();
            break;
            
        case STATE_PAUSED:
            ClearBackground(ColorAlpha(BLACK, 0.5f));
            BeginMode2D(game->camera);
            if (game->world) {
                DrawWorld(game->world);
                if (player) {
                    player->Draw(player);
                }
                if (npc) {
                    npc->Draw(npc);
                }
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
                if (player) {
                    player->Draw(player);
                }
                if (npc) {
                    npc->Draw(npc);
                }
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

void Game_Unload(Game* game) {
    if (!game) return;
    
    if (player) {
        UnloadPlayer(player);
        player = NULL;
    }
    
    if (npc) {
        UnloadNPC(npc);
        npc = NULL;
    }
    
    if (game->world) {
        UnloadWorld(game->world);
    }
    if (game->entityPool) {
        UnloadEntityPool(game->entityPool);
    }
    
    UnloadSoundManager();
    UnloadResourceManager();
    
    free(game);
}

void Game_TogglePause(Game* game) {
    if (!game) return;
    
    if (game->state == STATE_PLAYING) {
        game->state = STATE_PAUSED;
    } else if (game->state == STATE_PAUSED) {
        game->state = STATE_PLAYING;
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