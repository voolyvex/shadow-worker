#include "../../include/sound_manager.h"
#include <stdlib.h>

// Sound data structure
typedef struct {
    Sound sound;
    float volume;
    float pitch;
    bool isLooping;
    float minInterval;  // Minimum time between sound plays
    float lastPlayTime; // Time of last play
} GameSound;

// Sound manager state
static struct {
    GameSound sounds[SOUND_COUNT];
    Music ambientMusic;
    float masterVolume;
    bool isInitialized;
    float gameTime;     // Track game time for sound intervals
} soundManager;

// Sound file paths
static const char* soundPaths[SOUND_COUNT] = {
    "resources/sounds/footstep.wav",
    "resources/sounds/interact.wav",
    "resources/sounds/ambient.wav"
};

bool InitSoundManager(void) {
    InitAudioDevice();
    
    soundManager.masterVolume = 1.0f;
    soundManager.isInitialized = true;
    soundManager.gameTime = 0.0f;
    
    // Load footstep sound
    if (!FileExists(soundPaths[SOUND_FOOTSTEP])) {
        TraceLog(LOG_WARNING, "Footstep sound file not found: %s", soundPaths[SOUND_FOOTSTEP]);
        return false;
    }
    soundManager.sounds[SOUND_FOOTSTEP].sound = LoadSound(soundPaths[SOUND_FOOTSTEP]);
    soundManager.sounds[SOUND_FOOTSTEP].volume = 0.5f;
    soundManager.sounds[SOUND_FOOTSTEP].pitch = 1.0f;
    soundManager.sounds[SOUND_FOOTSTEP].isLooping = false;
    soundManager.sounds[SOUND_FOOTSTEP].minInterval = 0.2f;  // 200ms between footsteps
    soundManager.sounds[SOUND_FOOTSTEP].lastPlayTime = 0.0f;
    
    // Load interaction sound
    if (!FileExists(soundPaths[SOUND_INTERACT])) {
        TraceLog(LOG_WARNING, "Interaction sound file not found: %s", soundPaths[SOUND_INTERACT]);
        return false;
    }
    soundManager.sounds[SOUND_INTERACT].sound = LoadSound(soundPaths[SOUND_INTERACT]);
    soundManager.sounds[SOUND_INTERACT].volume = 0.7f;
    soundManager.sounds[SOUND_INTERACT].pitch = 1.0f;
    soundManager.sounds[SOUND_INTERACT].isLooping = false;
    soundManager.sounds[SOUND_INTERACT].minInterval = 0.0f;
    soundManager.sounds[SOUND_INTERACT].lastPlayTime = 0.0f;
    
    // Load and start ambient music
    if (!FileExists(soundPaths[SOUND_AMBIENT])) {
        TraceLog(LOG_WARNING, "Ambient music file not found: %s", soundPaths[SOUND_AMBIENT]);
        return false;
    }
    soundManager.ambientMusic = LoadMusicStream(soundPaths[SOUND_AMBIENT]);
    SetMusicVolume(soundManager.ambientMusic, 0.3f * soundManager.masterVolume);
    PlayMusicStream(soundManager.ambientMusic);
    
    return true;
}

void UnloadSoundManager(void) {
    if (!soundManager.isInitialized) return;
    
    // Unload all sounds
    for (int i = 0; i < SOUND_COUNT - 1; i++) {
        UnloadSound(soundManager.sounds[i].sound);
    }
    
    // Unload ambient music
    StopMusicStream(soundManager.ambientMusic);
    UnloadMusicStream(soundManager.ambientMusic);
    
    CloseAudioDevice();
    soundManager.isInitialized = false;
}

void PlayGameSound(SoundType type) {
    if (!soundManager.isInitialized || type >= SOUND_COUNT) return;
    
    GameSound* sound = &soundManager.sounds[type];
    
    // Check if enough time has passed since last play
    if (soundManager.gameTime - sound->lastPlayTime < sound->minInterval) {
        return;
    }
    
    SetSoundVolume(sound->sound, sound->volume * soundManager.masterVolume);
    SetSoundPitch(sound->sound, sound->pitch);
    PlaySound(sound->sound);
    
    sound->lastPlayTime = soundManager.gameTime;
}

void UpdateSoundManager(void) {
    if (!soundManager.isInitialized) return;
    
    // Update game time
    soundManager.gameTime += GetFrameTime();
    
    // Update ambient music
    UpdateMusicStream(soundManager.ambientMusic);
}

void SetGameMasterVolume(float volume) {
    soundManager.masterVolume = volume;
    
    // Update all sound volumes
    for (int i = 0; i < SOUND_COUNT - 1; i++) {
        SetSoundVolume(soundManager.sounds[i].sound, 
                      soundManager.sounds[i].volume * soundManager.masterVolume);
    }
    
    // Update ambient music volume
    SetMusicVolume(soundManager.ambientMusic, 0.3f * soundManager.masterVolume);
} 