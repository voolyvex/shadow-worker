#include "../include/entity.h"
#include "../include/world.h"
#include "../include/logger.h"
#include <stdlib.h>
#include <string.h>

// Internal helper functions
static void InitializeComponents(Entity* entity);
static void ClearComponents(Entity* entity);
static void InitializeTransformComponent(TransformComponent* component, Vector2 position);
static void InitializePhysicsComponent(PhysicsComponent* component);
static void InitializeRenderComponent(RenderComponent* component);
static void InitializeColliderComponent(ColliderComponent* component);
static void InitializeAIComponent(AIComponent* component);
static void InitializePlayerControlComponent(PlayerControlComponent* component);

// Component size lookup table
static const size_t componentSizes[] = {
    sizeof(TransformComponent),
    sizeof(PhysicsComponent),
    sizeof(RenderComponent),
    sizeof(ColliderComponent),
    sizeof(AIComponent),
    sizeof(PlayerControlComponent)
};

void DestroyEntity(Entity* entity) {
    if (!entity) return;

    // Call destroy callback if set
    if (entity->OnDestroy) {
        entity->OnDestroy(entity);
    }

    // Clear all components
    ClearComponents(entity);

    free(entity);
}

void UpdateEntity(Entity* entity, struct World* world, float deltaTime) {
    if (!entity || !entity->active) return;

    // Update physics if component is present
    PhysicsComponent* physics = GetPhysicsComponent(entity);
    TransformComponent* transform = GetTransformComponent(entity);
    
    if (physics && transform) {
        physics->velocity.x += physics->acceleration.x * deltaTime;
        physics->velocity.y += physics->acceleration.y * deltaTime;
        
        // Apply friction
        physics->velocity.x *= (1.0f - physics->friction * deltaTime);
        physics->velocity.y *= (1.0f - physics->friction * deltaTime);
        
        // Update position
        transform->position.x += physics->velocity.x * deltaTime;
        transform->position.y += physics->velocity.y * deltaTime;
    }

    // Call custom update function if set
    if (entity->Update) {
        entity->Update(entity, world, deltaTime);
    }
}

void DrawEntity(Entity* entity) {
    if (!entity || !entity->active) return;

    // Only draw if render component is present and visible
    RenderComponent* render = GetRenderComponent(entity);
    if (render && render->visible) {
        if (entity->Draw) {
            entity->Draw(entity);
        }
    }
}

void AddComponent(Entity* entity, ComponentFlags component) {
    if (!entity) return;
    
    // Check if component already exists
    if (entity->components & component) return;

    // Get component index
    int componentIndex = 0;
    ComponentFlags flag = component;
    while (flag > 1) {
        flag >>= 1;
        componentIndex++;
    }

    // Initialize component data based on type
    switch (component) {
        case COMPONENT_TRANSFORM: {
            TransformComponent* transform = &entity->components_data[componentIndex].transform;
            transform->position = (Vector2){0.0f, 0.0f};
            transform->rotation = 0.0f;
            transform->scale = 1.0f;
            break;
        }
        case COMPONENT_PHYSICS: {
            PhysicsComponent* physics = &entity->components_data[componentIndex].physics;
            physics->velocity = (Vector2){0.0f, 0.0f};
            physics->acceleration = (Vector2){0.0f, 0.0f};
            physics->friction = 0.5f;
            physics->mass = 1.0f;
            physics->isKinematic = false;
            break;
        }
        case COMPONENT_RENDER: {
            RenderComponent* render = &entity->components_data[componentIndex].render;
            render->texture = NULL;
            render->color = WHITE;
            render->sourceRect = (Rectangle){0.0f, 0.0f, 32.0f, 32.0f};
            render->origin = (Vector2){16.0f, 16.0f};
            render->visible = true;
            render->opacity = 1.0f;
            break;
        }
        case COMPONENT_COLLIDER: {
            ColliderComponent* collider = &entity->components_data[componentIndex].collider;
            collider->bounds = (Rectangle){0.0f, 0.0f, 32.0f, 32.0f};
            collider->isStatic = false;
            collider->isTrigger = false;
            collider->isEnabled = true;
            break;
        }
        case COMPONENT_AI: {
            AIComponent* ai = &entity->components_data[componentIndex].ai;
            ai->patrolRadius = 100.0f;
            ai->detectionRadius = 200.0f;
            ai->homePosition = (Vector2){0.0f, 0.0f};
            ai->targetPosition = (Vector2){0.0f, 0.0f};
            ai->isAggressive = false;
            ai->state = ENTITY_STATE_IDLE;
            ai->stateTimer = 0.0f;
            ai->animationFrame = 0;
            ai->animationTimer = 0.0f;
            break;
        }
        case COMPONENT_PLAYER_CONTROL: {
            PlayerControlComponent* control = &entity->components_data[componentIndex].playerControl;
            control->moveSpeed = 200.0f;
            control->turnSpeed = 180.0f;
            control->isInteracting = false;
            break;
        }
        default:
            break;
    }

    // Update entity's component mask
    entity->components |= component;
}

