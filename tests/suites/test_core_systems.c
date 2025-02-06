#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/entity_types.h"
#include "../include/entity.h"
#include "../include/entity_pool.h"
#include "../include/world.h"
#include "../include/map_types.h"
#include "../include/constants.h"
#include "../src/core/room_loader.h"
#include "../include/resource_manager.h"
#include <assert.h>
#include "../include/test_suites.h"

// Forward declarations
static int TestWorldStructure(void);
static int TestRoomTemplate(void);
static int TestResourceManagement(void);
static int TestTileRendering(void);
static int TestCollision(void);

int run_core_system_tests(void) {
    printf("=== Starting Core System Tests ===\n");
    
    int failures = 0;
    
    // Run tests in order of dependency
    printf("Running World Structure Test...\n");
    failures += TestWorldStructure();
    if (failures > 0) {
        fprintf(stderr, "World Structure Test failed, skipping remaining tests\n");
        return failures;
    }
    
    printf("Running Room Template Test...\n");
    failures += TestRoomTemplate();
    if (failures > 0) {
        fprintf(stderr, "Room Template Test failed, skipping remaining tests\n");
        return failures;
    }
    
    printf("Running Resource Management Test...\n");
    failures += TestResourceManagement();
    if (failures > 0) {
        fprintf(stderr, "Resource Management Test failed, skipping remaining tests\n");
        return failures;
    }
    
    printf("Running Tile Rendering Test...\n");
    failures += TestTileRendering();
    if (failures > 0) {
        fprintf(stderr, "Tile Rendering Test failed, skipping remaining tests\n");
        return failures;
    }
    
    printf("Running Collision Test...\n");
    failures += TestCollision();
    
    printf("=== Core System Tests Completed ===\n");
    printf("Total failures: %d\n", failures);
    
    return failures;
}

// Helper function to check if a position is within world bounds
static bool IsWorldPositionInBounds(const World* world, Vector2 pos) {
    if (!world) return false;
    return pos.x >= 0 && pos.x < world->width * TILE_SIZE &&
           pos.y >= 0 && pos.y < world->height * TILE_SIZE;
}

static int TestWorldStructure(void) {
    int failures = 0;
    World* world = NULL;
    ResourceManager* resources = NULL;
    bool gl_context_active = false;
    
    printf("  Setting up test resources...\n");
    
    // Setup phase with proper error handling
    resources = GetTestResourceManager();
    if (!resources) {
        fprintf(stderr, "Failed to get test resource manager\n");
        return 1;
    }
    
    // Ensure GL context is properly managed
    printf("  Initializing GL context...\n");
    if (!begin_gl_context()) {
        fprintf(stderr, "Failed to begin GL context\n");
        return 1;
    }
    gl_context_active = true;
    
    // Create world with error handling
    printf("  Creating test world...\n");
    world = CreateWorld(ESTATE_WIDTH, ESTATE_HEIGHT, 9.81f, resources);
    if (!world) {
        fprintf(stderr, "Failed to create world\n");
        end_gl_context();
        return 1;
    }
    
    // Run tests in a protected block
    do {
        printf("  Verifying world properties...\n");
        
        // Verify world properties with proper error handling
        if (!world->tiles || !world->resourceManager) {
            fprintf(stderr, "World structure is invalid\n");
            failures++;
            break;
        }
        
        TEST_EQUAL(world->width, ESTATE_WIDTH);
        TEST_EQUAL(world->height, ESTATE_HEIGHT);
        TEST_FLOAT_EQUAL(world->gravity, 9.81f);
        TEST_NOT_NULL(world->resourceManager);
        TEST_NOT_NULL(world->tiles);
        
        printf("  Testing tile operations...\n");
        
        // Test tile operations with bounds checking
        if (world->width > 0 && world->height > 0) {
            Tile wallTile = CreateTile(TILE_WALL, OBJECT_NONE);
            SetTileAt(world, 0, 0, wallTile);
            
            Tile resultTile = GetTileAt(world, 0, 0);
            TEST_EQUAL_ENUM(resultTile.type, TILE_WALL);
            
            // Test walkability with proper world coordinates and bounds checking
            Vector2 wallPos = {0.0f * TILE_SIZE, 0.0f * TILE_SIZE};
            if (IsWorldPositionInBounds(world, wallPos)) {
                TEST_FALSE(IsWalkable(world, wallPos));
            }
            
            // Test empty tile with bounds checking
            if (world->width > 1 && world->height > 1) {
                SetTile(world, 1, 1, TILE_EMPTY);
                Vector2 emptyPos = {1.0f * TILE_SIZE, 1.0f * TILE_SIZE};
                if (IsWorldPositionInBounds(world, emptyPos)) {
                    TEST_TRUE(IsWalkable(world, emptyPos));
                }
            }
        }
        
        printf("  World structure test completed successfully\n");
    } while (0);
    
    // Cleanup phase - always executed
    printf("  Cleaning up test resources...\n");
    if (world) {
        DestroyWorld(world);
        world = NULL;
    }
    
    if (gl_context_active) {
        end_gl_context();
        gl_context_active = false;
    }
    
    return failures;
}

static int TestRoomTemplate(void) {
    printf("  Room template test not yet implemented\n");
    return TEST_PASSED;
}

static int TestResourceManagement(void) {
    printf("  Resource management test not yet implemented\n");
    return TEST_PASSED;
}

static int TestTileRendering(void) {
    printf("  Tile rendering test not yet implemented\n");
    return TEST_PASSED;
}

static int TestCollision(void) {
    printf("  Collision test not yet implemented\n");
    return TEST_PASSED;
} 