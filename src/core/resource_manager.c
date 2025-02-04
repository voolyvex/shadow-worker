#include "../../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include "../../include/resource_manager.h"
#include "../../include/sound_manager.h"
#include "../../include/logger.h"
#include "../../include/resource_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Audio format definitions
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2

static ResourceManager g_resourceManager = {0};

// Resource validation configuration
typedef struct ValidationConfig {
    const char** supportedImageExtensions;
    size_t supportedImageCount;
    const char** supportedAudioExtensions;
    size_t supportedAudioCount;
    const char** supportedFontExtensions;
    size_t supportedFontCount;
    const char** supportedShaderExtensions;
    size_t supportedShaderCount;
    size_t maxTextureSize;
    size_t maxAudioSize;
    size_t maxFontSize;
    size_t maxShaderSize;
} ValidationConfig;

static ValidationConfig validationConfig = {0};

// Resource validation functions
static bool ValidateFileExtension(const char* path, const char** supportedExtensions, size_t supportedCount) {
    const char* extension = GetFileExtension(path);
    if (!extension) return false;
    
    for (size_t i = 0; i < supportedCount; i++) {
        if (TextIsEqual(extension, supportedExtensions[i])) {
            return true;
        }
    }
    return false;
}

static bool ValidateFileSize(const char* path, size_t maxSize) {
    if (!FileExists(path)) return false;
    
    size_t fileSize = GetFileLength(path);
    return fileSize <= maxSize;
}

// Resource manager functions
ResourceManager* GetResourceManager(void) {
    return &g_resourceManager;
}

ResourceManager* CreateResourceManager(void) {
    ResourceManager* manager = (ResourceManager*)calloc(1, sizeof(ResourceManager));
    if (!manager) return NULL;
    
    manager->textures = (TextureResource*)calloc(MAX_TEXTURES, sizeof(TextureResource));
    manager->waves = (WaveResource*)calloc(MAX_SOUNDS, sizeof(WaveResource));
    manager->music = (MusicResource*)calloc(MAX_MUSIC, sizeof(MusicResource));
    manager->fonts = (FontResource*)calloc(MAX_FONTS, sizeof(FontResource));
    manager->shaders = (ShaderResource*)calloc(MAX_SHADERS, sizeof(ShaderResource));
    
    if (!manager->textures || !manager->waves || !manager->music ||
        !manager->fonts || !manager->shaders) {
        UnloadAllResources(manager);
        free(manager);
        return NULL;
    }

    manager->textureCount = 0;
    manager->waveCount = 0;
    manager->musicCount = 0;
    manager->fontCount = 0;
    manager->shaderCount = 0;
    
    manager->textureCapacity = MAX_TEXTURES;
    manager->waveCapacity = MAX_SOUNDS;
    manager->musicCapacity = MAX_MUSIC;
    manager->fontCapacity = MAX_FONTS;
    manager->shaderCapacity = MAX_SHADERS;
    
    manager->initialized = true;
    
    return manager;
}

void DestroyResourceManager(ResourceManager* manager) {
    if (!manager) return;
    UnloadAllResources(manager);
}

void UnloadResourceManager(void) {
    UnloadAllResources(&g_resourceManager);
}

void UnloadAllResources(ResourceManager* manager) {
    if (!manager) return;
    
    // Unload textures
    for (int i = 0; i < manager->textureCount; i++) {
        UnloadTexture(manager->textures[i].texture);
        free((void*)manager->textures[i].name);
    }
    
    // Unload waves
    for (int i = 0; i < manager->waveCount; i++) {
        UnloadWave(manager->waves[i].wave);
        free((void*)manager->waves[i].name);
    }
    
    // Unload music
    for (int i = 0; i < manager->musicCount; i++) {
        UnloadMusicStream(manager->music[i].music);
        free((void*)manager->music[i].name);
    }
    
    // Unload fonts
    for (int i = 0; i < manager->fontCount; i++) {
        UnloadFont(manager->fonts[i].font);
        free((void*)manager->fonts[i].name);
    }
    
    // Unload shaders
    for (int i = 0; i < manager->shaderCount; i++) {
        UnloadShader(manager->shaders[i].shader);
        free((void*)manager->shaders[i].name);
    }
    
    free(manager->textures);
    free(manager->waves);
    free(manager->music);
    free(manager->fonts);
    free(manager->shaders);
    
    memset(manager, 0, sizeof(ResourceManager));
    TraceLog(LOG_INFO, "Resource Manager unloaded");
}

