#include "../../include/resource_manager.h"
#include "../../include/sound_manager.h"
#include "../../include/logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <raylib.h>

// Audio format definitions
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2

static ResourceManager resourceManager = {0};

// Resource validation utilities
static struct {
    const char* supportedImageExtensions[4];
    const char* supportedAudioExtensions[4];
    int maxImageSize;
    int maxAudioSize;
    int supportedImageCount;
    int supportedAudioCount;
} validationConfig = {
    .supportedImageExtensions = {".png", ".jpg", ".bmp", ".gif"},
    .supportedAudioExtensions = {".wav", ".mp3", ".ogg", ".flac"},
    .maxImageSize = 4096,  // Max texture size
    .maxAudioSize = 50 * 1024 * 1024,  // 50MB max audio size
    .supportedImageCount = 4,
    .supportedAudioCount = 4
};

// Forward declarations
static bool ValidateFileExtension(const char* filename, const char** extensions, int extensionCount);
static bool ValidateFileSize(const char* filename, size_t maxSize);
static bool ValidateAudioFormat(const char* filename);
static Wave* ConvertAudioFormat(Wave* wave);
static bool LoadResourceManifest(const char* manifestPath);

static bool ValidateFileExtension(const char* filename, const char** extensions, int extensionCount) {
    const char* ext = strrchr(filename, '.');
    if (!ext) {
        LOG_ERROR(LOG_RESOURCE, "No file extension found for: %s", filename);
        return false;
    }
    
    for (int i = 0; i < extensionCount; i++) {
        if (strcmp(ext, extensions[i]) == 0) {
            return true;
        }
    }
    
    LOG_ERROR(LOG_RESOURCE, "Unsupported file extension %s for file: %s", ext, filename);
    return false;
}

static bool ValidateFileSize(const char* filename, size_t maxSize) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        LOG_ERROR(LOG_RESOURCE, "Cannot open file for size validation: %s", filename);
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    
    if (size > maxSize) {
        LOG_ERROR(LOG_RESOURCE, "File too large: %s (%.2f MB > %.2f MB)", 
                 filename, size / (1024.0 * 1024.0), maxSize / (1024.0 * 1024.0));
        return false;
    }
    
    return true;
}

static Wave* ConvertAudioFormat(Wave* wave) {
    if (wave == NULL) {
        LOG_ERROR(LOG_RESOURCE, "Invalid wave pointer");
        return NULL;
    }

    LOG_DEBUG(LOG_RESOURCE, "Converting audio format - Channels: %d, Sample Rate: %u",
              wave->channels, wave->sampleRate);
        
    Wave* result = wave;
    bool needsConversion = false;

    // Convert sample rate if needed
    if (wave->sampleRate != AUDIO_SAMPLE_RATE) {
        needsConversion = true;
        Wave* temp = (Wave*)RL_MALLOC(sizeof(Wave));
        if (temp == NULL) {
            LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for wave conversion");
            return NULL;
        }
        memcpy(temp, wave, sizeof(Wave));
        temp->data = RL_MALLOC(wave->frameCount * wave->channels * wave->sampleSize / 8);
        if (temp->data == NULL) {
            LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for wave data");
            RL_FREE(temp);
            return NULL;
        }
        memcpy(temp->data, wave->data, wave->frameCount * wave->channels * wave->sampleSize / 8);
        WaveFormat(temp, AUDIO_SAMPLE_RATE, temp->sampleSize, temp->channels);
        LOG_INFO(LOG_RESOURCE, "Successfully resampled audio to %d Hz", AUDIO_SAMPLE_RATE);
        if (result != wave) {
            UnloadWave(*result);
            RL_FREE(result);
        }
        result = temp;
    }
        
    // Convert channels if needed
    if (wave->channels > AUDIO_CHANNELS) {
        needsConversion = true;
        Wave* temp = (Wave*)RL_MALLOC(sizeof(Wave));
        if (temp == NULL) {
            LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for wave conversion");
            if (result != wave) {
                UnloadWave(*result);
                RL_FREE(result);
            }
            return NULL;
        }
        memcpy(temp, result, sizeof(Wave));
        temp->data = RL_MALLOC(result->frameCount * result->channels * result->sampleSize / 8);
        if (temp->data == NULL) {
            LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for wave data");
            RL_FREE(temp);
            if (result != wave) {
                UnloadWave(*result);
                RL_FREE(result);
            }
            return NULL;
        }
        memcpy(temp->data, result->data, result->frameCount * result->channels * result->sampleSize / 8);
        WaveFormat(temp, temp->sampleRate, temp->sampleSize, AUDIO_CHANNELS);
        LOG_INFO(LOG_RESOURCE, "Successfully converted audio to stereo");
        if (result != wave) {
            UnloadWave(*result);
            RL_FREE(result);
        }
        result = temp;
    }
    
    return result;
}