void RemoveComponent(Entity* entity, ComponentFlags component) {
    if (!entity) return;

    // Check if component exists
    if (!(entity->components & component)) return;

    // Get component index
    int componentIndex = 0;
    ComponentFlags flag = component;
    while (flag > 1) {
        flag >>= 1;
        componentIndex++;
    }

    // Clear component data
    memset(&entity->components_data[componentIndex], 0, sizeof(ComponentData));

    // Update entity's component mask
    entity->components &= ~component;
}

bool HasComponent(const Entity* entity, ComponentFlags component) {
    return entity && (entity->components & component);
}

// Component access functions
TransformComponent* GetTransformComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_TRANSFORM)) return NULL;
    return &entity->components_data[0].transform;
}

PhysicsComponent* GetPhysicsComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_PHYSICS)) return NULL;
    return &entity->components_data[1].physics;
}

RenderComponent* GetRenderComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_RENDER)) return NULL;
    return &entity->components_data[2].render;
}

ColliderComponent* GetColliderComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_COLLIDER)) return NULL;
    return &entity->components_data[3].collider;
}

AIComponent* GetAIComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_AI)) return NULL;
    return &entity->components_data[4].ai;
}

PlayerControlComponent* GetPlayerControlComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_PLAYER_CONTROL)) return NULL;
    return &entity->components_data[5].playerControl;
}

void UpdateEntityPosition(Entity* entity, Vector2 newPosition) {
    if (!entity) return;

    entity->position = newPosition;

    // Update transform component if it exists
    TransformComponent* transform = GetTransformComponent(entity);
    if (transform) {
        transform->position = newPosition;
    }

    // Update collider bounds if it exists
    ColliderComponent* collider = GetColliderComponent(entity);
    if (collider) {
        collider->bounds.x = newPosition.x;
        collider->bounds.y = newPosition.y;
    }
}

static bool CheckEntityCollisionInternal(Entity* a, Entity* b) {
    if (!a || !b || !a->active || !b->active) return false;
    
    ColliderComponent* colliderA = GetColliderComponent(a);
    ColliderComponent* colliderB = GetColliderComponent(b);
    
    if (!colliderA || !colliderB) return false;
    
    return CheckCollisionRecs(colliderA->bounds, colliderB->bounds);
}

void HandleEntityCollision(Entity* a, Entity* b) {
    if (!a || !b || !a->active || !b->active) return;

    if (CheckEntityCollisionInternal(a, b)) {
        // Call collision callbacks if set
        if (a->OnCollision) a->OnCollision(a, b);
        if (b->OnCollision) b->OnCollision(b, a);
    }
}

// Internal helper function implementations
static void InitializeComponents(Entity* entity) {
    if (!entity) return;

    // Initialize transform component (always present)
    InitializeTransformComponent(&entity->components_data[0].transform, (Vector2){0.0f, 0.0f});

    // Clear all other components
    memset(&entity->components_data[1], 0, sizeof(ComponentData));
    memset(&entity->components_data[2], 0, sizeof(ComponentData));
    memset(&entity->components_data[3], 0, sizeof(ComponentData));
    memset(&entity->components_data[4], 0, sizeof(ComponentData));
    memset(&entity->components_data[5], 0, sizeof(ComponentData));
    
    entity->components = COMPONENT_NONE;
}

static void ClearComponents(Entity* entity) {
    if (!entity) return;
    
    // Clear all component data
    memset(&entity->components_data[0].transform, 0, sizeof(TransformComponent));
    memset(&entity->components_data[1].physics, 0, sizeof(PhysicsComponent));
    memset(&entity->components_data[2].render, 0, sizeof(RenderComponent));
    memset(&entity->components_data[3].collider, 0, sizeof(ColliderComponent));
    memset(&entity->components_data[4].ai, 0, sizeof(AIComponent));
    memset(&entity->components_data[5].playerControl, 0, sizeof(PlayerControlComponent));
    
    entity->components = COMPONENT_NONE;
}

// Component constructors
static void InitializeTransformComponent(TransformComponent* component, Vector2 position) {
    if (!component) return;
    component->position = position;
    component->rotation = 0.0f;
    component->scale = 1.0f;
}

static void InitializePhysicsComponent(PhysicsComponent* component) {
    if (!component) return;
    component->velocity = (Vector2){0.0f, 0.0f};
    component->acceleration = (Vector2){0.0f, 0.0f};
    component->friction = 0.5f;
    component->mass = 1.0f;
    component->isKinematic = false;
}

static void InitializeRenderComponent(RenderComponent* component) {
    if (!component) return;
    component->texture = NULL;
    component->color = WHITE;
    component->sourceRect = (Rectangle){0.0f, 0.0f, 32.0f, 32.0f};
    component->origin = (Vector2){16.0f, 16.0f};
    component->visible = true;
    component->opacity = 1.0f;
}

static void InitializeColliderComponent(ColliderComponent* component) {
    if (!component) return;
    component->bounds = (Rectangle){0.0f, 0.0f, 32.0f, 32.0f};
    component->isStatic = false;
    component->isTrigger = false;
    component->isEnabled = true;
}

