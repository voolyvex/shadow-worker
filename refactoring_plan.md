
Okay, here's a comprehensive refactoring plan to address the identified areas for improvement, focusing on the Component System, Entity System, Resource Management, and Map System. This plan prioritizes a more robust and idiomatic ECS architecture, improved naming conventions, and better resource handling.

**I. Component System Refinement**

1.  **Component Registration and Management:**

    *   **Goal:** Implement a dynamic component registration system that avoids hardcoding component types and simplifies adding new components.
    *   **Changes:**
        *   **Introduce `ComponentType`:** Replace `ComponentFlags` (an enum) with a `ComponentType` struct. This struct will hold a unique identifier (string or hash) and potentially metadata about the component (size, alignment).
        *   **Create `ComponentRegistry`:**
            *   Rename the existing `core/component_registry.h` and `core/component_registry.c` to `core/component_manager.h` and `core/component_manager.c`, respectively. This better reflects its role.
            *   The `ComponentManager` will store a dynamic array (or hash map) of
            `ComponentType` structs.
            *   Add functions:
                *   `ComponentManager_RegisterComponent(ComponentManager* manager, const char* name, size_t size, size_t alignment)`: Registers a new component type. Returns a `ComponentTypeID` (likely an integer index).
                *   `ComponentManager_GetComponentTypeID(ComponentManager* manager, const char* name)`: Retrieves the `ComponentTypeID` for a given component name.
                *   `ComponentManager_GetComponentData(ComponentManager* manager, EntityID entity, ComponentTypeID type)`: Retrieves a pointer to the component data for a given entity and component type.  This replaces the direct array access and `Get...Component` functions.
                *   `ComponentManager_HasComponent(ComponentManager* manager, EntityID entity, ComponentTypeID type)`: Checks if an entity has a specific component.
                *   `ComponentManager_AddComponent(ComponentManager* manager, EntityID entity, ComponentTypeID type, void* data)`: Adds a component to an entity, initializing it with the provided data (or default values if `data` is NULL).
                *   `ComponentManager_RemoveComponent(ComponentManager* manager, EntityID entity, ComponentTypeID type)`: Removes a component from an entity.
        *   **Modify `Entity`:**
            *   Remove `ComponentFlags components;` and `ComponentData components_data[MAX_COMPONENT_TYPES];`.
            *   Instead, the `EntityManager` (see below) will manage the association between entities and their components.
        *   **Refactor Existing Components:**
            *   For each existing component (e.g., `TransformComponent`, `RenderComponent`, `PhysicsComponent`, `AIComponent`, `ColliderComponent`), create a corresponding `ComponentType` registration call in the engine initialization.  For example:

                ```c
                // In engine initialization:
                ComponentTypeID transformType = ComponentManager_RegisterComponent(
                    componentManager,
                    "TransformComponent",
                    sizeof(TransformComponent),
                    alignof(TransformComponent)
                );
                ```

            *   Replace all instances of direct `components_data` access with calls to `ComponentManager_GetComponentData`.
            *   Replace all instances of `AddComponent(entity, COMPONENT_...)` with `ComponentManager_AddComponent`.
            *   Remove the `AddComponent` function from `src/entities/entity.c`.
            *   Remove the `GetComponent` function from `src/entities/entity.c`.
            *   Remove the `IterateComponents` function from `src/entities/entity.c`.
            *   Remove the `InitTransformComponent` function from `src/core/entity.c`.
            *   Remove the `InitColliderComponent` function from `src/core/entity.c`.
            *   Remove the `InitPhysicsComponent` function from `src/core/entity.c`.

2.  **Component Access:**

    *   **Goal:** Enforce consistent and safe access to component data through the `ComponentManager`.
    *   **Changes:** (These are covered in the `ComponentManager` changes above). The key is to *always* use `ComponentManager_GetComponentData` to access component data, and never access `components_data` directly.

**II. Entity System Enhancements**

