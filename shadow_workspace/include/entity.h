#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include <stdbool.h>
#include "entity_types.h"
#include "entity_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

// Entity type definitions
// typedef enum {
//     ENTITY_NONE,
//     ENTITY_PLAYER,
//     ENTITY_NPC,
//     ENTITY_ITEM
// } EntityType;

// Forward declarations
struct World;
struct EntityPool;

// Entity management functions
Entity* CreateEntity(EntityPool* pool, EntityType type);
void DestroyEntity(Entity* entity);
void UpdateEntity(Entity* entity, struct World* world, float deltaTime);
void DrawEntity(Entity* entity);

// Component access functions
TransformComponent* GetTransformComponent(Entity* entity);
PhysicsComponent* GetPhysicsComponent(Entity* entity);
RenderComponent* GetRenderComponent(Entity* entity);
ColliderComponent* GetColliderComponent(Entity* entity);
AIComponent* GetAIComponent(Entity* entity);
PlayerControlComponent* GetPlayerControlComponent(Entity* entity);

// Entity functions
void UpdateEntityPosition(Entity* entity, Vector2 newPosition);

// Component functions
void AddComponent(Entity* entity, ComponentFlags component);
void RemoveComponent(Entity* entity, ComponentFlags component);
bool HasComponent(const Entity* entity, ComponentFlags component);

#ifdef __cplusplus
}
#endif

#endif // ENTITY_H 