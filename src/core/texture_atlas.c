#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS
#include <raylib.h>
#include <raymath.h>
END_EXTERNAL_WARNINGS

#include "../../include/texture_atlas.h"
#include "../../include/resource_manager.h"
#include <stdlib.h>
#include <string.h>

#define MAX_TILES_PER_ATLAS 256
#define ATLAS_PADDING 1

typedef struct {
    Rectangle region;
    const char* name;
} AtlasEntry;

typedef struct {
    RenderTexture2D atlas;
    AtlasEntry* entries;
    int entryCount;
    int width;
    int height;
    int tileSize;
} TextureAtlas;

static TextureAtlas* currentAtlas = NULL;

bool InitTextureAtlas(int width, int height, int tileSize) {
    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Cannot initialize texture atlas - window/OpenGL context not initialized");
        return false;
    }

    // Validate dimensions
    if (width <= 0 || height <= 0) {
        TraceLog(LOG_ERROR, "Invalid atlas dimensions (width: %d, height: %d)", width, height);
        return false;
    }

    if (tileSize <= 0 || tileSize > width || tileSize > height) {
        TraceLog(LOG_ERROR, "Invalid tile size %d (must be positive and not larger than atlas dimensions)", tileSize);
        return false;
    }

    // Check power of two dimensions (recommended for textures)
    if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
        TraceLog(LOG_WARNING, "Atlas dimensions are not power of two - this may impact performance");
    }

    // Clean up existing atlas if any
    if (currentAtlas) {
        TraceLog(LOG_WARNING, "Texture atlas already exists - cleaning up");
        UnloadTextureAtlas();
    }

    // Allocate atlas structure
    currentAtlas = (TextureAtlas*)MemAlloc(sizeof(TextureAtlas));
    if (!currentAtlas) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for texture atlas");
        return false;
    }

    // Initialize atlas properties
    currentAtlas->width = width;
    currentAtlas->height = height;
    currentAtlas->tileSize = tileSize;
    currentAtlas->entryCount = 0;

    // Allocate entries array
    currentAtlas->entries = (AtlasEntry*)MemAlloc(MAX_TILES_PER_ATLAS * sizeof(AtlasEntry));
    if (!currentAtlas->entries) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for atlas entries");
        MemFree(currentAtlas);
        currentAtlas = NULL;
        return false;
    }

    // Initialize render texture
    currentAtlas->atlas = LoadRenderTexture(width, height);
    if (currentAtlas->atlas.id == 0 || currentAtlas->atlas.texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to create render texture for atlas");
        MemFree(currentAtlas->entries);
        MemFree(currentAtlas);
        currentAtlas = NULL;
        return false;
    }

    // Clear atlas to transparent
    BeginTextureMode(currentAtlas->atlas);
    ClearBackground(BLANK);
    EndTextureMode();

    // Initialize entries
    memset(currentAtlas->entries, 0, MAX_TILES_PER_ATLAS * sizeof(AtlasEntry));

    TraceLog(LOG_INFO, "Texture atlas initialized successfully (%dx%d, tile size: %d)", 
             width, height, tileSize);
    return true;
}

void UnloadTextureAtlas(void) {
    if (!currentAtlas) return;
    
    if (currentAtlas->atlas.id != 0) {
        UnloadRenderTexture(currentAtlas->atlas);
    }
    
    if (currentAtlas->entries) {
        for (int i = 0; i < currentAtlas->entryCount; i++) {
            if (currentAtlas->entries[i].name) {
                free((void*)currentAtlas->entries[i].name);
            }
        }
        free(currentAtlas->entries);
    }
    
    free(currentAtlas);
    currentAtlas = NULL;
    TraceLog(LOG_INFO, "Texture atlas unloaded successfully");
}

