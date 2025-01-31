#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#endif

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "../include/game.h"
#include "../include/resource_manager.h"
#include "../include/entity.h"
#include "../include/world.h"

#ifdef _WIN32
#define GetCurrentDir GetCurrentDirectoryA
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

int main(void) {
    // Initialize window
    TraceLog(LOG_INFO, "Initializing window...");
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
    SetTargetFPS(TARGET_FPS);
    
    // Get current directory
    char currentDir[MAX_PATH];
#ifdef _WIN32
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    TraceLog(LOG_INFO, "Current directory: %s", currentDir);
#else
    if (getcwd(currentDir, sizeof(currentDir)) != NULL) {
        TraceLog(LOG_INFO, "Current directory: %s", currentDir);
    } else {
        TraceLog(LOG_ERROR, "Failed to get current directory");
        return 1;
    }
#endif

    // Initialize game
    TraceLog(LOG_INFO, "Initializing game...");
    Game* game = Game_Init();
    if (!game) {
        TraceLog(LOG_ERROR, "Failed to initialize game");
        return 1;
    }
    TraceLog(LOG_INFO, "Game initialized successfully");

    // Main game loop
    TraceLog(LOG_INFO, "Entering main game loop");
    while (!WindowShouldClose() && game->isRunning) {
        game->deltaTime = GetFrameTime();
        
        // Update game state
        Game_Update(game);
        
        // Update camera
        Game_UpdateCamera(game);
        
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        Game_Draw(game);
        
        DrawFPS(10, 10);
        EndDrawing();
    }
    
    // Cleanup
    TraceLog(LOG_INFO, "Cleaning up resources...");
    Game_Unload(game);
    CloseWindow();
    TraceLog(LOG_INFO, "Cleanup complete");
    
    return 0;
}