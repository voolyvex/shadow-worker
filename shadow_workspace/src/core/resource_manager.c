#include "../../include/resource_manager.h"
#include "../../include/sound_manager.h"
#include "../../include/logger.h"
#include "../../include/resource_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <raylib.h>

// Audio format definitions
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2

typedef struct ResourceManager {
    TextureResource* textures;
    WaveResource* waves;
    MusicResource* music;
    FontResource* fonts;
    ShaderResource* shaders;
    int textureCount;
    int waveCount;
    int musicCount;
    int fontCount;
    int shaderCount;
    int textureCapacity;
    int waveCapacity;
    int musicCapacity;
    int fontCapacity;
    int shaderCapacity;
    bool initialized;
} ResourceManager;

static ResourceManager g_resourceManager = {0};

// Resource validation configuration
typedef struct {
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

bool InitResourceManager(void) {
    if (g_resourceManager.initialized) return true;
    
    g_resourceManager.textures = (TextureResource*)calloc(MAX_TEXTURES, sizeof(TextureResource));
    g_resourceManager.waves = (WaveResource*)calloc(MAX_SOUNDS, sizeof(WaveResource));
    g_resourceManager.music = (MusicResource*)calloc(MAX_MUSIC, sizeof(MusicResource));
    g_resourceManager.fonts = (FontResource*)calloc(MAX_FONTS, sizeof(FontResource));
    g_resourceManager.shaders = (ShaderResource*)calloc(MAX_SHADERS, sizeof(ShaderResource));
    
    if (!g_resourceManager.textures || !g_resourceManager.waves || !g_resourceManager.music ||
        !g_resourceManager.fonts || !g_resourceManager.shaders) {
        UnloadResourceManager();
        return false;
    }

    g_resourceManager.textureCount = 0;
    g_resourceManager.waveCount = 0;
    g_resourceManager.musicCount = 0;
    g_resourceManager.fontCount = 0;
    g_resourceManager.shaderCount = 0;
    
    g_resourceManager.textureCapacity = MAX_TEXTURES;
    g_resourceManager.waveCapacity = MAX_SOUNDS;
    g_resourceManager.musicCapacity = MAX_MUSIC;
    g_resourceManager.fontCapacity = MAX_FONTS;
    g_resourceManager.shaderCapacity = MAX_SHADERS;
    
    g_resourceManager.initialized = true;
    
    return true;
}

void UnloadResourceManager(void) {
    if (!g_resourceManager.initialized) return;

    // Unload textures
    for (int i = 0; i < g_resourceManager.textureCount; i++) {
        UnloadTexture(g_resourceManager.textures[i].texture);
        free((void*)g_resourceManager.textures[i].name);
    }
    
    // Unload waves
    for (int i = 0; i < g_resourceManager.waveCount; i++) {
        UnloadWave(g_resourceManager.waves[i].wave);
        free((void*)g_resourceManager.waves[i].name);
    }
    
    // Unload music
    for (int i = 0; i < g_resourceManager.musicCount; i++) {
        UnloadMusicStream(g_resourceManager.music[i].music);
        free((void*)g_resourceManager.music[i].name);
    }
    
    // Unload fonts
    for (int i = 0; i < g_resourceManager.fontCount; i++) {
        UnloadFont(g_resourceManager.fonts[i].font);
        free((void*)g_resourceManager.fonts[i].name);
    }
    
    // Unload shaders
    for (int i = 0; i < g_resourceManager.shaderCount; i++) {
        UnloadShader(g_resourceManager.shaders[i].shader);
        free((void*)g_resourceManager.shaders[i].name);
    }
    
    free(g_resourceManager.textures);
    free(g_resourceManager.waves);
    free(g_resourceManager.music);
    free(g_resourceManager.fonts);
    free(g_resourceManager.shaders);
    
    memset(&g_resourceManager, 0, sizeof(ResourceManager));
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

Texture2D* GetGameTexture(ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].name, name) == 0) {
            return &manager->textures[i].texture;
        }
    }
    return NULL;
}

Wave* GetGameWave(ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->waveCount; i++) {
        if (strcmp(manager->waves[i].name, name) == 0) {
            return &manager->waves[i].wave;
        }
    }
    return NULL;
}

Music* GetGameMusic(ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->musicCount; i++) {
        if (strcmp(manager->music[i].name, name) == 0) {
            return &manager->music[i].music;
        }
    }
    return NULL;
}

Font* GetGameFont(ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->fontCount; i++) {
        if (strcmp(manager->fonts[i].name, name) == 0) {
            return &manager->fonts[i].font;
        }
    }
    return NULL;
}

Shader* GetGameShader(ResourceManager* manager, const char* name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->shaderCount; i++) {
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

void UnloadAllResources(ResourceManager* manager) {
    if (!manager) return;
    
    // Unload all textures
    for (int i = 0; i < manager->textureCount; i++) {
        UnloadTexture(manager->textures[i].texture);
        free((void*)manager->textures[i].name);
    }
    manager->textureCount = 0;
    
    // Unload all waves
    for (int i = 0; i < manager->waveCount; i++) {
        UnloadWave(manager->waves[i].wave);
        free((void*)manager->waves[i].name);
    }
    manager->waveCount = 0;
    
    // Unload all music
    for (int i = 0; i < manager->musicCount; i++) {
        UnloadMusicStream(manager->music[i].music);
        free((void*)manager->music[i].name);
    }
    manager->musicCount = 0;
    
    // Unload all fonts
    for (int i = 0; i < manager->fontCount; i++) {
        UnloadFont(manager->fonts[i].font);
        free((void*)manager->fonts[i].name);
    }
    manager->fontCount = 0;
    
    // Unload all shaders
    for (int i = 0; i < manager->shaderCount; i++) {
        UnloadShader(manager->shaders[i].shader);
        free((void*)manager->shaders[i].name);
    }
    manager->shaderCount = 0;
}

void ReloadAllResources(ResourceManager* manager) {
    // Not implemented yet
    TraceLog(LOG_WARNING, "ReloadAllResources not implemented");
}

bool IsResourceLoaded(ResourceManager* manager, const char* name) {
    if (!manager || !name) return false;
    
    // Check all resource types
    for (int i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].name, name) == 0) return true;
    }
    
    for (int i = 0; i < manager->waveCount; i++) {
        if (strcmp(manager->waves[i].name, name) == 0) return true;
    }
    
    for (int i = 0; i < manager->musicCount; i++) {
        if (strcmp(manager->music[i].name, name) == 0) return true;
    }
    
    for (int i = 0; i < manager->fontCount; i++) {
        if (strcmp(manager->fonts[i].name, name) == 0) return true;
    }
    
    for (int i = 0; i < manager->shaderCount; i++) {
        if (strcmp(manager->shaders[i].name, name) == 0) return true;
    }
    
    return false;
}

int GetResourceCount(ResourceManager* manager) {
    if (!manager) return 0;
    return manager->textureCount + manager->waveCount + manager->musicCount + 
           manager->fontCount + manager->shaderCount;
} 