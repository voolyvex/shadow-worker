#include "../../include/entities/player.h"
#include "../../include/sound_manager.h"
#include "../../include/world.h"
#include "../../include/resource_manager.h"
#include "../../include/game.h"
#include "../../include/logger.h"
#include <stdlib.h>
#include <math.h>

// Forward declarations
World* GetWorld(void);

// Helper function declarations
static float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static PlayerData* player = NULL;

PlayerData* CreatePlayer(void) {
    if (player) {
        LOG_WARNING(LOG_CORE, "Player already exists");
        return player;
    }

    player = (PlayerData*)malloc(sizeof(PlayerData));
    if (!player) {
        LOG_ERROR(LOG_CORE, "Failed to allocate memory for player");
        return NULL;
    }

    // Initialize player data
    player->position = (Vector2){400, 300};
    player->speed = 200.0f;
    player->health = 100;
    player->stamina = 100;
    player->resonance = 0;
    player->isMoving = false;
    player->velocity = (Vector2){0, 0};
    player->footstepTimer = 0;
    player->footstepInterval = 0.5f;

    LOG_INFO(LOG_CORE, "Player created successfully");
    return player;
}

void DestroyPlayer(void) {
    if (!player) {
        LOG_WARNING(LOG_CORE, "No player to destroy");
        return;
    }

    free(player);
    player = NULL;
    LOG_INFO(LOG_CORE, "Player destroyed");
}

void UpdatePlayer(float deltaTime) {
    if (!player) return;
    
    // Get input
    Vector2 input = {0, 0};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) input.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) input.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) input.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input.x += 1.0f;
    
    // Normalize input vector if necessary
    if (input.x != 0.0f || input.y != 0.0f) {
        float length = sqrtf(input.x * input.x + input.y * input.y);
        input.x /= length;
        input.y /= length;
        player->isMoving = true;
    } else {
        player->isMoving = false;
    }
    
    // Calculate desired movement
    Vector2 movement = {
        input.x * player->speed * deltaTime,
        input.y * player->speed * deltaTime
    };
    
    // Update position
    player->position.x += movement.x;
    player->position.y += movement.y;
    
    // Update velocity for animation
    player->velocity = movement;
    
    // Play footstep sounds
    if (player->isMoving) {
        player->footstepTimer += deltaTime;
        if (player->footstepTimer >= player->footstepInterval) {
            PlayGameSound(SOUND_FOOTSTEP);
            player->footstepTimer = 0.0f;
        }
    } else {
        player->footstepTimer = player->footstepInterval;
    }
    
    // Update stamina
    if (player->isMoving) {
        player->stamina = (int)Clamp((float)player->stamina - 10.0f * deltaTime, 0.0f, 100.0f);
    } else {
        player->stamina = (int)Clamp((float)player->stamina + 20.0f * deltaTime, 0.0f, 100.0f);
    }
}

void DrawPlayer(void) {
    if (!player) return;
    
    // Draw player rectangle for now
    DrawRectangle((int)player->position.x - 16, (int)player->position.y - 16, 32, 32, BLUE);
    
    // Draw debug info if needed
    #ifdef DEBUG
    DrawText(TextFormat("Health: %d", player->health), 10, 10, 20, WHITE);
    DrawText(TextFormat("Stamina: %d", player->stamina), 10, 30, 20, WHITE);
    DrawText(TextFormat("Resonance: %d", player->resonance), 10, 50, 20, WHITE);
    #endif
} 