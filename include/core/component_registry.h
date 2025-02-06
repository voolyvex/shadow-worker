#ifndef CORE_COMPONENT_REGISTRY_H
#define CORE_COMPONENT_REGISTRY_H

#include <stddef.h>
#include <stdbool.h>
#include "component_flags.h"
#include "../entities/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Component Registry System
 * 
 * The Component Registry is responsible for managing component storage and access.
 * It works in conjunction with ComponentFlags to provide a complete ECS solution:
 * - ComponentFlags define WHAT components exist (the types)
 * - ComponentRegistry manages HOW components are stored and accessed (the data)
 */

/**
 * @brief Status codes for registry operations
 */
typedef enum {
    REGISTRY_OK,                  /**< Operation completed successfully */
    REGISTRY_INVALID_TYPE,        /**< Invalid component type specified */
    REGISTRY_OUT_OF_MEMORY,       /**< Memory allocation failed */
    REGISTRY_INVALID_ID,          /**< Entity ID out of range */
    REGISTRY_COMPONENT_NOT_FOUND  /**< Requested component doesn't exist */
} RegistryStatus;

// Component Registry management functions
/**
 * @brief Create a new component registry
 * @return Pointer to new registry or NULL if creation failed
 */
ComponentRegistry* CreateComponentRegistry(void);

/**
 * @brief Destroy a component registry and free all resources
 * @param registry Registry to destroy
 */
void DestroyComponentRegistry(ComponentRegistry* registry);

/**
 * @brief Get the array of components of a specific type
 * @param registry Registry to query
 * @param type Component type to retrieve
 * @return Pointer to component array or NULL if not found
 */
void* GetComponentArray(ComponentRegistry* registry, ComponentFlags type);

/**
 * @brief Add a new component type to the registry
 * @param registry Registry to modify
 * @param type Component type to add
 * @param componentSize Size of the component structure
 * @return Pointer to new component array or NULL if failed
 */
void* AddComponentToRegistry(ComponentRegistry* registry, ComponentFlags type, size_t componentSize);

/**
 * @brief Remove a component from an entity
 * @param registry Registry to modify
 * @param type Component type to remove
 * @param entityId Entity to remove component from
 */
void RemoveComponentFromRegistry(ComponentRegistry* registry, ComponentFlags type, size_t entityId);

// Component array management
/**
 * @brief Get a specific component for an entity
 * @param registry Registry to query
 * @param type Component type to retrieve
 * @param entityId Entity to get component for
 * @return Pointer to component or NULL if not found
 */
void* GetComponent(ComponentRegistry* registry, ComponentFlags type, size_t entityId);

/**
 * @brief Check if an entity has a specific component
 * @param registry Registry to query
 * @param type Component type to check
 * @param entityId Entity to check
 * @return true if component exists
 */
bool HasComponentInRegistry(ComponentRegistry* registry, ComponentFlags type, size_t entityId);

/**
 * @brief Clear all components from the registry
 * @param registry Registry to clear
 */
void ClearComponentRegistry(ComponentRegistry* registry);

/**
 * @brief Resize a component array
 * @param registry Registry to modify
 * @param type Component type to resize
 * @param newCapacity New capacity for the array
 */
void ResizeComponentArray(ComponentRegistry* registry, ComponentFlags type, size_t newCapacity);

// Error handling
/**
 * @brief Get the last error that occurred
 * @return Status code for last error
 */
RegistryStatus GetLastRegistryError(void);

/**
 * @brief Get a string description of an error code
 * @param status Status code to describe
 * @return Constant string describing the error
 */
const char* GetRegistryErrorString(RegistryStatus status);

#ifdef __cplusplus
}
#endif

#endif // CORE_COMPONENT_REGISTRY_H 