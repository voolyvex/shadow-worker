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

// Global resource manager instance
static ResourceManager g_resourceManager = {0};
static bool g_initialized = false;

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
    if (!g_initialized) {
        memset(&g_resourceManager, 0, sizeof(ResourceManager));
        g_resourceManager.initialized = true;
        g_initialized = true;
    }
    return &g_resourceManager;
}

ResourceManager* CreateResourceManager(void) {
    ResourceManager* manager = (ResourceManager*)calloc(1, sizeof(ResourceManager));
    if (!manager) return NULL;
    
    // Initialize counts
    manager->textureCount = 0;
    manager->waveCount = 0;
    manager->musicCount = 0;
    manager->fontCount = 0;
    manager->shaderCount = 0;
    manager->initialized = true;
    
    return manager;
}

void DestroyResourceManager(ResourceManager* manager) {
    if (!manager) return;
    UnloadAllResources(manager);
    free(manager);
}

void UnloadResourceManager(void) {
    UnloadAllResources(&g_resourceManager);
}

void UnloadAllResources(ResourceManager* manager) {
    if (!manager) {
        TraceLog(LOG_ERROR, "Invalid resource manager");
        return;
    }
    
    // Unload textures
    for (int i = 0; i < manager->textureCount; i++) {
        if (manager->textures[i].name) {
            UnloadGameTexture(manager->textures[i].name);
            i--; // Adjust index since UnloadGameTexture shifts elements
        }
    }
    
    // Unload waves
    for (int i = 0; i < manager->waveCount; i++) {
        UnloadWave(manager->waves[i].wave);
        free((void*)manager->waves[i].name);
        free((void*)manager->waves[i].filename);
    }
    
    // Unload music
    for (int i = 0; i < manager->musicCount; i++) {
        UnloadMusicStream(manager->music[i].music);
        free((void*)manager->music[i].name);
        free((void*)manager->music[i].filename);
    }
    
    // Unload fonts
    for (int i = 0; i < manager->fontCount; i++) {
        UnloadFont(manager->fonts[i].font);
        free((void*)manager->fonts[i].name);
        free((void*)manager->fonts[i].filename);
    }
    
    // Unload shaders
    for (int i = 0; i < manager->shaderCount; i++) {
        UnloadShader(manager->shaders[i].shader);
        free((void*)manager->shaders[i].name);
        if (manager->shaders[i].vsFilename) free((void*)manager->shaders[i].vsFilename);
        if (manager->shaders[i].fsFilename) free((void*)manager->shaders[i].fsFilename);
    }
    
    // Free the arrays
    free(manager->textures);
    free(manager->waves);
    free(manager->music);
    free(manager->fonts);
    free(manager->shaders);
    
    // Clear the structure
    memset(manager, 0, sizeof(ResourceManager));
    TraceLog(LOG_INFO, "Resource Manager unloaded");
}

bool VerifyTextureResources(const ResourceManager* manager) {
    if (!manager) {
        TraceLog(LOG_ERROR, "Invalid resource manager");
        return false;
    }

    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Window/OpenGL context not initialized");
        return false;
    }

    bool allValid = true;

    // Verify all textures
    for (size_t i = 0; i < manager->textureCount; i++) {
        const TextureResource* res = &manager->textures[i];
        
        // Check if texture exists
        if (!res->name) {
            TraceLog(LOG_ERROR, "Texture at index %zu has no name", i);
            allValid = false;
            continue;
        }

        // Verify texture ID
        if (res->texture.id == 0) {
            TraceLog(LOG_ERROR, "Invalid texture ID for resource '%s'", res->name);
            allValid = false;
            continue;
        }

        // Verify texture dimensions
        if (res->texture.width <= 0 || res->texture.height <= 0) {
            TraceLog(LOG_ERROR, "Invalid dimensions for texture '%s' (%dx%d)", 
                    res->name, res->texture.width, res->texture.height);
            allValid = false;
            continue;
        }

        // Verify texture format
        if (res->texture.format <= 0 || res->texture.format >= PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
            TraceLog(LOG_ERROR, "Invalid format for texture '%s' (format: %d)", 
                    res->name, res->texture.format);
            allValid = false;
            continue;
        }

        // Verify mipmaps
        if (res->texture.mipmaps <= 0) {
            TraceLog(LOG_ERROR, "Invalid mipmaps for texture '%s'", res->name);
            allValid = false;
            continue;
        }

        TraceLog(LOG_DEBUG, "Verified texture '%s' (%dx%d, format: %d, mipmaps: %d)", 
                res->name, res->texture.width, res->texture.height, 
                res->texture.format, res->texture.mipmaps);
    }

    if (allValid) {
        TraceLog(LOG_INFO, "All texture resources verified successfully");
    } else {
        TraceLog(LOG_WARNING, "Some texture resources failed verification");
    }

    return allValid;
}

