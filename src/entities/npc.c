#include "utils/warning_suppression.h"
#include "entities/npc.h"
#include "entities/entity.h"
#include "entities/entity_types.h"
#include "entities/player.h"
#include "world/world.h"
#include "resource/resource_manager.h"
#include "utils/debug.h"
#include "utils/constants.h"
#include <raymath.h>  // Must come before raylib.h
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raymath.h>  // Must come before raylib.h
#include <raylib.h>
#include <math.h>

END_EXTERNAL_WARNINGS

#define INTERACTION_DISTANCE 64.0f

// Forward declarations of internal functions
static void UpdateNPCInternal(Entity* self, struct World* world, float deltaTime);
static void DrawNPCInternal(const Entity* self);
static void OnNPCCollisionInternal(Entity* self, Entity* other);

// NPC state functions
static void UpdateIdleState(Entity* npc, struct World* world, float deltaTime);
static void UpdatePatrolState(Entity* npc, struct World* world, float deltaTime);
static void UpdateChaseState(Entity* npc, struct World* world, float deltaTime);
static void UpdateFleeState(Entity* npc, struct World* world, float deltaTime);

// Utility functions
static Vector2 GetRandomPatrolPoint(const Entity* npc, const struct World* world);
static float GetDistanceToPlayer(const Entity* npc, const struct World* world);
static bool IsPlayerVisible(const Entity* npc, const struct World* world);
static void UpdateNPCAnimation(Entity* npc, float deltaTime);

// Forward declarations of static functions
static void HandleCollision(Entity* npc, Entity* other);
static void UpdatePathfinding(Entity* npc, World* world);
static void UpdateAnimation(Entity* npc);
static void HandleStateTransition(Entity* npc, World* world);

// Helper function for creating Vector2 values
static Vector2 MakeVector2(float x, float y) {
    Vector2 result = { x, y };
    return result;
}

BEGIN_EXTERNAL_WARNINGS
// Implementation of functions
void UpdateNPC(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !world) return;

    // Update NPC state
    switch (npc->state) {
        case ENTITY_STATE_IDLE:
            UpdateIdleState(npc, world, deltaTime);
            break;
        case ENTITY_STATE_PATROL:
            UpdatePatrolState(npc, world, deltaTime);
            break;
        case ENTITY_STATE_CHASE:
            UpdateChaseState(npc, world, deltaTime);
            break;
        case ENTITY_STATE_FLEE:
            UpdateFleeState(npc, world, deltaTime);
            break;
        default:
            break;
    }

    // Update position and handle collisions
    HandleCollision(npc, world);
    UpdatePathfinding(npc, world);
    UpdateAnimation(npc);
    HandleStateTransition(npc, world);
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
        Vector2 origin = Vector2Zero();
        DrawTexturePro(*render->texture, source, dest, origin, transform->rotation, render->color);
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
        case ENTITY_STATE_CHASE:
            // Stop chasing if collided with target
            ai->state = ENTITY_STATE_IDLE;
            break;
        case ENTITY_STATE_FLEE:
            // Continue fleeing in a different direction
            ai->targetPosition = GetRandomPatrolPoint(self, NULL);
            break;
        default:
            break;
    }
}

Entity* CreateNPC(struct EntityPool* pool, Vector2 position) {
    Entity* npc = CreateEntity(pool, ENTITY_TYPE_NPC, position);
    if (!npc) return NULL;

    // Add required components
    AddComponent(npc, COMPONENT_TRANSFORM);
    AddComponent(npc, COMPONENT_PHYSICS);
    AddComponent(npc, COMPONENT_RENDER);
    AddComponent(npc, COMPONENT_COLLIDER);
    AddComponent(npc, COMPONENT_AI);

    // Set up transform component
    TransformComponent* transform = GetTransformComponent(npc);
    if (transform) {
        transform->position = position;
        transform->rotation = 0.0f;
        transform->scale = 1.0f;
    }

    // Set up physics component
    PhysicsComponent* physics = GetPhysicsComponent(npc);
    if (physics) {
        physics->velocity = (Vector2){0, 0};
        physics->acceleration = (Vector2){0, 0};
        physics->friction = 0.5f;
        physics->mass = 1.0f;
    }

    // Set up render component
    RenderComponent* render = GetRenderComponent(npc);
    if (render) {
        render->color = WHITE;
        render->sourceRect = (Rectangle){0, 0, 32, 32};
        render->origin = (Vector2){16, 16};
        render->visible = true;
    }

    // Set up collider component
    ColliderComponent* collider = GetColliderComponent(npc);
    if (collider) {
        collider->bounds = (Rectangle){position.x - 16, position.y - 16, 32, 32};
        collider->isStatic = false;
        collider->isTrigger = false;
    }

    // Set up AI component
    AIComponent* ai = GetAIComponent(npc);
    if (ai) {
        ai->state = ENTITY_STATE_IDLE;
        ai->patrolRadius = 100.0f;
        ai->detectionRadius = 200.0f;
        ai->homePosition = position;
        ai->targetPosition = position;
        ai->isAggressive = false;
    }

    // Set up callbacks
    npc->Update = UpdateNPCInternal;
    npc->Draw = DrawNPCInternal;
    npc->OnCollision = OnNPCCollisionInternal;
    npc->OnDestroy = UnloadNPC;

    return npc;
}

