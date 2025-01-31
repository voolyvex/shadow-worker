#include "../../include/resource_manager.h"
#include <stdlib.h>
#include <string.h>

static ResourceManager resourceManager = {0};

bool InitResourceManager(void) {
    // Allocate memory for textures
    resourceManager.textures = (Texture2D*)malloc(MAX_TEXTURES * sizeof(Texture2D));
    resourceManager.textureNames = (char**)malloc(MAX_TEXTURES * sizeof(char*));
    if (!resourceManager.textures || !resourceManager.textureNames) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for textures");
        return false;
    }
    resourceManager.textureCount = 0;
    
    // Allocate memory for sounds
    resourceManager.sounds = (Sound*)malloc(MAX_SOUNDS * sizeof(Sound));
    resourceManager.soundNames = (char**)malloc(MAX_SOUNDS * sizeof(char*));
    if (!resourceManager.sounds || !resourceManager.soundNames) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for sounds");
        return false;
    }
    resourceManager.soundCount = 0;
    
    // Allocate memory for music
    resourceManager.music = (Music*)malloc(MAX_MUSIC * sizeof(Music));
    resourceManager.musicNames = (char**)malloc(MAX_MUSIC * sizeof(char*));
    if (!resourceManager.music || !resourceManager.musicNames) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for music");
        return false;
    }
    resourceManager.musicCount = 0;
    
    return true;
}

void UnloadResourceManager(void) {
    // Unload textures
    for (int i = 0; i < resourceManager.textureCount; i++) {
        UnloadTexture(resourceManager.textures[i]);
        free(resourceManager.textureNames[i]);
    }
    free(resourceManager.textures);
    free(resourceManager.textureNames);
    
    // Unload sounds
    for (int i = 0; i < resourceManager.soundCount; i++) {
        UnloadSound(resourceManager.sounds[i]);
        free(resourceManager.soundNames[i]);
    }
    free(resourceManager.sounds);
    free(resourceManager.soundNames);
    
    // Unload music
    for (int i = 0; i < resourceManager.musicCount; i++) {
        UnloadMusicStream(resourceManager.music[i]);
        free(resourceManager.musicNames[i]);
    }
    free(resourceManager.music);
    free(resourceManager.musicNames);
}

ResourceManager* GetResourceManager(void) {
    return &resourceManager;
}

Texture2D* LoadGameTexture(const char* name, const char* filename) {
    if (resourceManager.textureCount >= MAX_TEXTURES) {
        TraceLog(LOG_ERROR, "Maximum texture count reached");
        return NULL;
    }
    
    // Check if texture already exists
    for (int i = 0; i < resourceManager.textureCount; i++) {
        if (strcmp(resourceManager.textureNames[i], name) == 0) {
            return &resourceManager.textures[i];
        }
    }
    
    // Load new texture
    Texture2D texture = LoadTexture(filename);
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", filename);
        return NULL;
    }
    
    // Store texture
    resourceManager.textures[resourceManager.textureCount] = texture;
    resourceManager.textureNames[resourceManager.textureCount] = strdup(name);
    return &resourceManager.textures[resourceManager.textureCount++];
}

Sound* LoadGameSound(const char* name, const char* filename) {
    if (resourceManager.soundCount >= MAX_SOUNDS) {
        TraceLog(LOG_ERROR, "Maximum sound count reached");
        return NULL;
    }
    
    // Check if sound already exists
    for (int i = 0; i < resourceManager.soundCount; i++) {
        if (strcmp(resourceManager.soundNames[i], name) == 0) {
            return &resourceManager.sounds[i];
        }
    }
    
    // Load new sound
    Sound sound = LoadSound(filename);
    if (sound.frameCount == 0) {
        TraceLog(LOG_ERROR, "Failed to load sound: %s", filename);
        return NULL;
    }
    
    // Store sound
    resourceManager.sounds[resourceManager.soundCount] = sound;
    resourceManager.soundNames[resourceManager.soundCount] = strdup(name);
    return &resourceManager.sounds[resourceManager.soundCount++];
}

Music* LoadGameMusic(const char* name, const char* filename) {
    if (resourceManager.musicCount >= MAX_MUSIC) {
        TraceLog(LOG_ERROR, "Maximum music count reached");
        return NULL;
    }
    
    // Check if music already exists
    for (int i = 0; i < resourceManager.musicCount; i++) {
        if (strcmp(resourceManager.musicNames[i], name) == 0) {
            return &resourceManager.music[i];
        }
    }
    
    // Load new music
    Music music = LoadMusicStream(filename);
    if (music.frameCount == 0) {
        TraceLog(LOG_ERROR, "Failed to load music: %s", filename);
        return NULL;
    }
    
    // Store music
    resourceManager.music[resourceManager.musicCount] = music;
    resourceManager.musicNames[resourceManager.musicCount] = strdup(name);
    return &resourceManager.music[resourceManager.musicCount++];
}

Texture2D* GetTexture(const char* name) {
    for (int i = 0; i < resourceManager.textureCount; i++) {
        if (strcmp(resourceManager.textureNames[i], name) == 0) {
            return &resourceManager.textures[i];
        }
    }
    return NULL;
}

Sound* GetSound(const char* name) {
    for (int i = 0; i < resourceManager.soundCount; i++) {
        if (strcmp(resourceManager.soundNames[i], name) == 0) {
            return &resourceManager.sounds[i];
        }
    }
    return NULL;
}

Music* GetMusic(const char* name) {
    for (int i = 0; i < resourceManager.musicCount; i++) {
        if (strcmp(resourceManager.musicNames[i], name) == 0) {
            return &resourceManager.music[i];
        }
    }
    return NULL;
} 