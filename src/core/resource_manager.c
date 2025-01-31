#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#endif

#include "../../include/resource_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <raylib.h>

#ifdef _WIN32
#define GetCurrentDir GetCurrentDirectoryA
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define strdup _strdup
#endif

// Error tracking
static struct {
    char lastError[256];
    int errorCount;
    bool hasError;
} errorState = {0};

// Set error message
static void SetError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(errorState.lastError, sizeof(errorState.lastError), format, args);
    va_end(args);
    errorState.errorCount++;
    errorState.hasError = true;
    TraceLog(LOG_ERROR, "Resource Manager Error: %s", errorState.lastError);
}

// Clear error state
static void ClearError(void) {
    errorState.lastError[0] = '\0';
    errorState.hasError = false;
}

// Get last error
const char* GetResourceError(void) {
    return errorState.lastError;
}

// Enhanced file checking
static bool ValidateFile(const char* path, const char* expectedExt) {
    if (!path) {
        SetError("NULL file path provided");
        return false;
    }

    if (!FileExists(path)) {
        SetError("File does not exist: %s", path);
        return false;
    }

    // Check file extension
    const char* ext = strrchr(path, '.');
    if (!ext) {
        SetError("File has no extension: %s", path);
        return false;
    }

    if (expectedExt && strcmp(ext + 1, expectedExt) != 0) {
        SetError("Invalid file extension for %s, expected .%s", path, expectedExt);
        return false;
    }

    return true;
}

// Enhanced memory allocation
static void* SafeAlloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        SetError("Failed to allocate %zu bytes", size);
        return NULL;
    }
    return ptr;
}

// Resource validation
static bool ValidateTexture(Texture2D texture) {
    if (texture.id == 0) {
        SetError("Invalid texture ID");
        return false;
    }
    if (texture.width <= 0 || texture.height <= 0) {
        SetError("Invalid texture dimensions: %dx%d", texture.width, texture.height);
        return false;
    }
    return true;
}

static bool ValidateSound(Sound sound) {
    if (sound.frameCount <= 0) {
        SetError("Invalid sound frame count: %d", sound.frameCount);
        return false;
    }
    return true;
}

static bool ValidateFont(Font font) {
    if (font.baseSize <= 0) {
        SetError("Invalid font base size: %d", font.baseSize);
        return false;
    }
    return true;
}

static bool ValidateShader(Shader shader) {
    if (shader.id == 0) {
        SetError("Invalid shader ID");
        return false;
    }
    return true;
}

// Helper function to safely open files
static FILE* SafeFileOpen(const char* filename, const char* mode) {
    FILE* file = NULL;
#ifdef _WIN32
    fopen_s(&file, filename, mode);
#else
    file = fopen(filename, mode);
#endif
    return file;
}

// Helper function for safe string copy
static char* SafeStrDup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* dup = (char*)malloc(len);
    if (dup) {
#ifdef _WIN32
        strcpy_s(dup, len, str);
#else
        strcpy(dup, str);
#endif
    }
    return dup;
}

static char* ResolvePath(const char* basePath, const char* relativePath) {
    static char fullPath[512];
    char baseDir[512];
    
    // Get the base directory from the manifest path
    char* lastSlash = strrchr(basePath, '/');
    if (!lastSlash) lastSlash = strrchr(basePath, '\\');
    
    if (lastSlash) {
        size_t baseLen = lastSlash - basePath + 1;
#ifdef _WIN32
        strncpy_s(baseDir, sizeof(baseDir), basePath, baseLen);
        baseDir[baseLen] = '\0';
#else
        strncpy(baseDir, basePath, baseLen);
        baseDir[baseLen] = '\0';
#endif
    } else {
        // If no slash found, use current directory
#ifdef _WIN32
        DWORD size = sizeof(baseDir);
        GetCurrentDirectoryA(size, baseDir);
        strcat_s(baseDir, sizeof(baseDir), "\\");
#else
        GetCurrentDir(baseDir, sizeof(baseDir));
        strcat(baseDir, "/");
#endif
    }
    
    // Handle absolute paths
    if (relativePath[0] == '/' || (isalpha(relativePath[0]) && relativePath[1] == ':')) {
#ifdef _WIN32
        strcpy_s(fullPath, sizeof(fullPath), relativePath);
#else
        strcpy(fullPath, relativePath);
#endif
        return fullPath;
    }
    
    // Combine base directory with relative path
#ifdef _WIN32
    strcpy_s(fullPath, sizeof(fullPath), baseDir);
    strcat_s(fullPath, sizeof(fullPath), relativePath);
#else
    strcpy(fullPath, baseDir);
    strcat(fullPath, relativePath);
#endif

    // Replace forward slashes with backslashes on Windows
#ifdef _WIN32
    for (char* p = fullPath; *p; p++) {
        if (*p == '/') *p = '\\';
    }
#endif

    TraceLog(LOG_DEBUG, "Resolved path: %s -> %s", relativePath, fullPath);
    return fullPath;
}

