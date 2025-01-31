#ifndef SHADOW_WORKER_ENTITY_H
#define SHADOW_WORKER_ENTITY_H

#include <raylib.h>
#include <stdbool.h>
#include "resource_manager.h"

// Forward declarations
struct ResourceManager;
struct Game;

// Entity types
typedef enum EntityType {
    ENTITY_PLAYER,
    ENTITY_NPC,
    ENTITY_ITEM,
    ENTITY_OBSTACLE
} EntityType;

// Entity states
typedef enum EntityState {
    ENTITY_IDLE,
    ENTITY_MOVING,
    ENTITY_INTERACTING,
    ENTITY_DEAD
} EntityState;

// Psychological traits
typedef struct PsychologicalProfile {
    int enneagramType;    // 1-9
    float shadowIntensity; // 0.0-1.0
    float resonanceLevel;  // 0.0-1.0
    float consciousness;   // 0.0-1.0
    bool isResonating;
} PsychologicalProfile;

// Entity structure
typedef struct Entity {
    EntityType type;
    EntityState state;
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float scale;
    Rectangle bounds;
    Texture2D* sprite;
    bool isActive;
    
    // Psychological components
    PsychologicalProfile* psyche;
    
    // Function pointers for behavior
    void (*Update)(struct Entity* self, float deltaTime);
    void (*Draw)(struct Entity* self);
    void (*OnCollision)(struct Entity* self, struct Entity* other);
    void (*OnInteraction)(struct Entity* self, struct Entity* other);
    
    // Entity-specific data pointer
    void* data; // Type-specific data
} Entity;

// Entity pool for efficient management
typedef struct EntityPool {
    Entity* entities;
    int capacity;
    int count;
} EntityPool;

// Function declarations
EntityPool* InitEntityPool(int capacity);
void UnloadEntityPool(EntityPool* pool);
Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position);
void UpdateEntities(EntityPool* pool, float deltaTime);
void DrawEntities(EntityPool* pool);
void ProcessEntityInteractions(EntityPool* pool);

// Entity factory functions
Entity* CreatePlayer(EntityPool* pool, Vector2 position, ResourceManager* resources);
Entity* CreateNPC(EntityPool* pool, Vector2 position, ResourceManager* resources, int enneagramType);
Entity* CreateObject(EntityPool* pool, Vector2 position, ResourceManager* resources, const char* objectType);

// Collision and interaction
bool CheckEntityCollision(Entity* e1, Entity* e2);
void HandleEntityCollision(Entity* e1, Entity* e2);

// Psychological system functions
void UpdatePsychologicalState(Entity* entity, float deltaTime);
void HandleResonance(Entity* source, Entity* target);
float CalculateResonanceStrength(PsychologicalProfile* p1, PsychologicalProfile* p2);

// Utility functions
void SetEntityState(Entity* entity, EntityState newState);
void MoveEntity(Entity* entity, Vector2 direction, float speed);
Entity* FindNearestEntity(EntityPool* pool, Vector2 position, float radius, EntityType type);

// Resource loading functions
Texture2D* LoadEntityTexture(struct ResourceManager* resources, const char* name);
Sound* LoadEntitySound(struct ResourceManager* resources, const char* name);
Shader* LoadEntityShader(struct ResourceManager* resources, const char* name, const char* vsPath);

#endif // SHADOW_WORKER_ENTITY_H 