static bool ValidateAudioFormat(const char* filename) {
    Wave wave = LoadWave(filename);
    bool isValid = true;
    bool needsConversion = false;
    
    if (wave.data != NULL) {
        // Log audio format details
        LOG_DEBUG(LOG_RESOURCE, "Audio format validation - File: %s", filename);
        LOG_DEBUG(LOG_RESOURCE, "  Channels: %d, Sample Rate: %d, Frame Count: %d",
                 wave.channels, wave.sampleRate, wave.frameCount);
        
        // Check if channel conversion is needed
        if (wave.channels > AUDIO_CHANNELS) {
            LOG_WARN(LOG_RESOURCE, "Channel count needs conversion: %d (will convert to stereo)", wave.channels);
            needsConversion = true;
        }
        
        // Check if sample rate conversion is needed
        if (wave.sampleRate != AUDIO_SAMPLE_RATE) {
            LOG_WARN(LOG_RESOURCE, "Sample rate needs conversion: %d (will convert to %d)", 
                    wave.sampleRate, AUDIO_SAMPLE_RATE);
            needsConversion = true;
        }
        
        UnloadWave(wave);
    } else {
        LOG_ERROR(LOG_RESOURCE, "Failed to load wave for format validation: %s", filename);
        isValid = false;
    }
    
    return isValid;
}

static bool ValidateResourcePath(const char* type, const char* path) {
    // Check if file exists
    FILE* file = fopen(path, "rb");
    if (!file) {
        LOG_ERROR(LOG_RESOURCE, "Resource file not found: %s", path);
        return false;
    }
    fclose(file);
    
    // Validate based on resource type
    if (strcmp(type, "texture") == 0) {
        if (!ValidateFileExtension(path, validationConfig.supportedImageExtensions, 
                                 validationConfig.supportedImageCount)) {
            return false;
        }
        if (!ValidateFileSize(path, validationConfig.maxImageSize)) {
            return false;
        }
        
        // Additional image validation
        Image img = LoadImage(path);
        bool isValid = true;
        if (img.data) {
            if (img.width > validationConfig.maxImageSize || img.height > validationConfig.maxImageSize) {
                LOG_ERROR(LOG_RESOURCE, "Image dimensions too large: %dx%d (max: %dx%d)",
                         img.width, img.height, validationConfig.maxImageSize, validationConfig.maxImageSize);
                isValid = false;
            }
            UnloadImage(img);
        } else {
            LOG_ERROR(LOG_RESOURCE, "Failed to load image for validation: %s", path);
            isValid = false;
        }
        return isValid;
        
    } else if (strcmp(type, "sound") == 0 || strcmp(type, "music") == 0) {
        if (!ValidateFileExtension(path, validationConfig.supportedAudioExtensions,
                                 validationConfig.supportedAudioCount)) {
            return false;
        }
        if (!ValidateFileSize(path, validationConfig.maxAudioSize)) {
            return false;
        }
        return ValidateAudioFormat(path);
    }
    
    LOG_ERROR(LOG_RESOURCE, "Unknown resource type: %s", type);
    return false;
}

