#include "include/test_suites.h"
#include "../include/game.h"
#include "../include/logger.h"
#include <raylib.h>

void setup_test_environment(void) {
    // Initialize window in test mode (hidden)
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1280, 720, "Shadow Worker Test");
    SetTargetFPS(60);

    // Initialize game systems
    LOG_INFO(LOG_CORE, "Initializing test environment...");
    if (!Game_Init()) {
        LOG_ERROR(LOG_CORE, "Failed to initialize game systems");
        return;
    }
}

void teardown_test_environment(void) {
    LOG_INFO(LOG_CORE, "Cleaning up test environment...");
    Game_Unload();
    CloseWindow();
} 