#include "../include/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS

// External includes
#include <raylib.h>
#include <raymath.h>

END_EXTERNAL_WARNINGS

#include <stdlib.h>
#include <string.h>
#include "texture_manager.h"

#define MAX_TEXTURES 256

typedef struct TextureAtlas {
    Texture2D texture;
    Rectangle* regions;
    int regionCount;
} TextureAtlas;

struct TextureManager {
    TextureAtlas atlases[MAX_TEXTURES];
    int atlasCount;
};

TextureManager* CreateTextureManager(void) {
    // Verify raylib is initialized first
    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Cannot create texture manager - window/OpenGL context not initialized");
        return NULL;
    }

    TextureManager* manager = (TextureManager*)malloc(sizeof(TextureManager));
    if (!manager) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for texture manager");
        return NULL;
    }

    memset(manager, 0, sizeof(TextureManager));
    return manager;
}

void DestroyTextureManager(TextureManager* manager) {
    if (!manager) return;

    for (int i = 0; i < manager->atlasCount; i++) {
        if (manager->atlases[i].texture.id != 0) {
            UnloadTexture(manager->atlases[i].texture);
        }
        if (manager->atlases[i].regions) {
            free(manager->atlases[i].regions);
        }
    }

    free(manager);
    TraceLog(LOG_INFO, "Texture manager destroyed successfully");
}

int LoadTextureAtlas(TextureManager* manager, const char* fileName, Rectangle* regions, int regionCount) {
    if (!manager || !fileName || !regions || regionCount <= 0) {
        TraceLog(LOG_ERROR, "Invalid parameters for LoadTextureAtlas");
        return -1;
    }

    if (manager->atlasCount >= MAX_TEXTURES) {
        TraceLog(LOG_ERROR, "Maximum number of texture atlases reached (%d)", MAX_TEXTURES);
        return -1;
    }

    // Verify file exists
    if (!FileExists(fileName)) {
        TraceLog(LOG_ERROR, "Texture file not found: %s", fileName);
        return -1;
    }

    TextureAtlas* atlas = &manager->atlases[manager->atlasCount];
    
    // Load texture and verify
    atlas->texture = LoadTexture(fileName);
    if (atlas->texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", fileName);
        return -1;
    }

    // Allocate and verify regions
    atlas->regions = (Rectangle*)malloc(regionCount * sizeof(Rectangle));
    if (!atlas->regions) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for texture regions");
        UnloadTexture(atlas->texture);
        return -1;
    }

    // Validate region coordinates
    for (int i = 0; i < regionCount; i++) {
        if (regions[i].x < 0 || regions[i].y < 0 || 
            regions[i].width <= 0 || regions[i].height <= 0 ||
            regions[i].x + regions[i].width > atlas->texture.width ||
            regions[i].y + regions[i].height > atlas->texture.height) {
            TraceLog(LOG_ERROR, "Invalid region coordinates for atlas %s at index %d", fileName, i);
            UnloadTexture(atlas->texture);
            free(atlas->regions);
            return -1;
        }
    }

    memcpy(atlas->regions, regions, regionCount * sizeof(Rectangle));
    atlas->regionCount = regionCount;

    return manager->atlasCount++;
}

Texture2D GetTextureFromAtlas(TextureManager* manager, int atlasIndex, int regionIndex) {
    if (!manager) {
        TraceLog(LOG_ERROR, "Invalid texture manager");
        return (Texture2D){0};
    }

    if (atlasIndex < 0 || atlasIndex >= manager->atlasCount) {
        TraceLog(LOG_ERROR, "Invalid atlas index: %d", atlasIndex);
        return (Texture2D){0};
    }

    if (regionIndex < 0 || regionIndex >= manager->atlases[atlasIndex].regionCount) {
        TraceLog(LOG_ERROR, "Invalid region index: %d for atlas %d", regionIndex, atlasIndex);
        return (Texture2D){0};
    }

    return manager->atlases[atlasIndex].texture;
}

Rectangle GetTextureRegion(TextureManager* manager, int atlasIndex, int regionIndex) {
    if (!manager || atlasIndex >= manager->atlasCount || regionIndex >= manager->atlases[atlasIndex].regionCount) {
        return (Rectangle){0};
    }

    return manager->atlases[atlasIndex].regions[regionIndex];
} 