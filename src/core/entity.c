#include "entities/entity.h"
#include "entities/entity_pool.h"
#include "utils/logger.h"
#include <stdlib.h>
#include <string.h>

EntityPool* InitEntityPool(void) {
    EntityPool* pool = (EntityPool*)malloc(sizeof(EntityPool));
    if (!pool) {
        LOG_ERROR(LOG_ENTITY, "Failed to allocate entity pool");
        return NULL;
    }
    
    pool->entities = (Entity*)calloc(MAX_ENTITIES, sizeof(Entity));
    if (!pool->entities) {
        LOG_ERROR(LOG_ENTITY, "Failed to allocate entity array");
        free(pool);
        return NULL;
    }
    
    // Initialize all entities
    for (int i = 0; i < MAX_ENTITIES; i++) {
        pool->entities[i].type = ENTITY_TYPE_NONE;
        pool->entities[i].components = COMPONENT_NONE;
        pool->entities[i].active = false;
        memset(&pool->entities[i].components_data, 0, sizeof(pool->entities[i].components_data));
        pool->entities[i].Update = NULL;
        pool->entities[i].Draw = NULL;
        pool->entities[i].OnCollision = NULL;
        pool->entities[i].OnDestroy = NULL;
    }
    
    pool->capacity = MAX_ENTITIES;
    pool->count = 0;
    
    LOG_INFO(LOG_ENTITY, "Entity pool initialized with capacity %d", MAX_ENTITIES);
    return pool;
}

void UnloadEntityPool(EntityPool* pool) {
    if (!pool) return;
    
    // Call destroy callbacks for active entities
    for (int i = 0; i < pool->count; i++) {
        if (pool->entities[i].active && pool->entities[i].OnDestroy) {
            pool->entities[i].OnDestroy(&pool->entities[i]);
        }
    }
    
    free(pool->entities);
    free(pool);
    
    LOG_INFO(LOG_ENTITY, "Entity pool unloaded");
}

Entity* CreateEntity(EntityPool* pool, EntityType type) {
    if (!pool || pool->count >= pool->capacity) {
        return NULL;
    }
    
    Entity* entity = &pool->entities[pool->count++];
    entity->type = type;
    entity->components = COMPONENT_NONE;
    entity->active = true;
    
    // Initialize component data array
    memset(&entity->components_data, 0, sizeof(entity->components_data));
    
    return entity;
}

void DestroyEntity(EntityPool* pool, Entity* entity) {
    if (!entity) return;
    
    if (entity->OnDestroy) {
        entity->OnDestroy(entity);
    }
    
    entity->active = false;
    entity->components = COMPONENT_NONE;
    memset(&entity->components_data, 0, sizeof(entity->components_data));
}

void UpdateEntity(Entity* entity, struct World* world, float deltaTime) {
    if (!entity || !entity->active) return;
    
    // Update physics
    if (HasComponent(entity, COMPONENT_PHYSICS)) {
        PhysicsComponent* physics = &entity->components_data[COMPONENT_PHYSICS].physics;
        TransformComponent* transform = &entity->components_data[COMPONENT_TRANSFORM].transform;
        
        // Apply acceleration
        physics->velocity.x += physics->acceleration.x * deltaTime;
        physics->velocity.y += physics->acceleration.y * deltaTime;
        
        // Apply friction
        physics->velocity.x *= physics->friction;
        physics->velocity.y *= physics->friction;
        
        // Update position
        transform->position.x += physics->velocity.x * deltaTime;
        transform->position.y += physics->velocity.y * deltaTime;
    }
    
    // Call entity's update function
    if (entity->Update) {
        entity->Update(entity, world, deltaTime);
    }
}

void DrawEntity(Entity* entity) {
    if (!entity || !entity->active) return;
    
    if (entity->Draw) {
        entity->Draw(entity);
    }
}

Entity* GetEntityByType(EntityPool* pool, EntityType type) {
    if (!pool) return NULL;
    
    for (int i = 0; i < pool->count; i++) {
        if (pool->entities[i].type == type) {
            return &pool->entities[i];
        }
    }
    
    return NULL;
}

bool CheckEntityCollision(Entity* entity1, Entity* entity2) {
    if (!entity1 || !entity2 || !entity1->active || !entity2->active) return false;
    
    return CheckCollisionRecs(entity1->bounds, entity2->bounds);
}

void HandleEntityCollision(Entity* entity1, Entity* entity2) {
    if (!entity1 || !entity2 || !entity1->active || !entity2->active) return;
    
    if (entity1->OnCollision) {
        entity1->OnCollision(entity1, entity2);
    }
    if (entity2->OnCollision) {
        entity2->OnCollision(entity2, entity1);
    }
}

void UpdateEntityPosition(Entity* entity, Vector2 newPosition) {
    if (!entity) return;
    
    entity->position = newPosition;
    entity->bounds.x = newPosition.x;
    entity->bounds.y = newPosition.y;
}

// Component access functions
TransformComponent* GetTransformComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_TRANSFORM)) return NULL;
    return &entity->components_data[COMPONENT_TRANSFORM].transform;
}

