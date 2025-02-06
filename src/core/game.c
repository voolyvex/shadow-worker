#include "../../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "../../include/game.h"
#include "../../include/resource_manager.h"
#include "../../include/texture_manager.h"
#include "../../include/logger.h"

// Window dimensions
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Texture atlas configuration
#define ATLAS_WIDTH 2048
#define ATLAS_HEIGHT 2048
#define TILE_SIZE 32

bool InitGame(void) {
    // Initialize window and OpenGL context first
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game Title");
    
    // Initialize resource manager
    ResourceManager* resourceManager = GetResourceManager();
    if (!resourceManager || !resourceManager->initialized) {
        LOG_ERROR(LOG_CORE, "Failed to initialize resource manager");
        return false;
    }
    
    // Initialize texture atlas with appropriate dimensions
    if (!InitTextureAtlas(ATLAS_WIDTH, ATLAS_HEIGHT, TILE_SIZE)) {
        LOG_ERROR(LOG_CORE, "Failed to initialize texture atlas");
        return false;
    }
    
    return true;
}

void CleanupGame(void) {
    // Unload all resources in reverse order of initialization
    UnloadTextureAtlas();
    
    // Close window and OpenGL context
    CloseWindow();
    
    LOG_INFO(LOG_CORE, "Game cleanup completed successfully");
} 