#include "../include/entity.h"
#include "../include/world.h"
#include <stdlib.h>
#include <string.h>

// Internal helper functions
static void InitializeComponents(Entity* entity);
static void ClearComponents(Entity* entity);

// Component size lookup table
static const size_t componentSizes[] = {
    sizeof(TransformComponent),
    sizeof(PhysicsComponent),
    sizeof(RenderComponent),
    sizeof(ColliderComponent),
    sizeof(AIComponent),
    sizeof(PlayerControlComponent)
};

Entity* CreateEntity(EntityType type) {
    Entity* entity = (Entity*)calloc(1, sizeof(Entity));
    if (!entity) return NULL;

    entity->type = type;
    entity->active = true;
    entity->components = COMPONENT_NONE;

    // Initialize components with default values
    InitializeComponents(entity);

    // Set default transform component (always present)
    AddComponent(entity, COMPONENT_TRANSFORM);
    entity->transform.scale = (Vector2){ 1.0f, 1.0f };

    return entity;
}

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
    if (HasComponent(entity, COMPONENT_PHYSICS)) {
        entity->physics.velocity.x += entity->physics.acceleration.x * deltaTime;
        entity->physics.velocity.y += entity->physics.acceleration.y * deltaTime;
        
        // Apply friction
        entity->physics.velocity.x *= (1.0f - entity->physics.friction * deltaTime);
        entity->physics.velocity.y *= (1.0f - entity->physics.friction * deltaTime);
        
        // Update position
        entity->transform.position.x += entity->physics.velocity.x * deltaTime;
        entity->transform.position.y += entity->physics.velocity.y * deltaTime;
    }

    // Call custom update function if set
    if (entity->Update) {
        entity->Update(entity, world, deltaTime);
    }
}

void DrawEntity(Entity* entity) {
    if (!entity || !entity->active) return;

    // Only draw if render component is present and visible
    if (HasComponent(entity, COMPONENT_RENDER) && entity->render.visible) {
        if (entity->Draw) {
            entity->Draw(entity);
        }
    }
}

void AddComponent(Entity* entity, ComponentFlags component) {
    if (!entity || !entity->pool || !entity->pool->registry) return;
    
    // Check if component already exists
    if (entity->components & component) return;

    // Get component index
    int componentIndex = 0;
    ComponentFlags flag = component;
    while (flag > 1) {
        flag >>= 1;
        componentIndex++;
    }

    // Add component to registry
    void* componentData = AddComponentToRegistry(
        entity->pool->registry,
        componentIndex,
        componentSizes[componentIndex]
    );

    if (!componentData) return;

    // Initialize component data
    memset(componentData, 0, componentSizes[componentIndex]);

    // Update entity's component mask
    entity->components |= component;

    // Store component data reference
    entity->components_data[componentIndex] = componentData;

    // Initialize default values based on component type
    switch (component) {
        case COMPONENT_TRANSFORM:
            ((TransformComponent*)componentData)->scale = (Vector2){1.0f, 1.0f};
            break;
        case COMPONENT_PHYSICS:
            ((PhysicsComponent*)componentData)->friction = 0.8f;
            break;
        case COMPONENT_RENDER:
            ((RenderComponent*)componentData)->tint = WHITE;
            break;
        case COMPONENT_COLLIDER:
            ((ColliderComponent*)componentData)->bounds = (Rectangle){0, 0, 32, 32};
            break;
        case COMPONENT_AI:
            ((AIComponent*)componentData)->moveSpeed = 100.0f;
            ((AIComponent*)componentData)->turnSpeed = 2.0f;
            break;
        case COMPONENT_PLAYER_CONTROL:
            ((PlayerControlComponent*)componentData)->moveSpeed = 200.0f;
            ((PlayerControlComponent*)componentData)->turnSpeed = 4.0f;
            break;
    }
}