bool LoadGameTexture(const char* filename, const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !filename || !name) {
        LOG_ERROR(LOG_RESOURCE, "Invalid parameters for LoadGameTexture");
        return false;
    }

    if (manager->textureCount >= MAX_TEXTURES) {
        LOG_ERROR(LOG_RESOURCE, "Maximum texture count reached");
        return false;
    }

    // Check if texture already exists
    for (size_t i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, name)) {
            LOG_WARNING(LOG_RESOURCE, "Texture with name %s already exists", name);
            return false;
        }
    }

    // Load the texture
    Texture2D texture = LoadTexture(filename);
    if (texture.id == 0) {
        LOG_WARNING(LOG_RESOURCE, "Failed to load texture from file: %s", filename);
        return false;
    }

    // Store the texture
    TextureResource* resource = &manager->textures[manager->textureCount];
    resource->name = strdup(name);
    resource->filename = strdup(filename);
    resource->texture = texture;
    manager->textureCount++;

    LOG_INFO(LOG_RESOURCE, "Loaded texture: %s", name);
    return true;
}

void UnloadGameTexture(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return;

    for (size_t i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, name)) {
            UnloadTexture(manager->textures[i].texture);
            free((void*)manager->textures[i].name);
            free((void*)manager->textures[i].filename);

            // Shift remaining textures
            if (i < manager->textureCount - 1) {
                memmove(&manager->textures[i], &manager->textures[i + 1], 
                       (manager->textureCount - i - 1) * sizeof(TextureResource));
            }
            manager->textureCount--;
            return;
        }
    }
}

Texture2D* GetGameTexture(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return NULL;

    for (size_t i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, name)) {
            return &manager->textures[i].texture;
        }
    }
    return NULL;
}

bool LoadGameWave(const char* filename, const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !filename || !name) {
        LOG_ERROR(LOG_RESOURCE, "Invalid parameters for LoadGameWave");
        return false;
    }

    if (manager->waveCount >= MAX_SOUNDS) {
        LOG_ERROR(LOG_RESOURCE, "Maximum wave count reached");
        return false;
    }

    // Check if wave already exists
    for (size_t i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, name)) {
            LOG_WARNING(LOG_RESOURCE, "Wave with name %s already exists", name);
            return false;
        }
    }

    // Load the wave
    Wave wave = LoadWave(filename);
    if (wave.frameCount == 0) {
        LOG_WARNING(LOG_RESOURCE, "Failed to load wave from file: %s", filename);
        return false;
    }

    // Store the wave
    WaveResource* resource = &manager->waves[manager->waveCount];
    resource->name = strdup(name);
    resource->filename = strdup(filename);
    resource->wave = wave;
    manager->waveCount++;

    LOG_INFO(LOG_RESOURCE, "Loaded wave: %s", name);
    return true;
}

void UnloadGameWave(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return;

    for (size_t i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, name)) {
            UnloadWave(manager->waves[i].wave);
            free((void*)manager->waves[i].name);
            free((void*)manager->waves[i].filename);

            // Shift remaining waves
            if (i < manager->waveCount - 1) {
                memmove(&manager->waves[i], &manager->waves[i + 1],
                       (manager->waveCount - i - 1) * sizeof(WaveResource));
            }
            manager->waveCount--;
            return;
        }
    }
}

Wave* GetGameWave(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return NULL;

    for (size_t i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, name)) {
            return &manager->waves[i].wave;
        }
    }
    return NULL;
}

bool LoadGameMusic(const char* filename, const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !filename || !name) {
        LOG_ERROR(LOG_RESOURCE, "Invalid parameters for LoadGameMusic");
        return false;
    }

    if (manager->musicCount >= MAX_MUSIC) {
        LOG_ERROR(LOG_RESOURCE, "Maximum music count reached");
        return false;
    }

    // Check if music already exists
    for (size_t i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, name)) {
            LOG_WARNING(LOG_RESOURCE, "Music with name %s already exists", name);
            return false;
        }
    }

    // Load the music
    Music music = LoadMusicStream(filename);
    if (music.frameCount == 0) {
        LOG_WARNING(LOG_RESOURCE, "Failed to load music from file: %s", filename);
        return false;
    }

    // Store the music
    MusicResource* resource = &manager->music[manager->musicCount];
    resource->name = strdup(name);
    resource->filename = strdup(filename);
    resource->music = music;
    manager->musicCount++;

    LOG_INFO(LOG_RESOURCE, "Loaded music: %s", name);
    return true;
}

