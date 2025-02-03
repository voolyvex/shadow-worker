#ifndef SHADOW_WORKER_PLAYER_H
#define SHADOW_WORKER_PLAYER_H

#include <raylib.h>

typedef struct PlayerData {
    Vector2 position;
    float speed;
    int health;
    int stamina;
    int resonance;
    bool isMoving;
    Vector2 velocity;
    float footstepTimer;
    float footstepInterval;
} PlayerData;

PlayerData* CreatePlayer(void);
void DestroyPlayer(void);
void UpdatePlayer(float deltaTime);
void DrawPlayer(void);
float PlayerClamp(float value, float min, float max);

#endif // SHADOW_WORKER_PLAYER_H 