1.  **Entity IDs:**

    *   **Goal:** Implement stable and unique entity IDs.
    *   **Changes:**
        *   **Introduce `EntityID`:** Create a new struct `EntityID` in `include/entities/entity_types.h`:

            ```c
            typedef struct {
                uint32_t index;
                uint32_t generation;
            } EntityID;
            ```

        *   **Modify `EntityPool`:**
            *   Rename `include/entity_pool.h` and `src/entity_pool.c` to `include/entities/entity_manager.h` and `src/entities/entity_manager.c`, respectively. This better reflects its broader role in a true ECS.
            *   Change the `entities` array in `EntityPool` to store `Entity` structs (or just metadata if component data is fully managed by `ComponentManager`). The `Entity` struct *only* contains active/inactive status.
            *   Add a `generations` array (of `uint32_t`) to the `EntityManager` to track entity generations.
            *   Modify `CreateEntity` (now `EntityManager_CreateEntity`):
                *   If a free entity slot is available, increment the generation counter for that slot.
                *   Return an `EntityID` struct containing the index and generation.
                *   Initialize the entity as inactive.
            *   Modify `DestroyEntity` (now `EntityManager_DestroyEntity`):
                *   Take an `EntityID` as input.
                *   Check if the provided `EntityID` is valid (index within bounds and generation matches).
                *   If valid, mark the entity as inactive and increment the generation counter for that slot.
            *   Add `EntityManager_IsValid(EntityManager* manager, EntityID entity)`: Checks if an `EntityID` is valid.
            *   Add `EntityManager_IsActive(EntityManager* manager, EntityID entity)`: Checks if an entity is active.
            *   Remove the `Entity` struct definition from `include/entity.h` and relocate it to `include/entities/entity_manager.h`.
            *   Remove `UpdateEntity` from `src/entities/entity.c`.
            *   Remove `DrawEntity` from `src/entities/entity.c`.

2.  **Systems (True ECS Systems):**

    *   **Goal:** Implement systems that operate on entities with specific component combinations.
    *   **Changes:**
        *   **Create System Functions:** For each logical system (e.g., Physics, Rendering, AI), create separate source files (e.g., `src/systems/physics_system.c`, `src/systems/render_system.c`, `src/systems/ai_system.c`).
        *   **System Update Functions:** Each system will have an `Update` function (e.g., `PhysicsSystem_Update`, `RenderSystem_Update`, `AISystem_Update`). These functions will:
            *   Take the `EntityManager`, `ComponentManager`, and `deltaTime` as input.
            *   Iterate through *all* active entities (using `EntityManager_IsActive`).
            *   For each entity, check if it has the required components for that system (using `ComponentManager_HasComponent`).
            *   If the entity has the required components, retrieve the component data (using `ComponentManager_GetComponentData`) and perform the system's logic.
        *   **Example (Physics System):**

            ```c
            // src/systems/physics_system.c
            void PhysicsSystem_Update(EntityManager* entityManager, ComponentManager* componentManager, float deltaTime) {
                ComponentTypeID transformType = ComponentManager_GetComponentTypeID(componentManager, "TransformComponent");
                ComponentTypeID physicsType = ComponentManager_GetComponentTypeID(componentManager, "PhysicsComponent");

                for (EntityID entity = {0}; entity.index < entityManager->capacity; entity.index++) {
                    if (EntityManager_IsActive(entityManager, entity)) {
                        if (ComponentManager_HasComponent(componentManager, entity, transformType) &&
                            ComponentManager_HasComponent(componentManager, entity, physicsType)) {

                            TransformComponent* transform = (TransformComponent*)ComponentManager_GetComponentData(componentManager, entity, transformType);
                            PhysicsComponent* physics = (PhysicsComponent*)ComponentManager_GetComponentData(componentManager, entity, physicsType);

                            // Apply physics calculations (e.g., velocity, acceleration)
                            transform->position.x += physics->velocity.x * deltaTime;
                            transform->position.y += physics->velocity.y * deltaTime;
                        }
                    }
                }
            }
            ```

        *   **Example (Render System):**

            ```c
            // src/systems/render_system.c
            void RenderSystem_Update(EntityManager* entityManager, ComponentManager* componentManager, float deltaTime) {
                ComponentTypeID transformType = ComponentManager_GetComponentTypeID(componentManager, "TransformComponent");
                ComponentTypeID renderType = ComponentManager_GetComponentTypeID(componentManager, "RenderComponent");

                for (EntityID entity = {0}; entity.index < entityManager->capacity; entity.index++) {
                    if (EntityManager_IsActive(entityManager, entity)) {
                        if (ComponentManager_HasComponent(componentManager, entity, transformType) &&
                            ComponentManager_HasComponent(componentManager, entity, renderType)) {

                            TransformComponent* transform = (TransformComponent*)ComponentManager_GetComponentData(componentManager, entity, transformType);
                            RenderComponent* render = (RenderComponent*)ComponentManager_GetComponentData(componentManager, entity, renderType);

                            // Draw the entity using Raylib functions, based on transform and render data
                            if (render->visible) {
                                DrawTextureRec(render->texture, render->sourceRect, transform->position, render->tint);
                            }
                        }
                    }
                }
            }
            ```
        *   **Integrate Systems into `UpdateWorld`:**  The `UpdateWorld` function (in `src/world/world.c`) will now call the `Update` functions of each system in the correct order.
        *   **Move `DrawWorld` Logic:** The drawing logic currently in `DrawWorld` (in `src/world/world.c`) should be moved into the `RenderSystem_Update` function. `DrawWorld` will then become very simple, primarily calling `RenderSystem_Update`.
        *   **Move `UpdateEntityPool` Logic:** The logic in `UpdateEntityPool` should be distributed among the relevant systems. Remove the `UpdateEntityPool` function.