static bool LoadResourceManifest(const char* manifestPath) {
    LOG_INFO(LOG_RESOURCE, "Loading resource manifest: %s", manifestPath);
    Logger_BeginTimer("manifest_load");
    
    FILE* file = fopen(manifestPath, "r");
    if (!file) {
        LOG_ERROR(LOG_RESOURCE, "Failed to open manifest file: %s", manifestPath);
        return false;
    }
    
    int resourceCount = 0;
    int validResources = 0;
    char line[256];
    
    LOG_INFO(LOG_RESOURCE, "Starting manifest validation...");
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        char type[32], name[64], path[256];
        if (sscanf(line, "%s %s %s", type, name, path) != 3) {
            LOG_WARN(LOG_RESOURCE, "Invalid manifest line: %s", line);
            continue;
        }
        
        resourceCount++;
        LOG_DEBUG(LOG_RESOURCE, "Validating resource - Type: %s, Name: %s, Path: %s", 
                 type, name, path);
        
        // Validate resource
        if (!ValidateResourcePath(type, path)) {
            LOG_ERROR(LOG_RESOURCE, "Resource validation failed for %s: %s", name, path);
            continue;
        }
        
        // Load resource based on type
        bool loadSuccess = false;
        if (strcmp(type, "texture") == 0) {
            loadSuccess = (LoadGameTexture(name) != NULL);
        } else if (strcmp(type, "sound") == 0) {
            loadSuccess = (LoadGameSound(path) != NULL);
        } else if (strcmp(type, "music") == 0) {
            loadSuccess = (LoadGameMusic(path) != NULL);
        }
        
        if (loadSuccess) {
            validResources++;
        }
    }
    
    fclose(file);
    Logger_EndTimer("manifest_load");
    
    LOG_INFO(LOG_RESOURCE, "Manifest processing complete - Total: %d, Valid: %d, Failed: %d",
             resourceCount, validResources, resourceCount - validResources);
    
    return validResources > 0;
}

bool InitResourceManager(void) {
    LOG_INFO(LOG_RESOURCE, "Initializing Resource Manager");
    Logger_BeginTimer("resource_manager_init");
    
    // Allocate memory for textures
    resourceManager.textures = (Texture2D*)malloc(MAX_TEXTURES * sizeof(Texture2D));
    resourceManager.textureNames = (char**)malloc(MAX_TEXTURES * sizeof(char*));
    if (!resourceManager.textures || !resourceManager.textureNames) {
        LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for textures");
        return false;
    }
    LOG_DEBUG(LOG_RESOURCE, "Allocated memory for %d textures", MAX_TEXTURES);
    resourceManager.textureCount = 0;
    
    // Allocate memory for waves
    resourceManager.waves = (Wave*)malloc(MAX_SOUNDS * sizeof(Wave));
    resourceManager.waveNames = (char**)malloc(MAX_SOUNDS * sizeof(char*));
    if (!resourceManager.waves || !resourceManager.waveNames) {
        LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for waves");
        return false;
    }
    LOG_DEBUG(LOG_RESOURCE, "Allocated memory for %d waves", MAX_SOUNDS);
    resourceManager.waveCount = 0;
    
    // Allocate memory for music
    resourceManager.music = (Music*)malloc(MAX_MUSIC * sizeof(Music));
    resourceManager.musicNames = (char**)malloc(MAX_MUSIC * sizeof(char*));
    if (!resourceManager.music || !resourceManager.musicNames) {
        LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for music");
        return false;
    }
    LOG_DEBUG(LOG_RESOURCE, "Allocated memory for %d music tracks", MAX_MUSIC);
    resourceManager.musicCount = 0;
    
    // Load resources from manifest
    if (!LoadResourceManifest("resources/manifest.txt")) {
        LOG_WARN(LOG_RESOURCE, "Failed to load resource manifest, continuing with manual loading");
    }
    
    Logger_EndTimer("resource_manager_init");
    Logger_LogMemoryUsage();
    LOG_INFO(LOG_RESOURCE, "Resource Manager initialized successfully");
    return true;
}