void UnloadGameMusic(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return;

    for (size_t i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, name)) {
            UnloadMusicStream(manager->music[i].music);
            free((void*)manager->music[i].name);
            free((void*)manager->music[i].filename);

            // Shift remaining music
            if (i < manager->musicCount - 1) {
                memmove(&manager->music[i], &manager->music[i + 1],
                       (manager->musicCount - i - 1) * sizeof(MusicResource));
            }
            manager->musicCount--;
            return;
        }
    }
}

Music* GetGameMusic(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return NULL;

    for (size_t i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, name)) {
            return &manager->music[i].music;
        }
    }
    return NULL;
}

bool LoadGameFont(const char* filename, const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !filename || !name) {
        LOG_ERROR(LOG_RESOURCE, "Invalid parameters for LoadGameFont");
        return false;
    }

    if (manager->fontCount >= MAX_FONTS) {
        LOG_ERROR(LOG_RESOURCE, "Maximum font count reached");
        return false;
    }

    // Check if font already exists
    for (size_t i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, name)) {
            LOG_WARNING(LOG_RESOURCE, "Font with name %s already exists", name);
            return false;
        }
    }

    // Load the font
    Font font = LoadFont(filename);
    if (font.baseSize == 0) {
        LOG_WARNING(LOG_RESOURCE, "Failed to load font from file: %s", filename);
        return false;
    }

    // Store the font
    FontResource* resource = &manager->fonts[manager->fontCount];
    resource->name = strdup(name);
    resource->filename = strdup(filename);
    resource->font = font;
    manager->fontCount++;

    LOG_INFO(LOG_RESOURCE, "Loaded font: %s", name);
    return true;
}

void UnloadGameFont(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return;

    for (size_t i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, name)) {
            UnloadFont(manager->fonts[i].font);
            free((void*)manager->fonts[i].name);
            free((void*)manager->fonts[i].filename);

            // Shift remaining fonts
            if (i < manager->fontCount - 1) {
                memmove(&manager->fonts[i], &manager->fonts[i + 1],
                       (manager->fontCount - i - 1) * sizeof(FontResource));
            }
            manager->fontCount--;
            return;
        }
    }
}

Font* GetGameFont(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return NULL;

    for (size_t i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, name)) {
            return &manager->fonts[i].font;
        }
    }
    return NULL;
}

bool LoadGameShader(const char* vsFilename, const char* fsFilename, const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name || (!vsFilename && !fsFilename)) {
        LOG_ERROR(LOG_RESOURCE, "Invalid parameters for LoadGameShader");
        return false;
    }

    if (manager->shaderCount >= MAX_SHADERS) {
        LOG_ERROR(LOG_RESOURCE, "Maximum shader count reached");
        return false;
    }

    // Check if shader already exists
    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, name)) {
            LOG_WARNING(LOG_RESOURCE, "Shader with name %s already exists", name);
            return false;
        }
    }

    // Load the shader
    Shader shader = LoadShader(vsFilename, fsFilename);
    if (shader.id == 0) {
        LOG_WARNING(LOG_RESOURCE, "Failed to load shader from files: vs=%s, fs=%s", 
                   vsFilename ? vsFilename : "NULL", 
                   fsFilename ? fsFilename : "NULL");
        return false;
    }

    // Store the shader
    ShaderResource* resource = &manager->shaders[manager->shaderCount];
    resource->name = strdup(name);
    resource->vsFilename = vsFilename ? strdup(vsFilename) : NULL;
    resource->fsFilename = fsFilename ? strdup(fsFilename) : NULL;
    resource->shader = shader;
    manager->shaderCount++;

    LOG_INFO(LOG_RESOURCE, "Loaded shader: %s", name);
    return true;
}

void UnloadGameShader(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return;

    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, name)) {
            UnloadShader(manager->shaders[i].shader);
            free((void*)manager->shaders[i].name);
            if (manager->shaders[i].vsFilename) free((void*)manager->shaders[i].vsFilename);
            if (manager->shaders[i].fsFilename) free((void*)manager->shaders[i].fsFilename);

            // Shift remaining shaders
            if (i < manager->shaderCount - 1) {
                memmove(&manager->shaders[i], &manager->shaders[i + 1],
                       (manager->shaderCount - i - 1) * sizeof(ShaderResource));
            }
            manager->shaderCount--;
            return;
        }
    }
}