void RemoveComponent(Entity* entity, ComponentFlags component) {
    if (!entity || !entity->pool || !entity->pool->registry) return;

    // Check if component exists
    if (!(entity->components & component)) return;

    // Get component index
    int componentIndex = 0;
    ComponentFlags flag = component;
    while (flag > 1) {
        flag >>= 1;
        componentIndex++;
    }

    // Calculate entity index in pool
    size_t entityId = entity - entity->pool->entities;

    // Remove component from registry
    RemoveComponentFromRegistry(entity->pool->registry, componentIndex, entityId);

    // Update entity's component mask
    entity->components &= ~component;
    entity->components_data[componentIndex] = NULL;
}

bool HasComponent(const Entity* entity, ComponentFlags component) {
    return entity && (entity->components & component);
}

// Component getter functions
TransformComponent* GetTransformComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_TRANSFORM) ? 
        (TransformComponent*)entity->components_data[0] : NULL;
}

PhysicsComponent* GetPhysicsComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_PHYSICS) ? 
        (PhysicsComponent*)entity->components_data[1] : NULL;
}

RenderComponent* GetRenderComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_RENDER) ? 
        (RenderComponent*)entity->components_data[2] : NULL;
}

ColliderComponent* GetColliderComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_COLLIDER) ? 
        (ColliderComponent*)entity->components_data[3] : NULL;
}

AIComponent* GetAIComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_AI) ? 
        (AIComponent*)entity->components_data[4] : NULL;
}

