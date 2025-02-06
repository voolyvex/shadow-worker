#include "utils/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "core/game.h"
#include "utils/logger.h"
#include "utils/debug.h"
#include "utils/crash_handler.h"

int main(void) {
    setupCrashHandler();
    // Initialize logging system
    if (!Logger_Init("shadow_worker.log")) {
        return 1;
    }
    
    LOG_INFO(LOG_CORE, "Shadow Worker starting up...");
    Logger_LogMemoryUsage();
    
    // Initialize window
    LOG_DEBUG(LOG_CORE, "Initializing window (1280x720)");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Shadow Worker");
    if (!IsWindowReady()) {
        LOG_ERROR(LOG_CORE, "Window initialization failed");
        Logger_Shutdown();
        return 1;
    }
    SetTargetFPS(60);
    
    // Initialize game
    LOG_INFO(LOG_CORE, "Initializing game systems");
    Logger_BeginTimer("game_init");
    Game* game = Game_Init();
    Logger_EndTimer("game_init");

    if (!game) {
        LOG_ERROR(LOG_CORE, "Game initialization failed");
        CloseWindow();
        Logger_Shutdown();
        return 1;
    }
    
    LOG_INFO(LOG_CORE, "Entering main game loop");
    
    // Main game loop
    while (!WindowShouldClose() && game->isRunning) {
        Game_Update(game);
        Game_Draw(game);
    }
    
    // Cleanup
    LOG_INFO(LOG_CORE, "Shutting down game systems");
    Game_Unload(game);
    CloseWindow();
    Logger_Shutdown();
    
    return 0;
} 