static void InitializeAIComponent(AIComponent* component) {
    if (!component) return;
    component->patrolRadius = 100.0f;
    component->detectionRadius = 200.0f;
    component->homePosition = (Vector2){0.0f, 0.0f};
    component->targetPosition = (Vector2){0.0f, 0.0f};
    component->isAggressive = false;
    component->state = ENTITY_STATE_IDLE;
    component->stateTimer = 0.0f;
    component->animationFrame = 0;
    component->animationTimer = 0.0f;
}

static void InitializePlayerControlComponent(PlayerControlComponent* component) {
    if (!component) return;
    component->moveSpeed = 200.0f;
    component->turnSpeed = 180.0f;
    component->isInteracting = false;
}

// Game-specific entity creation functions
Entity* CreatePlayerEntity(EntityPool* pool, Vector2 position) {
    Entity* entity = CreateEntity(pool, ENTITY_TYPE_PLAYER, position);
    if (!entity) return NULL;

    // Add required components
    AddComponent(entity, COMPONENT_TRANSFORM);
    AddComponent(entity, COMPONENT_PHYSICS);
    AddComponent(entity, COMPONENT_RENDER);
    AddComponent(entity, COMPONENT_COLLIDER);
    AddComponent(entity, COMPONENT_PLAYER_CONTROL);

    // Initialize transform
    TransformComponent* transform = GetTransformComponent(entity);
    if (transform) {
        transform->position = position;
        transform->rotation = 0.0f;
        transform->scale = 1.0f;
    }

    // Initialize physics
    PhysicsComponent* physics = GetPhysicsComponent(entity);
    if (physics) {
        physics->velocity = (Vector2){0, 0};
        physics->acceleration = (Vector2){0, 0};
        physics->friction = 0.5f;
    }

    // Initialize render
    RenderComponent* render = GetRenderComponent(entity);
    if (render) {
        render->color = WHITE;
        render->sourceRect = (Rectangle){0, 0, 32, 32};
        render->origin = (Vector2){16, 16};
    }

    // Initialize collider
    ColliderComponent* collider = GetColliderComponent(entity);
    if (collider) {
        collider->bounds = (Rectangle){position.x - 16, position.y - 16, 32, 32};
        collider->isStatic = false;
    }

    // Initialize player control
    PlayerControlComponent* control = GetPlayerControlComponent(entity);
    if (control) {
        control->moveSpeed = 200.0f;
        control->turnSpeed = 180.0f;
    }

    return entity;
}

Entity* CreateNPCEntity(EntityPool* pool, Vector2 position) {
    Entity* entity = CreateEntity(pool, ENTITY_TYPE_NPC, position);
    if (!entity) return NULL;

    // Add required components
    AddComponent(entity, COMPONENT_TRANSFORM);
    AddComponent(entity, COMPONENT_PHYSICS);
    AddComponent(entity, COMPONENT_RENDER);
    AddComponent(entity, COMPONENT_COLLIDER);
    AddComponent(entity, COMPONENT_AI);

    // Initialize transform
    TransformComponent* transform = GetTransformComponent(entity);
    if (transform) {
        transform->position = position;
        transform->rotation = 0.0f;
        transform->scale = 1.0f;
    }

    // Initialize physics
    PhysicsComponent* physics = GetPhysicsComponent(entity);
    if (physics) {
        physics->velocity = (Vector2){0, 0};
        physics->acceleration = (Vector2){0, 0};
        physics->friction = 0.5f;
    }

    // Initialize render
    RenderComponent* render = GetRenderComponent(entity);
    if (render) {
        render->color = WHITE;
        render->sourceRect = (Rectangle){0, 0, 32, 32};
        render->origin = (Vector2){16, 16};
    }

    // Initialize collider
    ColliderComponent* collider = GetColliderComponent(entity);
    if (collider) {
        collider->bounds = (Rectangle){position.x - 16, position.y - 16, 32, 32};
        collider->isStatic = false;
    }

    // Initialize AI
    AIComponent* ai = GetAIComponent(entity);
    if (ai) {
        ai->state = ENTITY_STATE_IDLE;
        ai->patrolRadius = 100.0f;
        ai->detectionRadius = 200.0f;
        ai->homePosition = position;
    }

    return entity;
}

// Component iterator functions
void IterateComponents(Entity* entity, void (*callback)(void* component, ComponentFlags type)) {
    if (!entity || !callback) return;

    ComponentFlags flag = 1;
    for (int i = 0; i < MAX_COMPONENT_TYPES; i++, flag <<= 1) {
        if (entity->components & flag) {
            void* componentPtr = &entity->components_data[i];
            callback(componentPtr, flag);
        }
    }
}

void IterateActiveEntities(EntityPool* pool, void (*callback)(Entity* entity)) {
    if (!pool || !callback) return;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i]) {
            callback(&pool->entities[i]);
        }
    }
}

void ForEachEntity(Entity* entities, size_t count, void (*callback)(Entity*)) {
    if (!entities || !callback) return;
    for (size_t i = 0; i < count; i++) {
        if (entities[i].active) {
            callback(&entities[i]);
        }
    }
} 