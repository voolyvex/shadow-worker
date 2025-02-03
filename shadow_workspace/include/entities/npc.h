#ifndef NPC_H
#define NPC_H

#include "../entity.h"
#include <raylib.h>

#define NPC_WIDTH 32
#define NPC_HEIGHT 32

typedef enum {
    PERSONALITY_NEUTRAL,
    PERSONALITY_FRIENDLY,
    PERSONALITY_HOSTILE,
    PERSONALITY_MYSTERIOUS
} Personality;

typedef struct {
    NPCState state;
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

// NPC Management
Entity* CreateNPC(struct EntityPool* pool, Vector2 position);
void DestroyNPC(Entity* npc);
void UnloadNPC(Entity* entity);

// NPC State Updates
void UpdateNPC(Entity* npc, struct World* world, float deltaTime);
void DrawNPC(Entity* npc);
void OnNPCCollision(Entity* self, Entity* other);

// NPC State Management
void UpdateIdleState(NPCData* data, float deltaTime);
void UpdatePatrolState(NPCData* data, float deltaTime);
void UpdateChaseState(NPCData* data, float deltaTime);
void UpdateFleeState(NPCData* data, float deltaTime);

// NPC Utility Functions
float GetDistanceToPlayer(Vector2 npcPos);
bool IsPlayerVisible(Vector2 npcPos);
Vector2 GetRandomPatrolPoint(void);
void UpdateNPCAnimation(NPCData* data, float deltaTime);

#endif // NPC_H 