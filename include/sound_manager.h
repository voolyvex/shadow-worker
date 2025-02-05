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

// Sound manager structure
typedef struct SoundManager {
    Music* currentMusic;
    float masterVolume;
    float musicVolume;
    float effectsVolume;
    bool isMusicEnabled;
    bool isSoundEnabled;
} SoundManager;

// Function declarations
bool InitSoundManager(void);
void UnloadSoundManager(void);

void PlayGameSound(SoundType type);
void PlayGameMusic(Music* music);
void StopGameMusic(void);
void UpdateSoundManager(void);

void SetGameMusicVolume(float volume);
void SetGameSoundVolume(float volume);
void ToggleMusic(void);
void ToggleSound(void);

// Helper functions
const char* GetSoundName(SoundType type);

#endif // SHADOW_WORKER_SOUND_MANAGER_H 