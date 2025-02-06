#include "../include/test_suites.h"
#include "../../include/resource_manager.h"
#include <assert.h>
#include <stdio.h>
#include <raylib.h>

// Helper function to validate texture
static bool is_texture_valid(Texture2D texture) {
    return texture.id > 0 && texture.width > 0 && texture.height > 0;
}

// Test resource manager functionality
int run_resource_manager_tests(void) {
    printf("\nRunning Resource Manager Tests...\n");
    
    // Get the global resource manager
    ResourceManager* manager = GetResourceManager();
    if (!manager || !manager->initialized) {
        printf("Resource manager not initialized\n");
        return TEST_FAILED;
    }
    
    // Create a simple test image file (1x1 white pixel)
    const char* test_image = "test_texture.png";
    Image img = GenImageColor(1, 1, WHITE);
    if (!ExportImage(img, test_image)) {
        printf("Failed to create test image file\n");
        UnloadImage(img);
        return TEST_FAILED;
    }
    UnloadImage(img);
    
    // Test loading the texture using raylib directly first
    Texture2D test_texture = { 0 };
    begin_gl_context();
    {
        test_texture = LoadTexture(test_image);
        if (!is_texture_valid(test_texture)) {
            printf("Failed to load test texture directly with raylib\n");
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
        
        // Verify texture properties
        if (test_texture.width != 1 || test_texture.height != 1) {
            printf("Unexpected texture dimensions: %d x %d\n", test_texture.width, test_texture.height);
            UnloadTexture(test_texture);
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
        
        // Clean up test texture
        UnloadTexture(test_texture);
    }
    end_gl_context();
    
    // Now test the resource manager's texture handling
    bool loaded = false;
    begin_gl_context();
    {
        loaded = LoadGameTexture(manager, test_image, "test_texture");
        if (!loaded) {
            printf("Failed to load test texture through resource manager\n");
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
        
        // Test resource retrieval
        const Texture2D* managed_texture = GetGameTexture(manager, "test_texture");
        if (!managed_texture || !is_texture_valid(*managed_texture)) {
            printf("Failed to retrieve valid texture from resource manager\n");
            UnloadGameTexture(manager, "test_texture");
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
        
        // Test texture properties
        if (managed_texture->width != 1 || managed_texture->height != 1) {
            printf("Unexpected managed texture dimensions: %d x %d\n", 
                   managed_texture->width, managed_texture->height);
            UnloadGameTexture(manager, "test_texture");
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
        
        // Test unloading
        UnloadGameTexture(manager, "test_texture");
        
        // Verify unloading
        managed_texture = GetGameTexture(manager, "test_texture");
        if (managed_texture != NULL) {
            printf("Texture still exists after unloading\n");
            end_gl_context();
            remove(test_image);
            return TEST_FAILED;
        }
    }
    end_gl_context();
    
    // Clean up test file
    remove(test_image);
    printf("Resource manager tests completed successfully\n");
    return TEST_PASSED;
} 