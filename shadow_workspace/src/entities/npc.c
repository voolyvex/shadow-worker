#include "../../include/entities/npc.h"
#include "../../include/world.h"
#include "../../include/resource_manager.h"
#include "../../include/logger.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>

#define INTERACTION_DISTANCE 64.0f

// Forward declarations of internal functions
static void UpdateNPCInternal(Entity* self, struct World* world, float deltaTime);
static void DrawNPCInternal(const Entity* self);
static void OnNPCCollisionInternal(Entity* self, Entity* other);

// NPC state functions
static void UpdateIdleState(Entity* npc, float deltaTime);
static void UpdatePatrolState(Entity* npc, float deltaTime);
static void UpdateChaseState(Entity* npc, float deltaTime);
static void UpdateFleeState(Entity* npc, float deltaTime);

// Utility functions
static Vector2 GetRandomPatrolPoint(void);
static float GetDistanceToPlayer(Vector2 position);
static bool IsPlayerVisible(Vector2 position);
static void UpdateNPCAnimation(Entity* npc, float deltaTime);

void UpdateNPC(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    TransformComponent* transform = &npc->components_data[COMPONENT_TRANSFORM].transform;
    
    // Update based on current state
    switch (ai->state) {
        case NPC_STATE_IDLE:
            UpdateIdleState(npc, deltaTime);
            break;
        case NPC_STATE_PATROL:
            UpdatePatrolState(npc, deltaTime);
            break;
        case NPC_STATE_CHASE:
            UpdateChaseState(npc, deltaTime);
            break;
        case NPC_STATE_FLEE:
            UpdateFleeState(npc, deltaTime);
            break;
        default:
            break;
    }
    
    // Update animation
    UpdateNPCAnimation(npc, deltaTime);
}

void DrawNPC(Entity* npc) {
    if (!npc || !HasComponent(npc, COMPONENT_RENDER)) return;
    
    RenderComponent* render = &npc->components_data[COMPONENT_RENDER].render;
    TransformComponent* transform = &npc->components_data[COMPONENT_TRANSFORM].transform;
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    
    // Draw NPC sprite
    if (render->texture) {
        Rectangle source = { ai->animationFrame * NPC_WIDTH, 0, NPC_WIDTH, NPC_HEIGHT };
        Rectangle dest = { transform->position.x, transform->position.y, NPC_WIDTH * transform->scale, NPC_HEIGHT * transform->scale };
        DrawTexturePro(*render->texture, source, dest, (Vector2){0, 0}, transform->rotation, render->color);
    }
}

void UnloadNPC(Entity* entity) {
    if (!entity) return;
    // No need to free data as it's now handled by the component system
}

void OnNPCCollision(Entity* self, Entity* other) {
    if (!self || !other || !HasComponent(self, COMPONENT_AI)) return;
    
    AIComponent* ai = &self->components_data[COMPONENT_AI].ai;
    
    // Handle collision based on state
    switch (ai->state) {
        case NPC_STATE_CHASE:
            // Stop chasing if collided with target
            ai->state = NPC_STATE_IDLE;
            break;
        case NPC_STATE_FLEE:
            // Continue fleeing in a different direction
            ai->targetPosition = GetRandomPatrolPoint();
            break;
        default:
            break;
    }
}

Entity* CreateNPC(struct EntityPool* pool, Vector2 position) {
    Entity* npc = CreateEntity(pool, ENTITY_TYPE_NPC);
    if (!npc) return NULL;

    // Add required components
    AddComponent(npc, COMPONENT_TRANSFORM);
    AddComponent(npc, COMPONENT_RENDER);
    AddComponent(npc, COMPONENT_PHYSICS);
    AddComponent(npc, COMPONENT_COLLIDER);
    AddComponent(npc, COMPONENT_AI);

    // Initialize transform component
    TransformComponent* transform = &npc->components_data[COMPONENT_TRANSFORM].transform;
    transform->position = position;
    transform->rotation = 0.0f;
    transform->scale = 1.0f;

    // Initialize render component
    RenderComponent* render = &npc->components_data[COMPONENT_RENDER].render;
    render->color = WHITE;
    render->visible = true;
    render->texture = GetGameTexture("npc"); // Assuming this function exists

    // Initialize physics component
    PhysicsComponent* physics = &npc->components_data[COMPONENT_PHYSICS].physics;
    physics->velocity = (Vector2){0, 0};
    physics->acceleration = (Vector2){0, 0};
    physics->mass = 1.0f;

    // Initialize collider component
    ColliderComponent* collider = &npc->components_data[COMPONENT_COLLIDER].collider;
    collider->bounds = (Rectangle){position.x, position.y, NPC_WIDTH, NPC_HEIGHT};
    collider->isTrigger = false;

    // Initialize AI component
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    ai->detectionRadius = 200.0f;
    ai->patrolRadius = 100.0f;
    ai->homePosition = position;
    ai->isAggressive = false;

    // Set up entity functions
    npc->Update = UpdateNPCInternal;
    npc->Draw = DrawNPCInternal;
    npc->OnCollision = OnNPCCollisionInternal;

    return npc;
}

static void UpdateIdleState(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    
    // Randomly transition to patrol state
    if (GetRandomValue(0, 100) < 10) {
        ai->state = NPC_STATE_PATROL;
        ai->targetPosition = GetRandomPatrolPoint();
    }
}

static void UpdatePatrolState(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    TransformComponent* transform = &npc->components_data[COMPONENT_TRANSFORM].transform;
    
    // Move towards target position
    Vector2 direction = Vector2Subtract(ai->targetPosition, transform->position);
    float distance = Vector2Length(direction);
    
    if (distance > 5.0f) {
        direction = Vector2Scale(Vector2Normalize(direction), 100.0f * deltaTime);
        transform->position = Vector2Add(transform->position, direction);
    } else {
        // Get new patrol point
        ai->targetPosition = GetRandomPatrolPoint();
    }
}

static void UpdateChaseState(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    
    // If player is too far, return to idle
    if (!IsPlayerVisible(ai->targetPosition)) {
        ai->state = NPC_STATE_IDLE;
    }
}

static void UpdateFleeState(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    
    // If we've been fleeing for a while, return to idle
    ai->state = NPC_STATE_IDLE;
}

float GetDistanceToPlayer(Vector2 npcPos) {
    // TODO: Implement actual player distance calculation
    return 1000.0f; // Placeholder
}

bool IsPlayerVisible(Vector2 npcPos) {
    // TODO: Implement actual player visibility check
    return false; // Placeholder
}

Vector2 GetRandomPatrolPoint(void) {
    // TODO: Implement proper patrol point generation
    return (Vector2){ GetRandomValue(100, 700), GetRandomValue(100, 500) };
}

static void UpdateNPCAnimation(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    static float animationTimer = 0;
    
    animationTimer += deltaTime;
    if (animationTimer >= 0.2f) {
        ai->animationFrame = (ai->animationFrame + 1) % 4;
        animationTimer = 0;
    }
}

void DestroyNPC(Entity* npc) {
    if (!npc) return;
    DestroyEntity(npc);
}

static void UpdateNPCInternal(Entity* self, struct World* world, float deltaTime) {
    if (!self) return;
    UpdateNPC(self, world, deltaTime);
}

static void DrawNPCInternal(const Entity* self) {
    if (!self) return;
    DrawNPC((Entity*)self);
}

static void OnNPCCollisionInternal(Entity* self, Entity* other) {
    if (!self || !other) return;
    OnNPCCollision(self, other);
} 