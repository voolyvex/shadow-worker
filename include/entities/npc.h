#ifndef SHADOW_WORKER_NPC_H
#define SHADOW_WORKER_NPC_H

#include "../entity.h"
#include "../resource_manager.h"

// NPC states
typedef enum NPCState {
    NPC_IDLE,
    NPC_TALKING,
    NPC_WALKING
} NPCState;

// Function declarations
Entity* CreateNPC(EntityPool* pool, Vector2 position, ResourceManager* resources);
void UnloadNPC(Entity* npc);

#endif // SHADOW_WORKER_NPC_H 