// Static resource manager instance
static ResourceManager manager;

// Get resource manager instance
ResourceManager* GetResourceManager(void) {
    return &manager;
}

// Default resources for fallback
static Texture2D defaultTexture;
static Font defaultFont;
static Sound defaultSound;
static Shader defaultShader;

bool InitResourceManager(void) {
    ClearError();

    // Initialize counts
    manager.textureCount = 0;
    manager.soundCount = 0;
    manager.fontCount = 0;
    manager.shaderCount = 0;

    // Initialize all resources as not loaded
    for (int i = 0; i < MAX_TEXTURES; i++) {
        manager.texturePool[i].isLoaded = false;
        manager.texturePool[i].name = NULL;
    }
    for (int i = 0; i < MAX_SOUNDS; i++) {
        manager.soundPool[i].isLoaded = false;
        manager.soundPool[i].name = NULL;
    }
    for (int i = 0; i < MAX_FONTS; i++) {
        manager.fontPool[i].isLoaded = false;
        manager.fontPool[i].name = NULL;
    }
    for (int i = 0; i < MAX_SHADERS; i++) {
        manager.shaderPool[i].isLoaded = false;
        manager.shaderPool[i].name = NULL;
    }

    // Create default white texture
    Image whiteImage = GenImageColor(32, 32, WHITE);
    defaultTexture = LoadTextureFromImage(whiteImage);
    UnloadImage(whiteImage);
    
    // Use raylib's default font
    defaultFont = GetFontDefault();
    
    // Create empty sound
    Wave emptyWave = {
        .data = NULL,
        .frameCount = 1,
        .sampleRate = 44100,
        .sampleSize = 16,
        .channels = 1
    };
    defaultSound = LoadSoundFromWave(emptyWave);
    
    // Use default shader (null shader means default shader in raylib)
    defaultShader = LoadShaderFromMemory(NULL, NULL);

    if (!ValidateTexture(defaultTexture)) {
        SetError("Failed to create default texture");
        return false;
    }

    TraceLog(LOG_INFO, "Resource manager initialized successfully");
    TraceLog(LOG_INFO, "Texture pool: %d/%d", manager.textureCount, MAX_TEXTURES);
    TraceLog(LOG_INFO, "Sound pool: %d/%d", manager.soundCount, MAX_SOUNDS);
    TraceLog(LOG_INFO, "Font pool: %d/%d", manager.fontCount, MAX_FONTS);
    TraceLog(LOG_INFO, "Shader pool: %d/%d", manager.shaderCount, MAX_SHADERS);

    // Fix GetCurrentDirectory usage
    #ifdef _WIN32
    char manifestPath[MAX_PATH];
    DWORD bufferSize = MAX_PATH;
    if (GetCurrentDirectoryA(bufferSize, manifestPath) == 0) {
        TraceLog(LOG_ERROR, "Failed to get current directory");
        return false;
    }
    strcat_s(manifestPath, MAX_PATH, "\\resources\\manifest.txt");
    #else
    char manifestPath[PATH_MAX];
    if (getcwd(manifestPath, sizeof(manifestPath)) == NULL) {
        TraceLog(LOG_ERROR, "Failed to get current directory");
        return false;
    }
    strcat(manifestPath, "/resources/manifest.txt");
    #endif

    return true;
}

void UnloadResourceManager(void) {
    // Unload all textures
    for (int i = 0; i < manager.textureCount; i++) {
        if (manager.texturePool[i].isLoaded) {
            UnloadTexture(manager.texturePool[i].texture);
            free(manager.texturePool[i].name);
        }
    }

    // Unload all sounds
    for (int i = 0; i < manager.soundCount; i++) {
        if (manager.soundPool[i].isLoaded) {
            UnloadSound(manager.soundPool[i].sound);
            free(manager.soundPool[i].name);
        }
    }

    // Unload all fonts
    for (int i = 0; i < manager.fontCount; i++) {
        if (manager.fontPool[i].isLoaded) {
            UnloadFont(manager.fontPool[i].font);
            free(manager.fontPool[i].name);
        }
    }

    // Unload all shaders
    for (int i = 0; i < manager.shaderCount; i++) {
        if (manager.shaderPool[i].isLoaded) {
            UnloadShader(manager.shaderPool[i].shader);
            free(manager.shaderPool[i].name);
        }
    }

    // Unload default resources
    UnloadTexture(defaultTexture);
    UnloadFont(defaultFont);
    UnloadSound(defaultSound);
    UnloadShader(defaultShader);

    TraceLog(LOG_INFO, "Resource manager unloaded");
}

