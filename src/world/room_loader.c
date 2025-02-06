#include "utils/warning_suppression.h"
#include "world/room_loader.h"
#include "world/world.h"
#include "world/map_types.h"
#include "utils/debug.h"
#include "utils/logger.h"

BEGIN_EXTERNAL_WARNINGS
#include <stdlib.h>
#include <string.h>
END_EXTERNAL_WARNINGS

RoomTemplate* LoadRoomTemplate(const char* templatePath) {
    if (!templatePath) {
        LOG_ERROR(LOG_CORE, "Invalid template path");
        return NULL;
    }

    RoomTemplate* template = (RoomTemplate*)malloc(sizeof(RoomTemplate));
    if (!template) {
        LOG_ERROR(LOG_CORE, "Failed to allocate room template");
        return NULL;
    }

    // TODO: Implement room template loading from file
    template->width = 0;
    template->height = 0;
    template->tiles = NULL;
    template->objects = NULL;
    template->spawnPoints = NULL;
    template->spawnCount = 0;
    template->connections = NULL;
    template->connectionCount = 0;

    return template;
}

void UnloadRoomTemplate(RoomTemplate* template) {
    if (!template) return;

    if (template->tiles) {
        for (int i = 0; i < template->height; i++) {
            free(template->tiles[i]);
        }
        free(template->tiles);
    }

    if (template->objects) {
        for (int i = 0; i < template->height; i++) {
            free(template->objects[i]);
        }
        free(template->objects);
    }

    free(template->spawnPoints);
    free(template->connections);
    free(template);
}

bool ValidateRoomTemplate(const RoomTemplate* template) {
    if (!template) return false;
    if (template->width <= 0 || template->height <= 0) return false;
    if (!template->tiles || !template->objects) return false;
    return true;
}

bool CheckConnectionCompatibility(const RoomTemplate* room1, const RoomTemplate* room2) {
    if (!room1 || !room2) return false;
    if (!room1->connections || !room2->connections) return false;
    
    // TODO: Implement connection compatibility checking
    return true;
} 