void UnloadResourceManager(void) {
    LOG_INFO(LOG_RESOURCE, "Unloading Resource Manager");
    Logger_BeginTimer("resource_manager_unload");
    
    // Log current resource counts
    LOG_DEBUG(LOG_RESOURCE, "Current resource counts - Textures: %d, Waves: %d, Music: %d",
             resourceManager.textureCount, resourceManager.waveCount, resourceManager.musicCount);
    
    // Unload textures
    LOG_DEBUG(LOG_RESOURCE, "Unloading %d textures", resourceManager.textureCount);
    for (int i = 0; i < resourceManager.textureCount; i++) {
        LOG_TRACE(LOG_RESOURCE, "Unloading texture: %s", resourceManager.textureNames[i]);
        UnloadTexture(resourceManager.textures[i]);
        free(resourceManager.textureNames[i]);
    }
    free(resourceManager.textures);
    free(resourceManager.textureNames);
    
    // Unload waves
    LOG_DEBUG(LOG_RESOURCE, "Unloading %d waves", resourceManager.waveCount);
    for (int i = 0; i < resourceManager.waveCount; i++) {
        LOG_TRACE(LOG_RESOURCE, "Unloading wave: %s", resourceManager.waveNames[i]);
        UnloadWave(resourceManager.waves[i]);
        free(resourceManager.waveNames[i]);
    }
    free(resourceManager.waves);
    free(resourceManager.waveNames);
    
    // Unload music
    LOG_DEBUG(LOG_RESOURCE, "Unloading %d music tracks", resourceManager.musicCount);
    for (int i = 0; i < resourceManager.musicCount; i++) {
        LOG_TRACE(LOG_RESOURCE, "Unloading music: %s", resourceManager.musicNames[i]);
        UnloadMusicStream(resourceManager.music[i]);
        free(resourceManager.musicNames[i]);
    }
    free(resourceManager.music);
    free(resourceManager.musicNames);
    
    Logger_EndTimer("resource_manager_unload");
    LOG_INFO(LOG_RESOURCE, "Resource Manager unloaded successfully");
    Logger_LogMemoryUsage();
}

ResourceManager* GetResourceManager(void) {
    return &resourceManager;
}

Texture2D* LoadGameTexture(const char* name) {
    Logger_BeginTimer("texture_load");
    
    if (resourceManager.textureCount >= MAX_TEXTURES) {
        LOG_ERROR(LOG_RESOURCE, "Maximum texture count reached (%d)", MAX_TEXTURES);
        Logger_EndTimer("texture_load");
        return NULL;
    }
    
    // Check if texture already exists
    for (int i = 0; i < resourceManager.textureCount; i++) {
        if (strcmp(resourceManager.textureNames[i], name) == 0) {
            LOG_DEBUG(LOG_RESOURCE, "Texture already loaded: %s", name);
            Logger_EndTimer("texture_load");
            return &resourceManager.textures[i];
        }
    }
    
    // Load new texture
    LOG_DEBUG(LOG_RESOURCE, "Loading texture: %s", name);
    Texture2D texture = LoadTexture(name);
    if (texture.id == 0) {
        LOG_ERROR(LOG_RESOURCE, "Failed to load texture: %s", name);
        Logger_EndTimer("texture_load");
        return NULL;
    }
    
    // Store texture
    resourceManager.textures[resourceManager.textureCount] = texture;
    resourceManager.textureNames[resourceManager.textureCount] = _strdup(name);
    LOG_INFO(LOG_RESOURCE, "Texture loaded successfully: %s (%dx%d)", 
             name, texture.width, texture.height);
    
    Logger_EndTimer("texture_load");
    return &resourceManager.textures[resourceManager.textureCount++];
}

