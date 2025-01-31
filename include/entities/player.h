#ifndef SHADOW_WORKER_PLAYER_H
#define SHADOW_WORKER_PLAYER_H

#include "../entity.h"
#include "../resource_manager.h"
#include "../world.h"

// Player data structure
typedef struct PlayerData {
    float health;
    float stamina;
    float resonance;
    bool isInvulnerable;
    bool isInteracting;
    World* world;
} PlayerData;

// Function declarations
Entity* CreatePlayer(EntityPool* pool, Vector2 position, ResourceManager* resources);
void UpdatePlayer(Entity* entity, float deltaTime);
void DrawPlayer(Entity* entity);
void UnloadPlayer(Entity* player);

#endif // SHADOW_WORKER_PLAYER_H 