Texture2D* LoadGameTexture(const char* name, const char* path) {
    ClearError();

    if (!name) {
        SetError("NULL texture name provided");
        return &defaultTexture;
    }

    // Check for existing texture
    for (int i = 0; i < manager.textureCount; i++) {
        if (manager.texturePool[i].isLoaded && strcmp(manager.texturePool[i].name, name) == 0) {
            return &manager.texturePool[i].texture;
        }
    }

    // Validate capacity
    if (manager.textureCount >= MAX_TEXTURES) {
        SetError("Maximum texture count reached (%d)", MAX_TEXTURES);
        return &defaultTexture;
    }

    // Validate file
    if (!ValidateFile(path, "png")) {
        return &defaultTexture;
    }

    // Load the texture
    Texture2D texture = LoadTexture(path);
    if (!ValidateTexture(texture)) {
        return &defaultTexture;
    }

    // Store the texture
    int index = manager.textureCount++;
    manager.texturePool[index].texture = texture;
    manager.texturePool[index].name = SafeStrDup(name);
    if (!manager.texturePool[index].name) {
        UnloadTexture(texture);
        manager.textureCount--;
        return &defaultTexture;
    }
    manager.texturePool[index].isLoaded = true;

    TraceLog(LOG_INFO, "Loaded texture '%s' from %s", name, path);
    return &manager.texturePool[index].texture;
}

Sound* LoadGameSound(const char* name, const char* path) {
    // Check if sound already exists
    for (int i = 0; i < manager.soundCount; i++) {
        if (manager.soundPool[i].isLoaded && strcmp(manager.soundPool[i].name, name) == 0) {
            return &manager.soundPool[i].sound;
        }
    }

    // Check if we have room for another sound
    if (manager.soundCount >= MAX_SOUNDS) {
        TraceLog(LOG_WARNING, "Cannot load sound %s: Maximum sound count reached", name);
        return &defaultSound;
    }

    // Load the sound
    Sound sound = LoadSound(path);
    if (sound.frameCount == 0) {
        TraceLog(LOG_WARNING, "Failed to load sound %s from %s", name, path);
        return &defaultSound;
    }

    // Store the sound
    int index = manager.soundCount++;
    manager.soundPool[index].sound = sound;
    manager.soundPool[index].name = SafeStrDup(name);
    manager.soundPool[index].isLoaded = true;

    TraceLog(LOG_INFO, "Loaded sound %s from %s", name, path);
    return &manager.soundPool[index].sound;
}

Font* LoadGameFont(const char* name, const char* path) {
    // Check if font already exists
    for (int i = 0; i < manager.fontCount; i++) {
        if (manager.fontPool[i].isLoaded && strcmp(manager.fontPool[i].name, name) == 0) {
            return &manager.fontPool[i].font;
        }
    }

    // Check if we have room for another font
    if (manager.fontCount >= MAX_FONTS) {
        TraceLog(LOG_WARNING, "Cannot load font %s: Maximum font count reached", name);
        return &defaultFont;
    }

    // Load the font
    Font font = LoadFont(path);
    if (font.baseSize == 0) {
        TraceLog(LOG_WARNING, "Failed to load font %s from %s", name, path);
        return &defaultFont;
    }

    // Store the font
    int index = manager.fontCount++;
    manager.fontPool[index].font = font;
    manager.fontPool[index].name = SafeStrDup(name);
    manager.fontPool[index].isLoaded = true;

    TraceLog(LOG_INFO, "Loaded font %s from %s", name, path);
    return &manager.fontPool[index].font;
}

Shader* LoadGameShader(const char* name, const char* vsPath, const char* fsPath) {
    // Check if shader already exists
    for (int i = 0; i < manager.shaderCount; i++) {
        if (manager.shaderPool[i].isLoaded && strcmp(manager.shaderPool[i].name, name) == 0) {
            return &manager.shaderPool[i].shader;
        }
    }

    // Check if we have room for another shader
    if (manager.shaderCount >= MAX_SHADERS) {
        TraceLog(LOG_WARNING, "Cannot load shader %s: Maximum shader count reached", name);
        return &defaultShader;
    }

    // Load the shader
    Shader shader = LoadShader(vsPath, fsPath);
    if (shader.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load shader %s from %s and %s", name, vsPath, fsPath);
        return &defaultShader;
    }

    // Store the shader
    int index = manager.shaderCount++;
    manager.shaderPool[index].shader = shader;
    manager.shaderPool[index].name = SafeStrDup(name);
    manager.shaderPool[index].isLoaded = true;

    TraceLog(LOG_INFO, "Loaded shader %s from %s and %s", name, vsPath, fsPath);
    return &manager.shaderPool[index].shader;
}