static void UpdateIdleState(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    if (!ai) return;
    
    // Check for player proximity
    float distanceToPlayer = GetDistanceToPlayer(npc, world);
    if (distanceToPlayer < ai->detectionRadius && IsPlayerVisible(npc, world)) {
        ai->state = ai->isAggressive ? ENTITY_STATE_CHASE : ENTITY_STATE_FLEE;
        return;
    }
    
    // Randomly transition to patrol state
    if (GetRandomValue(0, 100) < 10) {
        ai->state = ENTITY_STATE_PATROL;
        ai->targetPosition = GetRandomPatrolPoint(npc, world);
    }
}

static void UpdatePatrolState(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    TransformComponent* transform = GetTransformComponent(npc);
    if (!ai || !transform) return;
    
    // Check for player
    float distanceToPlayer = GetDistanceToPlayer(npc, world);
    if (distanceToPlayer < ai->detectionRadius && IsPlayerVisible(npc, world)) {
        ai->state = ai->isAggressive ? ENTITY_STATE_CHASE : ENTITY_STATE_FLEE;
        return;
    }
    
    // Move towards target position
    Vector2 direction = Vector2Subtract(ai->targetPosition, transform->position);
    float distance = Vector2Length(direction);
    
    if (distance > 5.0f) {
        direction = Vector2Scale(Vector2Normalize(direction), 100.0f * deltaTime);
        Vector2 newPos = Vector2Add(transform->position, direction);
        
        if (IsWalkable(world, newPos)) {
            transform->position = newPos;
        } else {
            ai->targetPosition = GetRandomPatrolPoint(npc, world);
        }
    }
}

static void UpdateChaseState(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    TransformComponent* transform = GetTransformComponent(npc);
    if (!ai || !transform) return;
    
    Vector2 playerPos = GetPlayerPosition(world);
    float distanceToPlayer = Vector2Distance(transform->position, playerPos);
    
    if (distanceToPlayer > ai->detectionRadius || !IsPlayerVisible(npc, world)) {
        ai->state = ENTITY_STATE_IDLE;
        return;
    }
    
    Vector2 direction = Vector2Subtract(playerPos, transform->position);
    direction = Vector2Scale(Vector2Normalize(direction), 120.0f * deltaTime);
    Vector2 newPos = Vector2Add(transform->position, direction);
    
    if (IsWalkable(world, newPos)) {
        transform->position = newPos;
    }
}

static void UpdateFleeState(Entity* npc, struct World* world, float deltaTime) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    TransformComponent* transform = GetTransformComponent(npc);
    if (!ai || !transform) return;
    
    Vector2 playerPos = GetPlayerPosition(world);
    Vector2 direction = Vector2Subtract(transform->position, playerPos);
    
    if (Vector2Length(direction) > ai->detectionRadius * 2.0f) {
        ai->state = ENTITY_STATE_IDLE;
        return;
    }
    
    direction = Vector2Scale(Vector2Normalize(direction), 150.0f * deltaTime);
    Vector2 newPos = Vector2Add(transform->position, direction);
    
    if (IsWalkable(world, newPos)) {
        transform->position = newPos;
    } else {
        ai->targetPosition = GetRandomPatrolPoint(npc, world);
    }
}

float GetDistanceToPlayer(const Entity* npc, const struct World* world) {
    if (!npc || !world) return 1000.0f;
    
    Vector2 playerPos = GetPlayerPosition(world);
    return Vector2Distance(npc->position, playerPos);
}

bool IsPlayerVisible(const Entity* npc, const struct World* world) {
    if (!npc || !world) return false;
    
    Vector2 playerPos = GetPlayerPosition(world);
    // TODO: Implement line of sight check
    return true;
}

