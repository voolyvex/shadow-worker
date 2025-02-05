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

bool InitResourceManager(void) {
    ResourceManager* manager = GetResourceManager();
    if (!manager) return false;
    
    // Initialize the manager's arrays
    manager->textures = (TextureResource*)calloc(MAX_TEXTURES, sizeof(TextureResource));
    manager->waves = (WaveResource*)calloc(MAX_SOUNDS, sizeof(WaveResource));
    manager->music = (MusicResource*)calloc(MAX_MUSIC, sizeof(MusicResource));
    manager->fonts = (FontResource*)calloc(MAX_FONTS, sizeof(FontResource));
    manager->shaders = (ShaderResource*)calloc(MAX_SHADERS, sizeof(ShaderResource));
    
    if (!manager->textures || !manager->waves || !manager->music ||
        !manager->fonts || !manager->shaders) {
        UnloadResourceManager();
        return false;
    }
    
    manager->textureCount = 0;
    manager->waveCount = 0;
    manager->musicCount = 0;
    manager->fontCount = 0;
    manager->shaderCount = 0;
    
    manager->textureCapacity = MAX_TEXTURES;
    manager->waveCapacity = MAX_SOUNDS;
    manager->musicCapacity = MAX_MUSIC;
    manager->fontCapacity = MAX_FONTS;
    manager->shaderCapacity = MAX_SHADERS;
    
    manager->initialized = true;
    return true;
}

void begin_gl_context(void) {
    if (!in_draw_context && raylib_initialized) {
        BeginDrawing();
        in_draw_context = true;
    }
}

void end_gl_context(void) {
    if (in_draw_context && raylib_initialized) {
        EndDrawing();
        in_draw_context = false;
    }
}

void setup_test_environment(void) {
    // Set trace log level before anything else
    SetTraceLogLevel(LOG_DEBUG);
    
    // Initialize raylib if not already initialized
    if (!IsWindowReady()) {
        SetConfigFlags(FLAG_WINDOW_HIDDEN);  // Hide the window during tests
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shadow Worker Tests");
        SetTargetFPS(60);
        raylib_initialized = true;
        
        // Do an initial frame to ensure OpenGL context is fully set up
        begin_gl_context();
        ClearBackground(BLACK);
        end_gl_context();
    }
    
    // Initialize logger
    InitLogger();
    SetLogLevel(LOG_DEBUG);
    
    // Initialize resource manager
    if (!InitResourceManager()) {
        fprintf(stderr, "Failed to initialize resource manager\n");
        CloseLogger();
        if (raylib_initialized) {
            CloseWindow();
        }
        exit(1);
    }
}

void teardown_test_environment(void) {
    // Make sure we're in the main thread's OpenGL context
    if (raylib_initialized) {
        // Ensure we're not in a drawing context
        if (in_draw_context) {
            end_gl_context();
        }
        
        // Do final cleanup with fresh context
        begin_gl_context();
        UnloadResourceManager();
        end_gl_context();
        
        // One final frame to ensure all GL operations are complete
        begin_gl_context();
        ClearBackground(BLACK);
        end_gl_context();
    }
    
    // Cleanup logger
    CloseLogger();
    
    // Close raylib last
    if (raylib_initialized) {
        CloseWindow();
        raylib_initialized = false;
    }
    
    printf("\nTest environment cleaned up\n");
} 