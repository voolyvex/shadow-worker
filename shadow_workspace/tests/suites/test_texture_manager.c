#include "../include/test_suites.h"
#include "../../include/texture_manager.h"
#include <raylib.h>
#include <assert.h>
#include <stdio.h>

// Test texture manager functionality
int run_texture_manager_tests(void) {
    printf("\nRunning Texture Manager Tests...\n");
    int failures = 0;
    
    // Verify raylib is initialized (should be done by test environment)
    if (!IsWindowReady()) {
        printf("Error: Window not initialized. Test environment setup may have failed.\n");
        return TEST_FAILED;
    }
    
    // Test creation and destruction
    TextureManager* manager = CreateTextureManager();
    if (!manager) {
        printf("Failed to create texture manager\n");
        return TEST_FAILED;
    }
    
    // Test loading a texture atlas
    Rectangle regions[] = {
        (Rectangle){ 0, 0, 32, 32 },  // Idle frame
        (Rectangle){ 32, 0, 32, 32 }  // Walk frame
    };
    
    // Test with test-specific player idle sprite
    const char* testSpritePath = "tests/resources/sprites/player/idle.png";
    printf("Loading test sprite from: %s\n", testSpritePath);
    
    int atlasId = LoadTextureAtlas(manager, testSpritePath, regions, 2);
    if (atlasId < 0) {
        printf("Failed to load texture atlas from: %s\n", testSpritePath);
        DestroyTextureManager(manager);
        return TEST_FAILED;
    }
    
    // Test texture retrieval
    Texture2D texture = GetTextureFromAtlas(manager, atlasId, 0);
    if (texture.id == 0) {
        printf("Failed to retrieve texture from atlas\n");
        DestroyTextureManager(manager);
        return TEST_FAILED;
    }
    
    // Test region retrieval
    Rectangle region = GetTextureRegion(manager, atlasId, 0);
    if (region.width != 32 || region.height != 32) {
        printf("Invalid region dimensions: %f x %f\n", region.width, region.height);
        DestroyTextureManager(manager);
        return TEST_FAILED;
    }
    
    // Test invalid indices
    texture = GetTextureFromAtlas(manager, -1, 0);
    TEST_TRUE(texture.id == 0);
    
    texture = GetTextureFromAtlas(manager, atlasId, -1);
    TEST_TRUE(texture.id == 0);
    
    region = GetTextureRegion(manager, -1, 0);
    TEST_TRUE(region.width == 0 && region.height == 0);
    
    // Cleanup
    DestroyTextureManager(manager);
    
    return failures;
} 