PlayerControlComponent* GetPlayerControlComponent(Entity* entity) {
    return HasComponent(entity, COMPONENT_PLAYER_CONTROL) ? 
        (PlayerControlComponent*)entity->components_data[5] : NULL;
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

bool CheckEntityCollision(Entity* a, Entity* b) {
    if (!a || !b || !a->active || !b->active) return false;
    if (!HasComponent(a, COMPONENT_COLLIDER) || !HasComponent(b, COMPONENT_COLLIDER)) return false;

    return CheckCollisionRecs(a->collider.bounds, b->collider.bounds);
}

void HandleEntityCollision(Entity* a, Entity* b) {
    if (!a || !b || !a->active || !b->active) return;

    if (CheckEntityCollision(a, b)) {
        // Call collision callbacks if set
        if (a->OnCollision) a->OnCollision(a, b);
        if (b->OnCollision) b->OnCollision(b, a);
    }
}

// Internal helper function implementations
static void InitializeComponents(Entity* entity) {
    if (!entity) return;

    // Initialize transform component (always present)
    InitializeTransformComponent(&entity->components_data[0].transform);

    // Clear all other components
    memset(&entity->components_data[1], 0, sizeof(ComponentData) * (MAX_COMPONENTS - 1));
}

static void ClearComponents(Entity* entity) {
    if (!entity) return;
    
    // Clear all component data
    memset(&entity->transform, 0, sizeof(TransformComponent));
    memset(&entity->physics, 0, sizeof(PhysicsComponent));
    memset(&entity->render, 0, sizeof(RenderComponent));
    memset(&entity->collider, 0, sizeof(ColliderComponent));
    memset(&entity->ai, 0, sizeof(AIComponent));
    memset(&entity->playerControl, 0, sizeof(PlayerControlComponent));
    
    entity->components = COMPONENT_NONE;
}

// Component constructors
static void InitializeTransformComponent(TransformComponent* component) {
    if (!component) return;
    component->position = (Vector2){0, 0};
    component->scale = (Vector2){1.0f, 1.0f};
    component->rotation = 0.0f;
}

static void InitializePhysicsComponent(PhysicsComponent* component) {
    if (!component) return;
    component->velocity = (Vector2){0, 0};
    component->acceleration = (Vector2){0, 0};
    component->friction = 0.5f;
}

static void InitializeRenderComponent(RenderComponent* component) {
    if (!component) return;
    component->tint = WHITE;
    component->sourceRect = (Rectangle){0, 0, 32, 32};
    component->origin = (Vector2){16, 16};
}

static void InitializeColliderComponent(ColliderComponent* component) {
    if (!component) return;
    component->bounds = (Rectangle){0, 0, 32, 32};
    component->isStatic = false;
}

static void InitializeAIComponent(AIComponent* component) {
    if (!component) return;
    component->state = NPC_STATE_IDLE;
    component->patrolRadius = 100.0f;
    component->moveSpeed = 100.0f;
    component->turnSpeed = 90.0f;
    component->homePosition = (Vector2){0, 0};
}

static void InitializePlayerControlComponent(PlayerControlComponent* component) {
    if (!component) return;
    component->moveSpeed = 200.0f;
    component->turnSpeed = 180.0f;
    component->isInteracting = false;
}

// Game-specific entity creation functions
Entity* CreatePlayerEntity(EntityPool* pool, Vector2 position) {
    Entity* entity = CreateEntity(pool, ENTITY_TYPE_PLAYER);
    if (!entity) return NULL;

    // Add required components
    AddComponent(entity, COMPONENT_TRANSFORM);
    AddComponent(entity, COMPONENT_PHYSICS);
    AddComponent(entity, COMPONENT_RENDER);
    AddComponent(entity, COMPONENT_COLLIDER);
    AddComponent(entity, COMPONENT_PLAYER_CONTROL);

    // Initialize transform
    TransformComponent* transform = GetTransformComponent(entity);
    transform->position = position;
    transform->rotation = 0.0f;
    transform->scale = (Vector2){1.0f, 1.0f};

    // Initialize physics
    PhysicsComponent* physics = GetPhysicsComponent(entity);
    physics->velocity = (Vector2){0, 0};
    physics->acceleration = (Vector2){0, 0};
    physics->friction = 0.5f;

    // Initialize render
    RenderComponent* render = GetRenderComponent(entity);
    render->tint = WHITE;
    render->sourceRect = (Rectangle){0, 0, 32, 32};
    render->origin = (Vector2){16, 16};

    // Initialize collider
    ColliderComponent* collider = GetColliderComponent(entity);
    collider->bounds = (Rectangle){position.x - 16, position.y - 16, 32, 32};
    collider->isStatic = false;

    // Initialize player control
    PlayerControlComponent* control = GetPlayerControlComponent(entity);
    control->moveSpeed = 200.0f;
    control->turnSpeed = 180.0f;

    return entity;
}

Entity* CreateNPCEntity(EntityPool* pool, Vector2 position) {
    Entity* entity = CreateEntity(pool, ENTITY_TYPE_NPC);
    if (!entity) return NULL;

    // Add required components
    AddComponent(entity, COMPONENT_TRANSFORM);
    AddComponent(entity, COMPONENT_PHYSICS);
    AddComponent(entity, COMPONENT_RENDER);
    AddComponent(entity, COMPONENT_COLLIDER);
    AddComponent(entity, COMPONENT_AI);

    // Initialize transform
    TransformComponent* transform = GetTransformComponent(entity);
    transform->position = position;
    transform->rotation = 0.0f;
    transform->scale = (Vector2){1.0f, 1.0f};

    // Initialize physics
    PhysicsComponent* physics = GetPhysicsComponent(entity);
    physics->velocity = (Vector2){0, 0};
    physics->acceleration = (Vector2){0, 0};
    physics->friction = 0.5f;

    // Initialize render
    RenderComponent* render = GetRenderComponent(entity);
    render->tint = WHITE;
    render->sourceRect = (Rectangle){0, 0, 32, 32};
    render->origin = (Vector2){16, 16};

    // Initialize collider
    ColliderComponent* collider = GetColliderComponent(entity);
    collider->bounds = (Rectangle){position.x - 16, position.y - 16, 32, 32};
    collider->isStatic = false;

    // Initialize AI
    AIComponent* ai = GetAIComponent(entity);
    ai->state = NPC_STATE_IDLE;
    ai->patrolRadius = 100.0f;
    ai->moveSpeed = 100.0f;
    ai->turnSpeed = 90.0f;
    ai->homePosition = position;

    return entity;
}

// Component iterator functions
void IterateComponents(Entity* entity, void (*callback)(void* component, ComponentFlags type)) {
    if (!entity || !callback) return;

    ComponentFlags flag = 1;
    for (int i = 0; i < MAX_COMPONENT_TYPES; i++, flag <<= 1) {
        if (entity->components & flag) {
            callback(entity->components_data[i], flag);
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