bool LoadGameTexture(ResourceManager* manager, const char* filename, const char* name) {
    if (!manager || !filename || !name || manager->textureCount >= manager->textureCapacity) return false;
    
    if (!FileExists(filename)) {
        TraceLog(LOG_WARNING, "Texture file not found: %s", filename);
        return false;
    }
    
    Texture2D texture = LoadTexture(filename);
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", filename);
        return false;
    }
    
    manager->textures[manager->textureCount].texture = texture;
    manager->textures[manager->textureCount].name = _strdup(name);
    manager->textureCount++;
    
    return true;
}

bool LoadGameWave(ResourceManager* manager, const char* filename, const char* name) {
    if (!manager || !filename || !name || manager->waveCount >= manager->waveCapacity) return false;
    
    if (!FileExists(filename)) {
        TraceLog(LOG_WARNING, "Wave file not found: %s", filename);
        return false;
    }
    
    Wave wave = LoadWave(filename);
    if (wave.data == NULL) {
        TraceLog(LOG_ERROR, "Failed to load wave: %s", filename);
        return false;
    }
    
    manager->waves[manager->waveCount].wave = wave;
    manager->waves[manager->waveCount].name = _strdup(name);
    manager->waveCount++;
    
    return true;
}

bool LoadGameMusic(ResourceManager* manager, const char* filename, const char* name) {
    if (!manager || !filename || !name || manager->musicCount >= manager->musicCapacity) return false;
    
    if (!FileExists(filename)) {
        TraceLog(LOG_WARNING, "Music file not found: %s", filename);
        return false;
    }
    
    Music music = LoadMusicStream(filename);
    if (music.stream.buffer == NULL) {
        TraceLog(LOG_ERROR, "Failed to load music: %s", filename);
        return false;
    }
    
    manager->music[manager->musicCount].music = music;
    manager->music[manager->musicCount].name = _strdup(name);
    manager->musicCount++;
    
    return true;
}

bool LoadGameFont(ResourceManager* manager, const char* filename, const char* name) {
    if (!manager || !filename || !name || manager->fontCount >= manager->fontCapacity) return false;
    
    if (!FileExists(filename)) {
        TraceLog(LOG_WARNING, "Font file not found: %s", filename);
        return false;
    }
    
    Font font = LoadFont(filename);
    if (font.baseSize == 0) {
        TraceLog(LOG_ERROR, "Failed to load font: %s", filename);
        return false;
    }
    
    manager->fonts[manager->fontCount].font = font;
    manager->fonts[manager->fontCount].name = _strdup(name);
    manager->fontCount++;
    
    return true;
}

bool LoadGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name) {
    if (!manager || !name || manager->shaderCount >= manager->shaderCapacity) return false;
    
    Shader shader = LoadShader(vsFilename, fsFilename);
    if (shader.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load shader: %s, %s", vsFilename, fsFilename);
        return false;
    }
    
    manager->shaders[manager->shaderCount].shader = shader;
    manager->shaders[manager->shaderCount].name = _strdup(name);
    manager->shaderCount++;
    
    return true;
}

void UnloadGameTexture(ResourceManager* manager, const char* name) {
    if (!manager || !name) return;
    
    for (int i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].name, name) == 0) {
            UnloadTexture(manager->textures[i].texture);
            free((void*)manager->textures[i].name);
            
            // Move remaining textures up
            for (int j = i; j < manager->textureCount - 1; j++) {
                manager->textures[j] = manager->textures[j + 1];
            }
            manager->textureCount--;
            break;
        }
    }
}

void UnloadGameWave(ResourceManager* manager, const char* name) {
    if (!manager || !name) return;
    
    for (int i = 0; i < manager->waveCount; i++) {
        if (strcmp(manager->waves[i].name, name) == 0) {
            UnloadWave(manager->waves[i].wave);
            free((void*)manager->waves[i].name);
            
            // Move remaining waves up
            for (int j = i; j < manager->waveCount - 1; j++) {
                manager->waves[j] = manager->waves[j + 1];
            }
            manager->waveCount--;
            break;
        }
    }
}

void UnloadGameMusic(ResourceManager* manager, const char* name) {
    if (!manager || !name) return;
    
    for (int i = 0; i < manager->musicCount; i++) {
        if (strcmp(manager->music[i].name, name) == 0) {
            UnloadMusicStream(manager->music[i].music);
            free((void*)manager->music[i].name);
            
            // Move remaining music up
            for (int j = i; j < manager->musicCount - 1; j++) {
                manager->music[j] = manager->music[j + 1];
            }
            manager->musicCount--;
            break;
        }
    }
}

void UnloadGameFont(ResourceManager* manager, const char* name) {
    if (!manager || !name) return;
    
    for (int i = 0; i < manager->fontCount; i++) {
        if (strcmp(manager->fonts[i].name, name) == 0) {
            UnloadFont(manager->fonts[i].font);
            free((void*)manager->fonts[i].name);
            
            // Move remaining fonts up
            for (int j = i; j < manager->fontCount - 1; j++) {
                manager->fonts[j] = manager->fonts[j + 1];
            }
            manager->fontCount--;
            break;
        }
    }
}