Texture2D* GetTexture(const char* name) {
    for (int i = 0; i < manager.textureCount; i++) {
        if (manager.texturePool[i].isLoaded && strcmp(manager.texturePool[i].name, name) == 0) {
            return &manager.texturePool[i].texture;
        }
    }
    TraceLog(LOG_WARNING, "Texture %s not found, using default", name);
    return &defaultTexture;
}

Sound* GetSound(const char* name) {
    for (int i = 0; i < manager.soundCount; i++) {
        if (manager.soundPool[i].isLoaded && strcmp(manager.soundPool[i].name, name) == 0) {
            return &manager.soundPool[i].sound;
        }
    }
    TraceLog(LOG_WARNING, "Sound %s not found, using default", name);
    return &defaultSound;
}

Font* GetFont(const char* name) {
    for (int i = 0; i < manager.fontCount; i++) {
        if (manager.fontPool[i].isLoaded && strcmp(manager.fontPool[i].name, name) == 0) {
            return &manager.fontPool[i].font;
        }
    }
    TraceLog(LOG_WARNING, "Font %s not found, using default", name);
    return &defaultFont;
}

Shader* GetShader(const char* name) {
    for (int i = 0; i < manager.shaderCount; i++) {
        if (manager.shaderPool[i].isLoaded && strcmp(manager.shaderPool[i].name, name) == 0) {
            return &manager.shaderPool[i].shader;
        }
    }
    TraceLog(LOG_WARNING, "Shader %s not found, using default", name);
    return &defaultShader;
}

void PrintResourceStats(ResourceManager* mgr) {
    if (!mgr) return;
    
    printf("Resource Manager Statistics:\n");
    printf("Textures: %d/%d\n", mgr->textureCount, MAX_TEXTURES);
    printf("Sounds: %d/%d\n", mgr->soundCount, MAX_SOUNDS);
    printf("Fonts: %d/%d\n", mgr->fontCount, MAX_FONTS);
    printf("Shaders: %d/%d\n", mgr->shaderCount, MAX_SHADERS);
}

bool IsResourceLoaded(ResourceManager* mgr, const char* name, ResourceType type) {
    if (!mgr || !name) return false;
    
    switch (type) {
        case RESOURCE_TEXTURE:
            for (int i = 0; i < mgr->textureCount; i++) {
                if (mgr->texturePool[i].isLoaded && 
                    strcmp(mgr->texturePool[i].name, name) == 0) {
                    return true;
                }
            }
            break;
            
        case RESOURCE_SOUND:
            for (int i = 0; i < mgr->soundCount; i++) {
                if (mgr->soundPool[i].isLoaded && 
                    strcmp(mgr->soundPool[i].name, name) == 0) {
                    return true;
                }
            }
            break;
            
        case RESOURCE_FONT:
            for (int i = 0; i < mgr->fontCount; i++) {
                if (mgr->fontPool[i].isLoaded && 
                    strcmp(mgr->fontPool[i].name, name) == 0) {
                    return true;
                }
            }
            break;
            
        case RESOURCE_SHADER:
            for (int i = 0; i < mgr->shaderCount; i++) {
                if (mgr->shaderPool[i].isLoaded && 
                    strcmp(mgr->shaderPool[i].name, name) == 0) {
                    return true;
                }
            }
            break;
    }
    
    return false;
}

void UnloadResourceBatch(ResourceManager* mgr, ResourceType type) {
    if (!mgr) return;
    
    switch (type) {
        case RESOURCE_TEXTURE:
            for (int i = 0; i < mgr->textureCount; i++) {
                if (mgr->texturePool[i].isLoaded) {
                    UnloadTexture(mgr->texturePool[i].texture);
                    free(mgr->texturePool[i].name);
                    mgr->texturePool[i].isLoaded = false;
                }
            }
            mgr->textureCount = 0;
            break;
            
        case RESOURCE_SOUND:
            for (int i = 0; i < mgr->soundCount; i++) {
                if (mgr->soundPool[i].isLoaded) {
                    UnloadSound(mgr->soundPool[i].sound);
                    free(mgr->soundPool[i].name);
                    mgr->soundPool[i].isLoaded = false;
                }
            }
            mgr->soundCount = 0;
            break;
            
        case RESOURCE_FONT:
            for (int i = 0; i < mgr->fontCount; i++) {
                if (mgr->fontPool[i].isLoaded) {
                    UnloadFont(mgr->fontPool[i].font);
                    free(mgr->fontPool[i].name);
                    mgr->fontPool[i].isLoaded = false;
                }
            }
            mgr->fontCount = 0;
            break;
            
        case RESOURCE_SHADER:
            for (int i = 0; i < mgr->shaderCount; i++) {
                if (mgr->shaderPool[i].isLoaded) {
                    UnloadShader(mgr->shaderPool[i].shader);
                    free(mgr->shaderPool[i].name);
                    mgr->shaderPool[i].isLoaded = false;
                }
            }
            mgr->shaderCount = 0;
            break;
    }
} 