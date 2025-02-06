#include "utils/object_types.h"
#include "utils/debug.h"
#include "utils/logger.h"
#include <string.h>

const char* get_object_type_name(ObjectType type) {
    switch (type) {
        case OBJECT_TYPE_NONE:
            return "NONE";
        case OBJECT_TYPE_PLAYER:
            return "PLAYER";
        case OBJECT_TYPE_NPC:
            return "NPC";
        case OBJECT_TYPE_ITEM:
            return "ITEM";
        case OBJECT_TYPE_OBSTACLE:
            return "OBSTACLE";
        case OBJECT_TYPE_TRIGGER:
            return "TRIGGER";
        default:
            debug_log(LOG_LEVEL_WARN, "Unknown object type: %d", type);
            return "UNKNOWN";
    }
}

ObjectType get_object_type_from_name(const char* name) {
    if (!name) return OBJECT_TYPE_NONE;
    
    if (strcmp(name, "PLAYER") == 0) return OBJECT_TYPE_PLAYER;
    if (strcmp(name, "NPC") == 0) return OBJECT_TYPE_NPC;
    if (strcmp(name, "ITEM") == 0) return OBJECT_TYPE_ITEM;
    if (strcmp(name, "OBSTACLE") == 0) return OBJECT_TYPE_OBSTACLE;
    if (strcmp(name, "TRIGGER") == 0) return OBJECT_TYPE_TRIGGER;
    
    debug_log(LOG_LEVEL_WARN, "Unknown object type name: %s", name);
    return OBJECT_TYPE_NONE;
} 