**III. Resource Management Enhancements**

1.  **Resource Handles:**

    *   **Goal:** Use opaque handles to manage resources, improving safety and control.
    *   **Changes:**
        *   **Create `ResourceHandle`:** Define a `ResourceHandle` type (likely a `typedef` for an integer or pointer).
        *   **Modify `ResourceManager`:**
            *   Change resource loading functions (e.g., `LoadGameTexture`) to return a `ResourceHandle` instead of a raw pointer.
            *   Internally, the `ResourceManager` will maintain a mapping between handles and actual resource data (e.g., `Texture2D`, `Music`, `Wave`).
            *   Change resource access functions (e.g., `GetGameTexture`) to take a `ResourceHandle` and return a pointer to the resource data.  The `ResourceManager` will validate the handle before returning the pointer.
            *   Change resource unloading functions (e.g., `UnloadGameTexture`) to take a `ResourceHandle`.
        *   **Update Resource Usage:** Replace all direct uses of resource pointers with `ResourceHandle`s.

2.  **Asynchronous Loading (Conceptual Outline):**

    *   **Goal:** Load resources in the background to avoid blocking the main thread.
    *   **Changes (High-Level):**
        *   **Introduce a Loading Queue:** The `ResourceManager` will have a queue of resource loading requests.
        *   **Use Threads (or Coroutines):** Create a separate thread (or use a coroutine library) to process the loading queue.
        *   **Callbacks (or Futures):** When a resource finishes loading, the loading thread will trigger a callback (or set a future) to notify the main thread.
        *   **Non-Blocking Access:** The `GetGame...` functions will need to handle cases where a resource is still loading.  They might return a "placeholder" resource or provide a way to check if the resource is ready.

**IV. Map System**

1.  **Chunk Management:**

    *   **Goal:** Optimize chunk rendering by only redrawing changed tiles.
    *   **Changes:**
        *   **Introduce `TileChange` Struct:**

            ```c
            typedef struct {
                int x;
                int y;
                TileType oldType;
                TileType newType;
            } TileChange;
            ```

        *   **Modify `MapSystem`:**
            *   Add a `changes` array (or linked list) to the `MapSystem` to store `TileChange` structs.
            *   Whenever a tile is changed (e.g., through a `SetTile` function), add a `TileChange` record to the `changes` array.
            *   Modify `UpdateChunkTexture`:
                *   Instead of redrawing the entire chunk, iterate through the `changes` array.
                *   For each `TileChange`, redraw only the affected tile within the chunk's texture.
                *   Clear the `changes` array after processing.

2.  **Layer Management:**

    *   **Goal:** Allow dynamic creation and management of layers.
    *   **Changes:**
        *   **Modify `RenderLayer`:** Remove the `MAX_LAYERS` limit.
        *   **Modify `MapSystem`:**
            *   Change the `layers` array in `MapSystem` to a dynamically allocated array (or linked list) of `RenderLayer` structs.
            *   Add functions to create, destroy, and manage layers (e.g., `MapSystem_AddLayer`, `MapSystem_RemoveLayer`, `MapSystem_GetLayer`).

3. **Refactor Map System Functions:**
    * Rename `UpdateMapSystem` to `MapSystem_Update`.
    * Rename `RenderMapLayers` to `MapSystem_Render`.
    * Remove the `World* world` parameter from `UpdateMapSystem`.

