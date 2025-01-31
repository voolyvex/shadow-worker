#ifndef SHADOW_WORKER_SOUND_MANAGER_H
#define SHADOW_WORKER_SOUND_MANAGER_H

#include <raylib.h>
#include <stdbool.h>

// Sound types
typedef enum SoundType {
    SOUND_FOOTSTEP,
    SOUND_INTERACT,
    SOUND_AMBIENT,
    SOUND_COUNT
} SoundType;

// Function declarations
bool InitSoundManager(void);
void UnloadSoundManager(void);
void PlayGameSound(SoundType type);
void UpdateSoundManager(void);
void SetGameMasterVolume(float volume);

#endif // SHADOW_WORKER_SOUND_MANAGER_H 