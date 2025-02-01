#include <raylib.h>
#include "../include/game.h"
#include "../include/logger.h"

int main(void) {
    // Initialize logging system
    if (!Logger_Init("shadow_worker.log")) {
        return 1;
    }
    
    LOG_INFO(LOG_CORE, "Shadow Worker starting up...");
    Logger_LogMemoryUsage();
    
    // Initialize window
    LOG_DEBUG(LOG_CORE, "Initializing window (1280x720)");
    InitWindow(1280, 720, "Shadow Worker");
    SetTargetFPS(60);
    
    // Initialize game
    LOG_INFO(LOG_CORE, "Initializing game systems");
    Logger_BeginTimer("game_init");
    if (!Game_Init()) {
        LOG_FATAL(LOG_CORE, "Failed to initialize game systems");
        CloseWindow();
        Logger_Shutdown();
        return 1;
    }
    Logger_EndTimer("game_init");
    
    LOG_INFO(LOG_CORE, "Entering main game loop");
    
    // Main game loop
    while (!WindowShouldClose()) {
        Logger_BeginTimer("frame");
        
        Game_Update();
        Game_Draw();
        
        Logger_EndTimer("frame");
        
        // Log performance metrics every 60 frames
        static int frame_count = 0;
        if (++frame_count % 60 == 0) {
            Logger_LogMemoryUsage();
            LOG_TRACE(LOG_CORE, "FPS: %d", GetFPS());
        }
    }
    
    // Cleanup
    LOG_INFO(LOG_CORE, "Game shutting down");
    Game_Unload();
    CloseWindow();
    Logger_Shutdown();
    
    return 0;
} 