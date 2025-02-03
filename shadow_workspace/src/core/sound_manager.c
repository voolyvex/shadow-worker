#include "../../include/sound_manager.h"
#include "../../include/resource_manager.h"
#include "logger.h"
#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

static SoundManager soundManager = {0};
static bool isInitialized = false;

const char* GetSoundName(SoundType type) {
    switch (type) {
        case SOUND_FOOTSTEP: return "footstep";
        case SOUND_INTERACT: return "interact";
        case SOUND_AMBIENT: return "ambient";
        default: return NULL;
    }
}

bool InitSoundManager(void) {
    if (isInitialized) {
        LOG_WARN(LOG_AUDIO, "Sound manager already initialized");
        return true;
    }
    
    LOG_INFO(LOG_AUDIO, "Initializing sound manager");
    Logger_BeginTimer("sound_manager_init");
    
    // Initialize audio device
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        LOG_ERROR(LOG_AUDIO, "Failed to initialize audio device");
        return false;
    }
    LOG_INFO(LOG_AUDIO, "Audio device initialized successfully");
    
    // Set audio configuration
    SetMasterVolume(1.0f);
    
    // Initialize sound manager state
    memset(&soundManager, 0, sizeof(SoundManager));
    soundManager.masterVolume = 1.0f;
    soundManager.musicVolume = 1.0f;
    soundManager.effectsVolume = 1.0f;
    soundManager.currentMusic = NULL;
    
    isInitialized = true;
    
    Logger_EndTimer("sound_manager_init");
    LOG_INFO(LOG_AUDIO, "Sound manager initialized successfully");
    
    return true;
}

void UnloadSoundManager(void) {
    if (!isInitialized) {
        LOG_WARN(LOG_AUDIO, "Attempting to unload uninitialized sound manager");
        return;
    }
    
    LOG_INFO(LOG_AUDIO, "Unloading sound manager");
    Logger_BeginTimer("sound_manager_unload");
    
    if (soundManager.currentMusic) {
        LOG_DEBUG(LOG_AUDIO, "Stopping current music stream");
        StopMusicStream(*soundManager.currentMusic);
        soundManager.currentMusic = NULL;
    }
    
    CloseAudioDevice();
    isInitialized = false;
    
    Logger_EndTimer("sound_manager_unload");
    LOG_INFO(LOG_AUDIO, "Sound manager unloaded successfully");
}

void PlayGameSound(SoundType type) {
    if (!isInitialized || !soundManager.isSoundEnabled || type >= SOUND_COUNT) {
        LOG_WARN(LOG_AUDIO, "Cannot play sound: %s", 
                !isInitialized ? "manager not initialized" : 
                !soundManager.isSoundEnabled ? "sound disabled" : 
                "invalid sound type");
        return;
    }
    
    const char* soundName = GetSoundName(type);
    if (!soundName) {
        LOG_ERROR(LOG_AUDIO, "Invalid sound type: %d", type);
        return;
    }
    
    Logger_BeginTimer("sound_play");
    
    Sound sound = LoadSound(soundName);
    if (sound.frameCount > 0) {
        SetSoundVolume(sound, soundManager.effectsVolume);
        PlaySound(sound);
        LOG_DEBUG(LOG_AUDIO, "Playing sound: %s (volume: %.2f)", soundName, soundManager.effectsVolume);
    } else {
        LOG_ERROR(LOG_AUDIO, "Failed to play sound '%s' - sound not loaded or invalid", soundName);
    }
    
    Logger_EndTimer("sound_play");
}

void PlayGameMusic(Music* music) {
    if (!isInitialized || !music || !soundManager.isMusicEnabled) {
        LOG_WARN(LOG_AUDIO, "Cannot play music: %s",
                !isInitialized ? "manager not initialized" :
                !music ? "invalid music pointer" :
                "music disabled");
        return;
    }
    
    Logger_BeginTimer("music_play");
    
    if (soundManager.currentMusic) {
        LOG_DEBUG(LOG_AUDIO, "Stopping current music stream before playing new one");
        StopMusicStream(*soundManager.currentMusic);
    }
    
    if (music->ctxData == NULL) {
        LOG_ERROR(LOG_AUDIO, "Attempted to play invalid music stream");
        Logger_EndTimer("music_play");
        return;
    }
    
    soundManager.currentMusic = music;
    PlayMusicStream(*music);
    SetMusicVolume(*music, soundManager.musicVolume);
    LOG_INFO(LOG_AUDIO, "Playing music stream (volume: %.2f)", soundManager.musicVolume);
    
    Logger_EndTimer("music_play");
}

void StopGameMusic(void) {
    if (!isInitialized || !soundManager.currentMusic) {
        LOG_DEBUG(LOG_AUDIO, "No music to stop");
        return;
    }
    
    LOG_DEBUG(LOG_AUDIO, "Stopping music stream");
    StopMusicStream(*soundManager.currentMusic);
    soundManager.currentMusic = NULL;
}

void UpdateSoundManager(void) {
    if (!isInitialized) return;
    
    if (soundManager.currentMusic && soundManager.isMusicEnabled) {
        UpdateMusicStream(*soundManager.currentMusic);
    }
}

void SetGameMusicVolume(float volume) {
    if (!isInitialized) {
        LOG_WARN(LOG_AUDIO, "Cannot set music volume: manager not initialized");
        return;
    }
    
    LOG_DEBUG(LOG_AUDIO, "Setting music volume: %.2f", volume);
    soundManager.musicVolume = volume;
    if (soundManager.currentMusic) {
        SetMusicVolume(*soundManager.currentMusic, volume);
    }
}

void SetGameSoundVolume(float volume) {
    if (!isInitialized) {
        LOG_WARN(LOG_AUDIO, "Cannot set sound volume: manager not initialized");
        return;
    }
    
    LOG_DEBUG(LOG_AUDIO, "Setting sound volume: %.2f", volume);
    soundManager.effectsVolume = volume;
    SetMasterVolume(volume);
}

void ToggleMusic(void) {
    if (!isInitialized) {
        LOG_WARN(LOG_AUDIO, "Cannot toggle music: manager not initialized");
        return;
    }
    
    soundManager.isMusicEnabled = !soundManager.isMusicEnabled;
    LOG_INFO(LOG_AUDIO, "Music %s", soundManager.isMusicEnabled ? "enabled" : "disabled");
    
    if (!soundManager.isMusicEnabled && soundManager.currentMusic) {
        StopMusicStream(*soundManager.currentMusic);
    } else if (soundManager.isMusicEnabled && soundManager.currentMusic) {
        PlayMusicStream(*soundManager.currentMusic);
    }
}

void ToggleSound(void) {
    if (!isInitialized) {
        LOG_WARN(LOG_AUDIO, "Cannot toggle sound: manager not initialized");
        return;
    }
    
    soundManager.isSoundEnabled = !soundManager.isSoundEnabled;
    LOG_INFO(LOG_AUDIO, "Sound effects %s", soundManager.isSoundEnabled ? "enabled" : "disabled");
} 