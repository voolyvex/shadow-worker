#ifndef SHADOW_WORKER_NPC_H
#define SHADOW_WORKER_NPC_H

#include "entities/entity.h"
#include "world/world.h"
#include "utils/constants.h"
#include <raylib.h>

// Entity dimensions
#define NPC_WIDTH 32
#define NPC_HEIGHT 32

// Personality types
typedef enum {
    PERSONALITY_NEUTRAL,
    PERSONALITY_FRIENDLY,
    PERSONALITY_HOSTILE,
    PERSONALITY_MYSTERIOUS
} Personality;

// NPC data structure
typedef struct {
    EntityState state;  // Using EntityState from entity_types.h
    Texture2D texture;
    int frame;
    float frameTimer;
    float resonanceLevel;
    Personality personality;
    float interactionTimer;
    const char* dialogueText;
    bool canInteract;
    
    // Movement and behavior
    float health;
    float speed;
    float detectionRange;
    float patrolTimer;
    float lastStateChange;
    Vector2 targetPosition;
    int animationFrame;
    float animationTimer;
} NPCData;

// NPC Management functions
Entity* CreateNPC(struct EntityPool* pool, Vector2 position);
void DestroyNPC(Entity* npc);
void UnloadNPC(Entity* entity);

// State update functions
void UpdateNPC(Entity* npc, struct World* world, float deltaTime);
void DrawNPC(Entity* npc);
void OnNPCCollision(Entity* self, Entity* other);

// State management functions
void UpdateIdleState(Entity* npc, struct World* world, float deltaTime);
void UpdatePatrolState(Entity* npc, struct World* world, float deltaTime);
void UpdateChaseState(Entity* npc, struct World* world, float deltaTime);
void UpdateFleeState(Entity* npc, struct World* world, float deltaTime);

// Utility functions
float GetDistanceToPlayer(const Entity* npc, const struct World* world);
bool IsPlayerVisible(const Entity* npc, const struct World* world);
Vector2 GetRandomPatrolPoint(const Entity* npc, const struct World* world);
void UpdateNPCAnimation(Entity* npc, float deltaTime);

#endif // NPC_H