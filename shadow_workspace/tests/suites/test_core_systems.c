#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/entity_types.h"
#include "../include/entity.h"
#include "../include/entity_pool.h"
#include "../include/world.h"
#include "../src/core/room_loader.h"
#include "../include/resource_manager.h"
#include <assert.h>
#include "../test_env.c"

int passed_tests = 0;
int failed_tests = 0;

void RunCoreSystemTests(void) {
    setup_test_environment();
    LOG_INFO(LOG_CORE, "=== Starting Core System Tests ===");
    
    if (TestWorldStructure()) passed_tests++; else failed_tests++;
    if (TestRoomTemplate()) passed_tests++; else failed_tests++;
    if (TestResourceManagement()) passed_tests++; else failed_tests++;
    if (TestTileRendering()) passed_tests++; else failed_tests++;
    if (TestCollision()) passed_tests++; else failed_tests++;
    
    LOG_INFO(LOG_CORE, "=== Core System Tests Completed ===");
    LOG_INFO(LOG_CORE, "Tests Passed: %d", passed_tests);
    LOG_INFO(LOG_CORE, "Tests Failed: %d", failed_tests);
    teardown_test_environment();
}

void TestWorldStructure(void) {
    // Test world initialization
    World* world = InitWorld(NULL);  // ResourceManager will be set later
    assert(world != NULL);
    assert(world->dimensions.x == ESTATE_WIDTH * TILE_SIZE);
    assert(world->dimensions.y == ESTATE_HEIGHT * TILE_SIZE);
    assert(world->tiles != NULL);
    
    // Test tile operations
    SetTileAt(world, 0, 0, TILE_WALL);
    assert(GetTileAt(world, 0, 0) == TILE_WALL);
    assert(IsTileSolid(world, 0, 0) == true);
    
    UnloadWorld(world);
}

void TestRoomTemplate(void) {
    // Test template loading
    RoomTemplate* template = LoadRoomTemplate("resources/maps/room_templates/basic_room.json");
    assert(template != NULL);
    assert(template->width == 10);
    assert(template->height == 8);
    assert(template->tiles != NULL);
    assert(template->objects != NULL);
    
    // Validate room structure
    assert(template->tiles[0][0] == TILE_WALL); // Corner should be wall
    assert(template->tiles[3][1] == TILE_FLOOR); // Interior should be floor
    
    UnloadRoomTemplate(template);
}

void TestResourceManagement(void) {
    // Test texture loading
    Texture2D tileset = LoadTexture("resources/maps/tileset_main.png");
    assert(tileset.id > 0);
    assert(tileset.width > 0);
    assert(tileset.height > 0);
    
    UnloadTexture(tileset);
}

void TestTileRendering(void) {
    InitWindow(800, 600, "Test Render");
    
    World* world = InitWorld(NULL);  // ResourceManager will be set later
    SetTileAt(world, 5, 5, TILE_WALL);
    
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawWorld(world);
    EndDrawing();
    
    // Visual verification needed
    assert(IsWindowReady());
    
    UnloadWorld(world);
    CloseWindow();
}

void TestCollision(void) {
    World* world = InitWorld(NULL);  // ResourceManager will be set later
    
    // Set up collision test scenario
    SetTileAt(world, 5, 5, TILE_WALL);
    
    // Test collision detection
    Rectangle playerBounds = {4.5f * TILE_SIZE, 4.5f * TILE_SIZE, 32, 32};
    Vector2 movement = {TILE_SIZE, 0}; // Try to move right
    
    bool collision = CheckWorldCollision(world, playerBounds, movement);
    assert(collision == true);
    
    UnloadWorld(world);
}

int main(void) {
    RunCoreSystemTests();
    
    printf("All tests passed!\n");
    return 0;
} 