Wave* LoadGameSound(const char* filename) {
    if (!filename) {
        LOG_ERROR(LOG_RESOURCE, "Invalid filename for sound loading");
        return NULL;
    }

    // Load the wave file
    Wave wave = LoadWave(filename);
    if (wave.data == NULL) {
        LOG_ERROR(LOG_RESOURCE, "Failed to load wave: %s", filename);
        return NULL;
    }

    // Check if we need to convert the format
    bool needsConversion = false;
    if (wave.sampleRate != AUDIO_SAMPLE_RATE || wave.channels > AUDIO_CHANNELS) {
        needsConversion = true;
    }

    Wave* result = (Wave*)malloc(sizeof(Wave));
    if (!result) {
        LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for wave");
        UnloadWave(wave);
        return NULL;
    }
    *result = wave;

    if (needsConversion) {
        Wave* convertedWave = ConvertAudioFormat(&wave);
        if (convertedWave != NULL) {
            // Save the converted wave
            char convertedPath[256];
            snprintf(convertedPath, sizeof(convertedPath), "%s.converted.wav", filename);
            if (ExportWave(*convertedWave, convertedPath)) {
                LOG_INFO(LOG_RESOURCE, "Saved converted wave to %s", convertedPath);
                UnloadWave(wave);
                wave = *convertedWave;
                RL_FREE(convertedWave);
            } else {
                LOG_ERROR(LOG_RESOURCE, "Failed to save converted wave");
                UnloadWave(*convertedWave);
                RL_FREE(convertedWave);
                UnloadWave(wave);
                free(result);
                return NULL;
            }
        } else {
            LOG_ERROR(LOG_RESOURCE, "Failed to convert wave format");
            UnloadWave(wave);
            free(result);
            return NULL;
        }
    }

    // Store in resource manager
    if (resourceManager.waveCount >= MAX_SOUNDS) {
        LOG_ERROR(LOG_RESOURCE, "Maximum sound count reached");
        UnloadWave(*result);
        free(result);
        return NULL;
    }

    resourceManager.waves[resourceManager.waveCount] = *result;
    resourceManager.waveNames[resourceManager.waveCount] = _strdup(filename);
    Wave* storedWave = &resourceManager.waves[resourceManager.waveCount];
    resourceManager.waveCount++;

    free(result);
    return storedWave;
}

Music* LoadGameMusic(const char* filename) {
    if (!filename) {
        LOG_ERROR(LOG_RESOURCE, "Invalid filename for music loading");
        return NULL;
    }

    Music music = LoadMusicStream(filename);
    if (music.stream.buffer == NULL) {
        LOG_ERROR(LOG_RESOURCE, "Failed to load music: %s", filename);
        return NULL;
    }

    // Store the music in the resource manager
    if (resourceManager.musicCount >= MAX_MUSIC) {
        LOG_ERROR(LOG_RESOURCE, "Maximum music count reached");
        UnloadMusicStream(music);
        return NULL;
    }

    // Set default properties
    SetMusicVolume(music, 1.0f);
    SetMusicPitch(music, 1.0f);
    SetMusicPan(music, 0.5f);

    resourceManager.music[resourceManager.musicCount] = music;
    resourceManager.musicNames[resourceManager.musicCount] = _strdup(filename);
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

Wave* GetWave(const char* name) {
    for (int i = 0; i < resourceManager.waveCount; i++) {
        if (strcmp(resourceManager.waveNames[i], name) == 0) {
            return &resourceManager.waves[i];
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

bool LoadResources(const char* resourcePath) {
    ResourceManager* resourceManager = GetResourceManager();
    if (!resourceManager) {
        LOG_WARN(LOG_RESOURCE, "Resource manager not initialized");
        return false;
    }

    Logger_BeginTimer("LoadResources");

    // Load textures
    const char* texturePath = TextFormat("%s/textures", resourcePath);
    FilePathList textureFiles = LoadDirectoryFiles(texturePath);
    for (int i = 0; i < textureFiles.count; i++) {
        const char* filename = textureFiles.paths[i];
        if (IsFileExtension(filename, ".png")) {
            Texture2D* texture = LoadGameTexture(filename);
            if (texture != NULL) {
                LOG_INFO(LOG_RESOURCE, "Loaded texture: %s", filename);
            }
        }
    }
    UnloadDirectoryFiles(textureFiles);

    // Load audio
    const char* audioPath = TextFormat("%s/audio", resourcePath);
    FilePathList audioFiles = LoadDirectoryFiles(audioPath);
    for (int i = 0; i < audioFiles.count; i++) {
        const char* filename = audioFiles.paths[i];
        if (IsFileExtension(filename, ".wav")) {
            Wave* wave = LoadGameSound(filename);
            if (wave != NULL) {
                LOG_INFO(LOG_RESOURCE, "Loaded sound: %s", filename);
            }
        } else if (IsFileExtension(filename, ".ogg")) {
            Music* music = LoadGameMusic(filename);
            if (music != NULL) {
                LOG_INFO(LOG_RESOURCE, "Loaded music: %s", filename);
            }
        }
    }
    UnloadDirectoryFiles(audioFiles);

    return true;
} 