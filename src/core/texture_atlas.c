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
    if (currentAtlas) {
        TraceLog(LOG_WARNING, "Texture atlas already initialized");
        return false;
    }
    
    currentAtlas = (TextureAtlas*)malloc(sizeof(TextureAtlas));
    if (!currentAtlas) return false;
    
    currentAtlas->width = width;
    currentAtlas->height = height;
    currentAtlas->tileSize = tileSize;
    currentAtlas->entryCount = 0;
    currentAtlas->entries = (AtlasEntry*)malloc(MAX_TILES_PER_ATLAS * sizeof(AtlasEntry));
    
    if (!currentAtlas->entries) {
        free(currentAtlas);
        currentAtlas = NULL;
        return false;
    }
    
    // Create render texture for atlas
    currentAtlas->atlas = LoadRenderTexture(width, height);
    if (currentAtlas->atlas.id == 0) {
        free(currentAtlas->entries);
        free(currentAtlas);
        currentAtlas = NULL;
        return false;
    }
    
    return true;
}

void UnloadTextureAtlas(void) {
    if (!currentAtlas) return;
    
    UnloadRenderTexture(currentAtlas->atlas);
    free(currentAtlas->entries);
    free(currentAtlas);
    currentAtlas = NULL;
}

bool AddTextureToAtlas(const char* name, const char* filename) {
    if (!currentAtlas || !name || !filename) return false;
    
    // Load source texture
    Image image = LoadImage(filename);
    // Check if image is valid (data is not NULL and dimensions are positive)
    if (!image.data || image.width <= 0 || image.height <= 0) {
        TraceLog(LOG_WARNING, "Failed to load image: %s", filename);
        return false;
    }
    
    // Resize if needed
    if (image.width != currentAtlas->tileSize || image.height != currentAtlas->tileSize) {
        ImageResize(&image, currentAtlas->tileSize, currentAtlas->tileSize);
    }
    
    // Calculate position in atlas
    int tilesPerRow = currentAtlas->width / (currentAtlas->tileSize + ATLAS_PADDING);
    int x = (currentAtlas->entryCount % tilesPerRow) * (currentAtlas->tileSize + ATLAS_PADDING);
    int y = (currentAtlas->entryCount / tilesPerRow) * (currentAtlas->tileSize + ATLAS_PADDING);
    
    // Check if we have space
    if (y + currentAtlas->tileSize > currentAtlas->height) {
        UnloadImage(image);
        return false;
    }
    
    // Convert to texture and draw to atlas
    Texture2D temp = LoadTextureFromImage(image);
    UnloadImage(image);
    
    BeginTextureMode(currentAtlas->atlas);
        DrawTexture(temp, x, y, WHITE);
    EndTextureMode();
    
    UnloadTexture(temp);
    
    // Store entry information
    currentAtlas->entries[currentAtlas->entryCount].name = _strdup(name);
    currentAtlas->entries[currentAtlas->entryCount].region = (Rectangle){
        (float)x,
        (float)y,
        (float)currentAtlas->tileSize,
        (float)currentAtlas->tileSize
    };
    currentAtlas->entryCount++;
    
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