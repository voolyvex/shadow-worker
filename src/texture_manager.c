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
    TextureManager* manager = (TextureManager*)malloc(sizeof(TextureManager));
    if (!manager) return NULL;

    memset(manager, 0, sizeof(TextureManager));
    return manager;
}

void DestroyTextureManager(TextureManager* manager) {
    if (!manager) return;

    for (int i = 0; i < manager->atlasCount; i++) {
        UnloadTexture(manager->atlases[i].texture);
        free(manager->atlases[i].regions);
    }

    free(manager);
}

int LoadTextureAtlas(TextureManager* manager, const char* fileName, Rectangle* regions, int regionCount) {
    if (!manager || manager->atlasCount >= MAX_TEXTURES) return -1;

    TextureAtlas* atlas = &manager->atlases[manager->atlasCount];
    atlas->texture = LoadTexture(fileName);
    if (!atlas->texture.id) return -1;

    atlas->regions = (Rectangle*)malloc(regionCount * sizeof(Rectangle));
    if (!atlas->regions) {
        UnloadTexture(atlas->texture);
        return -1;
    }

    memcpy(atlas->regions, regions, regionCount * sizeof(Rectangle));
    atlas->regionCount = regionCount;

    return manager->atlasCount++;
}

Texture2D GetTextureFromAtlas(TextureManager* manager, int atlasIndex, int regionIndex) {
    if (!manager || atlasIndex >= manager->atlasCount || regionIndex >= manager->atlases[atlasIndex].regionCount) {
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