Vector2 GetRandomPatrolPoint(const Entity* npc, const struct World* world) {
    if (!npc || !world) return (Vector2){0.0f, 0.0f};
    
    const AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    float radius = ai->patrolRadius;
    
    float angle = (float)(rand() % 360) * DEG2RAD;
    float distance = (float)(rand() % (int)radius);
    
    return (Vector2){
        ai->homePosition.x + cosf(angle) * distance,
        ai->homePosition.y + sinf(angle) * distance
    };
}

void UpdateNPCAnimation(Entity* npc, float deltaTime) {
    if (!npc || !HasComponent(npc, COMPONENT_AI)) return;
    
    AIComponent* ai = &npc->components_data[COMPONENT_AI].ai;
    ai->animationTimer += deltaTime;
    
    if (ai->animationTimer >= 0.2f) {
        ai->animationFrame = (ai->animationFrame + 1) % 4;
        ai->animationTimer = 0;
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

static void HandleCollision(Entity* npc, Entity* other) {
    if (!npc || !other) return;
    
    // Get components
    PhysicsComponent* physics = GetPhysicsComponent(npc);
    ColliderComponent* collider = GetColliderComponent(npc);
    AIComponent* ai = GetAIComponent(npc);
    
    if (!physics || !collider || !ai) return;
    
    // Handle collision based on other entity type
    switch (other->type) {
        case ENTITY_TYPE_PLAYER:
            // React to player collision
            ai->state = ENTITY_STATE_FLEE;
            break;
            
        case ENTITY_TYPE_OBJECT:
            // Avoid solid objects
            physics->velocity = Vector2Zero();
            break;
            
        default:
            break;
    }
}

static void UpdatePathfinding(Entity* npc, World* world) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    TransformComponent* transform = GetTransformComponent(npc);
    
    if (!ai || !transform) return;
    
    // Simple pathfinding: move directly towards target
    Vector2 direction = Vector2Subtract(ai->targetPosition, transform->position);
    float distance = Vector2Length(direction);
    
    if (distance > 0) {
        direction = Vector2Normalize(direction);
        direction.x *= ai->moveSpeed;
        direction.y *= ai->moveSpeed;
        PhysicsComponent* physics = GetPhysicsComponent(npc);
        if (physics) {
            physics->velocity = direction;
        }
    }
}

static void UpdateAnimation(Entity* npc) {
    if (!npc) return;
    
    AIComponent* ai = GetAIComponent(npc);
    RenderComponent* render = GetRenderComponent(npc);
    
    if (!ai || !render) return;
    
    // Update animation frame based on state
    ai->animationTimer += GetFrameTime();
    if (ai->animationTimer >= ANIMATION_FRAME_TIME) {
        ai->animationTimer = 0;
        ai->animationFrame = (ai->animationFrame + 1) % ANIMATION_FRAME_COUNT;
        
        // Update source rectangle for sprite animation
        render->sourceRect.x = ai->animationFrame * TILE_SIZE;
    }
}

static void HandleStateTransition(Entity* npc, World* world) {
    if (!npc || !world) return;
    
    AIComponent* ai = GetAIComponent(npc);
    if (!ai) return;
    
    // Update state timer
    ai->stateTimer += GetFrameTime();
    
    // Handle state transitions
    switch (ai->state) {
        case ENTITY_STATE_IDLE:
            if (ai->stateTimer >= IDLE_DURATION) {
                ai->state = ENTITY_STATE_PATROL;
                ai->stateTimer = 0;
                // Set new patrol target
                ai->targetPosition = (Vector2){
                    ai->homePosition.x + GetRandomValue(-100, 100),
                    ai->homePosition.y + GetRandomValue(-100, 100)
                };
            }
            break;
            
        case ENTITY_STATE_PATROL:
            // Check if reached target
            if (Vector2Distance(npc->position, ai->targetPosition) < ARRIVAL_THRESHOLD) {
                ai->state = ENTITY_STATE_IDLE;
                ai->stateTimer = 0;
            }
            break;
            
        case ENTITY_STATE_FLEE:
            if (ai->stateTimer >= FLEE_DURATION) {
                ai->state = ENTITY_STATE_IDLE;
                ai->stateTimer = 0;
            }
            break;
            
        default:
            break;
    }
}

// Implementation of static functions follows...
// ... rest of the file ...
END_EXTERNAL_WARNINGS 