**V. World and Game State**

1.  **Consistency:**

    *   **Goal:** Use `WorldState` consistently throughout the codebase.
    *   **Changes:**
        *   Embed the `World` struct *within* the `WorldState` struct. This simplifies access and makes the relationship clearer.
        *   Pass `WorldState*` to all functions that need access to the world or related systems.
        *   Remove the standalone `World` struct definition from `include/world/world.h` and relocate it inside the `WorldState` struct definition in the same file.

2. **Refactor World Functions:**
    * Remove `DestroyWorldStateAndResources`.
    * Rename `DestroyWorldState` to `WorldState_Destroy`.
    * Rename `UnloadWorldState` to `WorldState_Unload`.

**VI. Naming Conventions**

*   **Functions:** Use `PascalCase` for all functions (e.g., `CreateWorld`, `UpdateEntity`, `LoadResource`).
*   **Structs:** Use `PascalCase` for all struct names (e.g., `TransformComponent`, `EntityID`, `WorldState`).
*   **Variables:** Use `camelCase` for local variables and struct members (e.g., `deltaTime`, `playerPosition`, `transformComponent`).
*   **Constants:** Use `UPPER_SNAKE_CASE` for constants (e.g., `MAX_ENTITIES`, `WINDOW_WIDTH`, `COMPONENT_TRANSFORM`).
*   **Enums:** Use `PascalCase` for enum names and `UPPER_SNAKE_CASE` for enum values.
*   **File Names:** Use `snake_case` for all file names (e.g., `entity_manager.c`, `physics_system.h`).
*   **Prefix Systems:** Prefix system functions with the system name (e.g., `PhysicsSystem_Update`, `RenderSystem_Update`).
*   **Prefix Managers:** Prefix manager functions with the manager name (e.g., `EntityManager_CreateEntity`, `ComponentManager_RegisterComponent`).

**VII. File Relocations**

*   `include/entity_pool.h` -> `include/entities/entity_manager.h`
*   `src/entity_pool.c` -> `src/entities/entity_manager.c`
*   `core/component_registry.h` -> `core/component_manager.h`
*   `core/component_registry.c` -> `core/component_manager.c`
*   Move the `Entity` struct definition from `include/entity.h` to `include/entities/entity_manager.h`.
*   Move the standalone `World` struct definition from `include/world/world.h` to inside the `WorldState` struct definition in the same file.

**VIII. Removals**

*   Remove `ComponentFlags` (replaced by `ComponentTypeID`).
*   Remove `ComponentData components_data[MAX_COMPONENT_TYPES];` from the `Entity` struct.
*   Remove `ComponentFlags components;` from the `Entity` struct.
*   Remove the `AddComponent` function from `src/entities/entity.c`.
*   Remove the `GetComponent` function from `src/entities/entity.c`.
*   Remove the `IterateComponents` function from `src/entities/entity.c`.
*   Remove the `InitTransformComponent` function from `src/core/entity.c`.
*   Remove the `InitColliderComponent` function from `src/core/entity.c`.
*   Remove the `InitPhysicsComponent` function from `src/core/entity.c`.
*   Remove `UpdateEntity` from `src/entities/entity.c`.
*   Remove `DrawEntity` from `src/entities/entity.c`.
*   Remove the `UpdateEntityPool` function.
*   Remove `DestroyWorldStateAndResources`.
*   Remove the `World* world` parameter from `UpdateMapSystem`.

**IX. Documentation**

This file serves as comprehensive documentation for the refactoring process. It includes:

*   **Goals:** Clear objectives for each change.
*   **Changes:** Detailed descriptions of the modifications, including code examples where appropriate.
*   **File Relocations:** A list of files that need to be moved or renamed.
*   **Removals:** A list of functions and struct members that should be removed.
*   **Naming Conventions:** A guide to the consistent naming style to be used.
*   **Step-by-Step Instructions:** The changes are broken down into manageable steps.

This refactoring plan provides a significant overhaul of the engine, moving it towards a more robust and idiomatic ECS architecture. It improves code clarity, maintainability, and performance. The use of dynamic component registration, stable entity IDs, resource handles, and true ECS systems will make the engine more flexible and scalable. The improved chunk management and layer system will enhance the map rendering capabilities. The consistent naming conventions and comprehensive documentation will make the codebase easier to understand and work with.