PhysicsComponent* GetPhysicsComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_PHYSICS)) return NULL;
    return &entity->components_data[COMPONENT_PHYSICS].physics;
}

RenderComponent* GetRenderComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_RENDER)) return NULL;
    return &entity->components_data[COMPONENT_RENDER].render;
}

ColliderComponent* GetColliderComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_COLLIDER)) return NULL;
    return &entity->components_data[COMPONENT_COLLIDER].collider;
}

AIComponent* GetAIComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_AI)) return NULL;
    return &entity->components_data[COMPONENT_AI].ai;
}

PlayerControlComponent* GetPlayerControlComponent(Entity* entity) {
    if (!entity || !HasComponent(entity, COMPONENT_PLAYER_CONTROL)) return NULL;
    return &entity->components_data[COMPONENT_PLAYER_CONTROL].playerControl;
}

// Component management functions
void AddComponent(Entity* entity, ComponentFlags component) {
    if (!entity) return;
    entity->components |= component;
}

void RemoveComponent(Entity* entity, ComponentFlags component) {
    if (!entity) return;
    entity->components &= ~component;
}

bool HasComponent(const Entity* entity, ComponentFlags component) {
    if (!entity) return false;
    return (entity->components & component) == component;
}

void InitEntity(Entity* entity) {
    if (!entity) return;
    
    entity->type = ENTITY_TYPE_NONE;
    entity->components = COMPONENT_NONE;
    entity->active = false;
    entity->position = (Vector2){0, 0};
    entity->bounds = (Rectangle){0, 0, 0, 0};
    entity->collider = (Rectangle){0, 0, 0, 0};
    entity->color = WHITE;
    entity->rotation = 0;
    entity->scale = 1.0f;
    entity->visible = true;
    
    // Initialize function pointers
    entity->Update = NULL;
    entity->Draw = NULL;
    entity->OnCollision = NULL;
    entity->OnDestroy = NULL;
}

void InitTransformComponent(Entity* entity, Vector2 position) {
    if (!entity) return;
    
    TransformComponent* transform = &entity->components_data[COMPONENT_TRANSFORM].transform;
    transform->position = position;
    transform->rotation = 0.0f;
    transform->scale = 1.0f;
    
    AddComponent(entity, COMPONENT_TRANSFORM);
}

void InitColliderComponent(Entity* entity, Rectangle bounds, bool isStatic) {
    if (!entity) return;
    
    ColliderComponent* collider = &entity->components_data[COMPONENT_COLLIDER].collider;
    collider->bounds = bounds;
    collider->isStatic = isStatic;
    collider->isTrigger = false;
    collider->isEnabled = true;
    
    AddComponent(entity, COMPONENT_COLLIDER);
}

void InitPhysicsComponent(Entity* entity) {
    if (!entity) return;
    
    PhysicsComponent* physics = &entity->components_data[COMPONENT_PHYSICS].physics;
    physics->velocity = (Vector2){ 0.0f, 0.0f };
    physics->acceleration = (Vector2){ 0.0f, 0.0f };
    physics->friction = 0.8f;
    physics->mass = 1.0f;
    physics->isKinematic = false;
    
    AddComponent(entity, COMPONENT_PHYSICS);
}

void InitRenderComponent(Entity* entity, Texture2D* texture) {
    if (!entity) return;
    
    RenderComponent* render = &entity->components_data[COMPONENT_RENDER].render;
    render->texture = texture;
    render->color = WHITE;
    render->sourceRect = (Rectangle){ 0, 0, texture->width, texture->height };
    render->origin = (Vector2){ texture->width / 2.0f, texture->height / 2.0f };
    render->visible = true;
    render->opacity = 1.0f;
    
    AddComponent(entity, COMPONENT_RENDER);
}

void InitializeComponents(Entity* entity) {
    if (!entity) return;
    
    // Initialize transform component
    if (HasComponent(entity, COMPONENT_TRANSFORM)) {
        TransformComponent* transform = GetTransformComponent(entity);
        transform->position = (Vector2){0, 0};
        transform->rotation = 0.0f;
        transform->scale = 1.0f;
    }
    
    // Initialize physics component
    if (HasComponent(entity, COMPONENT_PHYSICS)) {
        PhysicsComponent* physics = GetPhysicsComponent(entity);
        physics->velocity = (Vector2){0, 0};
        physics->acceleration = (Vector2){0, 0};
        physics->friction = 0.5f;
        physics->mass = 1.0f;
        physics->isKinematic = false;
    }
    
    // Initialize render component
    if (HasComponent(entity, COMPONENT_RENDER)) {
        RenderComponent* render = GetRenderComponent(entity);
        render->color = WHITE;
        render->visible = true;
        render->opacity = 1.0f;
    }
    
    // Initialize collider component
    if (HasComponent(entity, COMPONENT_COLLIDER)) {
        ColliderComponent* collider = GetColliderComponent(entity);
        collider->isStatic = false;
        collider->isTrigger = false;
        collider->isEnabled = true;
    }
}