bool AddTextureToAtlas(const char* name, const char* filename) {
    if (!currentAtlas) {
        TraceLog(LOG_ERROR, "Texture atlas not initialized");
        return false;
    }

    if (!name || !filename) {
        TraceLog(LOG_ERROR, "Invalid parameters (name: %p, filename: %p)", name, filename);
        return false;
    }

    if (currentAtlas->entryCount >= MAX_TILES_PER_ATLAS) {
        TraceLog(LOG_ERROR, "Atlas is full (max tiles: %d)", MAX_TILES_PER_ATLAS);
        return false;
    }

    // Check for duplicate names
    for (int i = 0; i < currentAtlas->entryCount; i++) {
        if (currentAtlas->entries[i].name && strcmp(currentAtlas->entries[i].name, name) == 0) {
            TraceLog(LOG_ERROR, "Texture '%s' already exists in atlas", name);
            return false;
        }
    }

    // Verify file exists and can be opened
    if (!FileExists(filename)) {
        TraceLog(LOG_ERROR, "Texture file not found: %s", filename);
        return false;
    }

    // Load and validate image
    Image image = LoadImage(filename);
    if (!image.data) {
        TraceLog(LOG_ERROR, "Failed to load image: %s", filename);
        return false;
    }

    // Validate image format and dimensions
    if (image.format <= 0 || image.width <= 0 || image.height <= 0) {
        TraceLog(LOG_ERROR, "Invalid image format or dimensions: %s", filename);
        UnloadImage(image);
        return false;
    }

    // Check if image fits in atlas
    if (image.width > currentAtlas->width || image.height > currentAtlas->height) {
        TraceLog(LOG_ERROR, "Image too large for atlas: %s (%dx%d > %dx%d)", 
                filename, image.width, image.height, currentAtlas->width, currentAtlas->height);
        UnloadImage(image);
        return false;
    }

    // Resize if needed
    if (image.width != currentAtlas->tileSize || image.height != currentAtlas->tileSize) {
        Image resized = image;
        ImageResize(&resized, currentAtlas->tileSize, currentAtlas->tileSize);
        if (!resized.data) {
            TraceLog(LOG_ERROR, "Failed to resize image: %s", filename);
            UnloadImage(image);
            return false;
        }
        image = resized;
    }

    // Calculate position in atlas
    int tilesPerRow = currentAtlas->width / (currentAtlas->tileSize + ATLAS_PADDING);
    int x = (currentAtlas->entryCount % tilesPerRow) * (currentAtlas->tileSize + ATLAS_PADDING);
    int y = (currentAtlas->entryCount / tilesPerRow) * (currentAtlas->tileSize + ATLAS_PADDING);

    // Verify position is within bounds
    if (y + currentAtlas->tileSize > currentAtlas->height) {
        TraceLog(LOG_ERROR, "Not enough space in atlas for image: %s", filename);
        UnloadImage(image);
        return false;
    }

    // Create texture and add to atlas
    Texture2D temp = LoadTextureFromImage(image);
    UnloadImage(image);

    if (temp.id == 0) {
        TraceLog(LOG_ERROR, "Failed to create texture from image: %s", filename);
        return false;
    }

    // Draw to atlas
    BeginTextureMode(currentAtlas->atlas);
    DrawTexture(temp, x, y, WHITE);
    EndTextureMode();

    UnloadTexture(temp);

    // Store entry information
    char* nameCopy = _strdup(name);
    if (!nameCopy) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for texture name");
        return false;
    }

    currentAtlas->entries[currentAtlas->entryCount].name = nameCopy;
    currentAtlas->entries[currentAtlas->entryCount].region = (Rectangle){
        (float)x,
        (float)y,
        (float)currentAtlas->tileSize,
        (float)currentAtlas->tileSize
    };

    currentAtlas->entryCount++;

    TraceLog(LOG_INFO, "Added texture '%s' to atlas at (%d,%d)", name, x, y);
    return true;
}

Rectangle GetTextureRegion(const char* name) {
    if (!currentAtlas || !name) return (Rectangle){0};
    
    for (int i = 0; i < currentAtlas->entryCount; i++) {
        if (strcmp(currentAtlas->entries[i].name, name) == 0) {
            return currentAtlas->entries[i].region;
        }
    }
    
    return (Rectangle){0};
}

const Texture2D* GetAtlasTexture(void) {
    return currentAtlas ? &currentAtlas->atlas.texture : NULL;
}

bool GenerateAtlasFromDirectory(const char* directory) {
    if (!currentAtlas || !directory) return false;
    
    // TODO: Implement directory scanning and automatic atlas generation
    // This would involve:
    // 1. Scanning directory for PNG files
    // 2. Sorting them by name
    // 3. Adding each to the atlas
    // 4. Generating a tileset configuration file
    
    return true;
}

bool SaveAtlasMetadata(const char* filename) {
    if (!currentAtlas || !filename) return false;
    
    // TODO: Implement metadata saving
    // This would save:
    // 1. Atlas dimensions
    // 2. Tile size
    // 3. Entry information (name, region)
    // 4. Additional properties
    
    return true;
} 