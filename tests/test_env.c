#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "include/test_suites.h"
#include "../include/resource_manager.h"
#include "../include/logger.h"
#include "../include/world.h"
#include "../include/map_types.h"
#include "../include/constants.h"

static bool raylib_initialized = false;
static bool in_draw_context = false;
static ResourceManager* g_testManager = NULL;

ResourceManager* GetTestResourceManager(void) {
    return g_testManager;
}

bool InitResourceManager(void) {
    if (!raylib_initialized) {
        fprintf(stderr, "Cannot initialize resource manager: raylib not initialized\n");
        return false;
    }
    
    const char* resourceDir = getenv("TEST_RESOURCES_DIR");
    if (!resourceDir) {
        fprintf(stderr, "TEST_RESOURCES_DIR environment variable not set\n");
        return false;
    }
    
    ResourceManager* manager = CreateResourceManager();
    if (!manager) {
        fprintf(stderr, "Failed to create resource manager\n");
        return false;
    }
    
    // Store the manager for later use
    g_testManager = manager;
    
    // Begin GL context for resource loading
    begin_gl_context();
    
    // Construct resource paths
    char tileset_path[512];
    char player_path[512];
    char npc_path[512];
    
    snprintf(tileset_path, sizeof(tileset_path), "%s/maps/tileset_main.png", resourceDir);
    snprintf(player_path, sizeof(player_path), "%s/sprites/player/idle.png", resourceDir);
    snprintf(npc_path, sizeof(npc_path), "%s/sprites/npc/base.png", resourceDir);
    
    bool success = true;
    
    // Load test resources
    if (!LoadGameTexture(manager, tileset_path, "tileset_main")) {
        fprintf(stderr, "Failed to load tileset: %s\n", tileset_path);
        success = false;
    }
    
    if (success && !LoadGameTexture(manager, player_path, "player_idle")) {
        fprintf(stderr, "Failed to load player sprite: %s\n", player_path);
        success = false;
    }
    
    if (success && !LoadGameTexture(manager, npc_path, "npc_base")) {
        fprintf(stderr, "Failed to load NPC sprite: %s\n", npc_path);
        success = false;
    }
    
    // End GL context after resource loading
    end_gl_context();
    
    if (!success) {
        DestroyResourceManager(manager);
        g_testManager = NULL;
        return false;
    }
    
    return true;
}

bool begin_gl_context(void) {
    if (!raylib_initialized) {
        fprintf(stderr, "Cannot begin GL context: raylib not initialized\n");
        return false;
    }
    
    if (in_draw_context) {
        fprintf(stderr, "GL context already active\n");
        return false;
    }
    
    BeginDrawing();
    in_draw_context = true;
    return true;
}

void end_gl_context(void) {
    if (!raylib_initialized) {
        fprintf(stderr, "Cannot end GL context: raylib not initialized\n");
        return;
    }
    
    if (!in_draw_context) {
        fprintf(stderr, "No active GL context to end\n");
        return;
    }
    
    EndDrawing();
    in_draw_context = false;
}

bool setup_test_environment(void) {
    if (raylib_initialized) {
        fprintf(stderr, "Test environment already initialized\n");
        return false;
    }
    
    // Initialize window with a reasonable size for testing
    SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "Shadow Worker Test Environment");
    
    if (!IsWindowReady()) {
        fprintf(stderr, "Failed to initialize window\n");
        return false;
    }
    
    SetTargetFPS(60);
    raylib_initialized = true;
    
    // Initialize resource manager
    if (!InitResourceManager()) {
        fprintf(stderr, "Failed to initialize resource manager\n");
        teardown_test_environment();
        return false;
    }
    
    return true;
}

void teardown_test_environment(void) {
    if (!raylib_initialized) {
        return;
    }
    
    // Make sure we're not in a GL context when cleaning up
    if (in_draw_context) {
        end_gl_context();
    }
    
    if (g_testManager) {
        begin_gl_context();  // Ensure GL context for resource cleanup
        DestroyResourceManager(g_testManager);
        end_gl_context();
        g_testManager = NULL;
    }
    
    CloseWindow();
    raylib_initialized = false;
} 