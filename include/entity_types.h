#ifndef ENTITY_TYPES_H
#define ENTITY_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "constants.h"

#define MAX_COMPONENTS 6

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct World;

// Component management function declarations
struct Entity;  // Forward declaration
void AddComponent(struct Entity* entity, enum ComponentFlags component);
void RemoveComponent(struct Entity* entity, enum ComponentFlags component);
bool HasComponent(const struct Entity* entity, enum ComponentFlags component);

// Entity types
typedef enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_OBJECT,
    ENTITY_TYPE_COUNT
} EntityType;

// Entity component flags
typedef enum {
    COMPONENT_NONE = 0,
    COMPONENT_TRANSFORM = 1 << 0,
    COMPONENT_PHYSICS = 1 << 1,
    COMPONENT_RENDER = 1 << 2,
    COMPONENT_COLLIDER = 1 << 3,
    COMPONENT_AI = 1 << 4,
    COMPONENT_PLAYER_CONTROL = 1 << 5
} ComponentFlags;

// NPC States
typedef enum {
    NPC_STATE_NONE = 0,
    NPC_STATE_IDLE,
    NPC_STATE_PATROL,
    NPC_STATE_CHASE,
    NPC_STATE_FLEE
} NPCState;

// Component type definitions
typedef struct {
    Vector2 position;
    float rotation;
    Vector2 scale;
} TransformComponent;

typedef struct {
    Vector2 velocity;
    Vector2 acceleration;
    float friction;
} PhysicsComponent;

typedef struct {
    Texture2D* texture;
    Color tint;
    Rectangle sourceRect;
    Vector2 origin;
} RenderComponent;

typedef struct {
    Rectangle bounds;
    bool isStatic;
} ColliderComponent;

typedef struct {
    float patrolRadius;
    Vector2 homePosition;
    bool isAggressive;
    NPCState state;
    float stateTimer;
    float moveSpeed;
    float turnSpeed;
    bool hasTarget;
    Vector2 lastKnownTargetPos;
    Vector2 targetPosition;
    int animationFrame;
} AIComponent;

typedef struct {
    float moveSpeed;
    float turnSpeed;
    bool isInteracting;
} PlayerControlComponent;

// Component data union
typedef union {
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;
    ColliderComponent collider;
    AIComponent ai;
    PlayerControlComponent playerControl;
} ComponentData;

// Entity definition
typedef struct Entity {
    EntityType type;
    ComponentFlags components;
    bool active;
    Vector2 position;
    Rectangle bounds;
    Rectangle collider;
    Color color;
    float rotation;
    float scale;
    bool visible;
    ComponentData components_data[MAX_COMPONENTS];
    void (*Update)(struct Entity* entity, struct World* world, float deltaTime);
    void (*Draw)(struct Entity* entity);
    void (*OnCollision)(struct Entity* entity, struct Entity* other);
    void (*OnDestroy)(struct Entity* entity);
} Entity;

#ifdef __cplusplus
}
#endif

#endif // ENTITY_TYPES_H 