Shader* GetGameShader(const char* name) {
    ResourceManager* manager = GetResourceManager();
    if (!manager || !name) return NULL;

    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, name)) {
            return &manager->shaders[i].shader;
        }
    }
    return NULL;
}

bool AddGameTexture(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameTexture(filename, name);
}

bool AddGameWave(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameWave(filename, name);
}

bool AddGameMusic(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameMusic(filename, name);
}

bool AddGameFont(ResourceManager* manager, const char* filename, const char* name) {
    return LoadGameFont(filename, name);
}

bool AddGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name) {
    return LoadGameShader(vsFilename, fsFilename, name);
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

bool IsResourceLoaded(const ResourceManager* manager, const char* key) {
    if (!manager || !key) return false;
    
    // Check all resource types
    for (size_t i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, key)) return true;
    }
    
    for (size_t i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, key)) return true;
    }
    
    for (size_t i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, key)) return true;
    }
    
    for (size_t i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, key)) return true;
    }
    
    for (size_t i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, key)) return true;
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
    
    const Wave* wave = GetGameWave(name);
    if (!wave) return NULL;
    
    // Convert Wave to Sound if needed
    static Sound cachedSound;
    // Create a non-const copy of the wave for LoadSoundFromWave
    Wave tempWave = {
        .data = wave->data,
        .frameCount = wave->frameCount,
        .sampleRate = wave->sampleRate,
        .sampleSize = wave->sampleSize,
        .channels = wave->channels
    };
    cachedSound = LoadSoundFromWave(tempWave);
    return &cachedSound;
}

void* GetResource(ResourceManager* manager, const char* key) {
    if (!manager || !key) return NULL;
    
    // Try to find in textures
    for (int i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, key)) {
            return &manager->textures[i].texture;
        }
    }
    
    // Try to find in waves
    for (int i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, key)) {
            return &manager->waves[i].wave;
        }
    }
    
    // Try to find in music
    for (int i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, key)) {
            return &manager->music[i].music;
        }
    }
    
    // Try to find in fonts
    for (int i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, key)) {
            return &manager->fonts[i].font;
        }
    }
    
    // Try to find in shaders
    for (int i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, key)) {
            return &manager->shaders[i].shader;
        }
    }
    
    return NULL;
}

void UnloadResource(ResourceManager* manager, const char* key) {
    if (!manager || !key) return;
    
    // Try to unload texture
    for (int i = 0; i < manager->textureCount; i++) {
        if (TextIsEqual(manager->textures[i].name, key)) {
            UnloadGameTexture(manager->textures[i].name);
            return;
        }
    }
    
    // Try to unload wave
    for (int i = 0; i < manager->waveCount; i++) {
        if (TextIsEqual(manager->waves[i].name, key)) {
            UnloadGameWave(key);
            return;
        }
    }
    
    // Try to unload music
    for (int i = 0; i < manager->musicCount; i++) {
        if (TextIsEqual(manager->music[i].name, key)) {
            UnloadGameMusic(key);
            return;
        }
    }
    
    // Try to unload font
    for (int i = 0; i < manager->fontCount; i++) {
        if (TextIsEqual(manager->fonts[i].name, key)) {
            UnloadGameFont(key);
            return;
        }
    }
    
    // Try to unload shader
    for (int i = 0; i < manager->shaderCount; i++) {
        if (TextIsEqual(manager->shaders[i].name, key)) {
            UnloadGameShader(key);
            return;
        }
    }
}

bool LoadResource(ResourceManager* manager, const char* name, const char* filename) {
    if (!manager || !name || !filename) return false;
    
    // Try to determine resource type from file extension
    const char* ext = strrchr(filename, '.');
    if (!ext) return false;
    ext++; // Skip the dot
    
    // Load based on file extension
    if (_stricmp(ext, "png") == 0 || _stricmp(ext, "jpg") == 0 || 
        _stricmp(ext, "bmp") == 0 || _stricmp(ext, "tga") == 0) {
        return LoadGameTexture(filename, name);
    }
    else if (_stricmp(ext, "wav") == 0) {
        return LoadGameWave(filename, name);
    }
    else if (_stricmp(ext, "mp3") == 0 || _stricmp(ext, "ogg") == 0) {
        return LoadGameMusic(filename, name);
    }
    else if (_stricmp(ext, "ttf") == 0 || _stricmp(ext, "otf") == 0) {
        return LoadGameFont(filename, name);
    }
    else if (_stricmp(ext, "fs") == 0 || _stricmp(ext, "vs") == 0) {
        // For shaders, we need both vertex and fragment shaders
        // This is a simplified version - you might want to enhance this
        return LoadGameShader(filename, filename, name);
    }
    
    return false;
} 