void UnloadGameShader(ResourceManager* manager, const char* name) {
    if (!manager || !name) return;
    
    for (int i = 0; i < manager->shaderCount; i++) {
        if (strcmp(manager->shaders[i].name, name) == 0) {
            UnloadShader(manager->shaders[i].shader);
            free((void*)manager->shaders[i].name);
            
            // Move remaining shaders up
            for (int j = i; j < manager->shaderCount - 1; j++) {
                manager->shaders[j] = manager->shaders[j + 1];
            }
            manager->shaderCount--;
            break;
        }
    }
}

const Texture2D* GetGameTexture(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (size_t i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].name, name) == 0) {
            return &manager->textures[i].texture;
        }
    }
    return NULL;
}

const Wave* GetGameWave(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (size_t i = 0; i < manager->waveCount; i++) {
        if (strcmp(manager->waves[i].name, name) == 0) {
            return &manager->waves[i].wave;
        }
    }
    return NULL;
}

const Music* GetGameMusic(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (size_t i = 0; i < manager->musicCount; i++) {
        if (strcmp(manager->music[i].name, name) == 0) {
            return &manager->music[i].music;
        }
    }
    return NULL;
}

const Font* GetGameFont(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (size_t i = 0; i < manager->fontCount; i++) {
        if (strcmp(manager->fonts[i].name, name) == 0) {
            return &manager->fonts[i].font;
        }
    }
    return NULL;
}

const Shader* GetGameShader(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (strcmp(manager->shaders[i].name, name) == 0) {
            return &manager->shaders[i].shader;
        }
    }
    return NULL;
}

bool AddGameTexture(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameTexture(manager, filename, name);
}

bool AddGameWave(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameWave(manager, filename, name);
}

bool AddGameMusic(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameMusic(manager, filename, name);
}

bool AddGameFont(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameFont(manager, filename, name);
}

bool AddGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name) {
    return LoadGameShader(manager, vsFilename, fsFilename, name);
}

void ReloadAllResources(ResourceManager* manager) {
    if (!manager) return;
    
    // Reload textures
    for (int i = 0; i < manager->textureCount; i++) {
        UnloadTexture(manager->textures[i].texture);
        manager->textures[i].texture = LoadTexture(manager->textures[i].filename);
    }
    
    // Reload waves
    for (int i = 0; i < manager->waveCount; i++) {
        UnloadWave(manager->waves[i].wave);
        manager->waves[i].wave = LoadWave(manager->waves[i].filename);
    }
    
    // Reload music
    for (int i = 0; i < manager->musicCount; i++) {
        UnloadMusicStream(manager->music[i].music);
        manager->music[i].music = LoadMusicStream(manager->music[i].filename);
    }
    
    // Reload fonts
    for (int i = 0; i < manager->fontCount; i++) {
        UnloadFont(manager->fonts[i].font);
        manager->fonts[i].font = LoadFont(manager->fonts[i].filename);
    }
    
    // Reload shaders
    for (int i = 0; i < manager->shaderCount; i++) {
        UnloadShader(manager->shaders[i].shader);
        manager->shaders[i].shader = LoadShader(manager->shaders[i].vsFilename, manager->shaders[i].fsFilename);
    }
    
    TraceLog(LOG_INFO, "All resources reloaded successfully");
}

bool IsResourceLoaded(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return false;
    
    // Check all resource types
    for (size_t i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].name, name) == 0) return true;
    }
    
    for (size_t i = 0; i < manager->waveCount; i++) {
        if (strcmp(manager->waves[i].name, name) == 0) return true;
    }
    
    for (size_t i = 0; i < manager->musicCount; i++) {
        if (strcmp(manager->music[i].name, name) == 0) return true;
    }
    
    for (size_t i = 0; i < manager->fontCount; i++) {
        if (strcmp(manager->fonts[i].name, name) == 0) return true;
    }
    
    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (strcmp(manager->shaders[i].name, name) == 0) return true;
    }
    
    return false;
}

size_t GetResourceCount(const ResourceManager* manager) {
    if (!manager) return 0;
    return manager->textureCount + manager->waveCount + manager->musicCount + 
           manager->fontCount + manager->shaderCount;
}

const Sound* GetGameSound(const ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    const Wave* wave = GetGameWave(manager, name);
    if (!wave) return NULL;
    
    // Convert Wave to Sound if needed
    static Sound cachedSound;
    cachedSound = LoadSoundFromWave(*wave);
    return &cachedSound;
} 