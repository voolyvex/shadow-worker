#include "world_system.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <queue>
#include <algorithm>
#include <chrono>
#include <godot_cpp/classes/tile_map.hpp>

using namespace godot;

void WorldSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("clear_world"), &WorldSystem::clear_world);
    ClassDB::bind_method(D_METHOD("generate_world", "params"), &WorldSystem::generate_world);
    ClassDB::bind_method(D_METHOD("get_tile_position", "world_pos"), &WorldSystem::get_tile_position);
    ClassDB::bind_method(D_METHOD("get_world_position", "tile_pos"), &WorldSystem::get_world_position);
    ClassDB::bind_method(D_METHOD("is_position_valid", "pos"), &WorldSystem::is_position_valid);
    ClassDB::bind_method(D_METHOD("get_placeable_objects"), &WorldSystem::get_placeable_objects);
}

WorldSystem::WorldSystem() : 
    min_room_size(5),
    max_room_size(15),
    min_rooms(5),
    max_rooms(10),
    current_seed(0),
    object_density(0.1f),
    max_placement_attempts(50),
    min_npcs_per_room(1),
    max_npcs_per_room(3) {
    
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    rng.seed(static_cast<unsigned int>(millis));
    
    register_placeable_objects();
}

WorldSystem::~WorldSystem() {
    // Cleanup if needed
}

void WorldSystem::_init() {
    // Initialize properties
}

void WorldSystem::_ready() {
    tile_map = get_node<TileMap>("TileMap");
    if (!tile_map) {
        UtilityFunctions::print("Error: TileMap node not found");
        return;
    }
    
    initialize_placeable_objects();
    initialize_spawn_points();
    initialize_navigation();
}

void WorldSystem::initialize_placeable_objects() {
    // Create workbench
    PlaceableObject workbench;
    workbench.type = "workbench";
    workbench.position = Vector2(100, 100);
    workbench.properties["crafting_level"] = 1;
    workbench.properties["durability"] = 100;
    placeable_objects.push_back(workbench);
    
    // Create altar
    PlaceableObject altar;
    altar.type = "altar";
    altar.position = Vector2(300, 300);
    altar.properties["ritual_power"] = 1;
    altar.properties["corruption"] = 0;
    placeable_objects.push_back(altar);
    
    // Create library
    PlaceableObject library;
    library.type = "library";
    library.position = Vector2(500, 100);
    library.properties["knowledge_level"] = 1;
    library.properties["book_count"] = 50;
    placeable_objects.push_back(library);
    
    // Create laboratory
    PlaceableObject laboratory;
    laboratory.type = "laboratory";
    laboratory.position = Vector2(700, 300);
    laboratory.properties["research_level"] = 1;
    laboratory.properties["contamination"] = 0;
    placeable_objects.push_back(laboratory);
    
    // Create ritual circle
    PlaceableObject ritual_circle;
    ritual_circle.type = "ritual_circle";
    ritual_circle.position = Vector2(900, 500);
    ritual_circle.properties["power_level"] = 1;
    ritual_circle.properties["stability"] = 100;
    placeable_objects.push_back(ritual_circle);
}

void WorldSystem::initialize_spawn_points() {
    // Add spawn points at strategic locations
    spawn_points.push_back(Vector2(150, 150));
    spawn_points.push_back(Vector2(450, 450));
    spawn_points.push_back(Vector2(750, 150));
    spawn_points.push_back(Vector2(150, 750));
}

void WorldSystem::initialize_navigation() {
    // Set up navigation grid
    if (tile_map) {
        // Get tilemap dimensions
        Rect2 bounds = tile_map->get_used_rect();
        navigation_grid.resize(bounds.size.x * bounds.size.y);
        
        // Initialize navigation costs
        for (int x = 0; x < bounds.size.x; x++) {
            for (int y = 0; y < bounds.size.y; y++) {
                int index = x + y * static_cast<int>(bounds.size.x);
                navigation_grid[index] = Vector2i(x, y);
            }
        }
    }
}

void WorldSystem::clear_world() {
    if (tile_map) {
        tile_map->clear();
    }
    placeable_objects.clear();
    spawn_points.clear();
}

void WorldSystem::generate_world(const Dictionary& params) {
    if (!tile_map) return;
    
    // Get generation parameters
    int width = params.has("width") ? static_cast<int>(params["width"]) : 100;
    int height = params.has("height") ? static_cast<int>(params["height"]) : 100;
    float noise_scale = params.has("noise_scale") ? static_cast<float>(params["noise_scale"]) : 0.1f;
    
    // Use random seed if not provided
    int world_seed = params.has("seed") ? static_cast<int>(params["seed"]) : UtilityFunctions::randi();
    UtilityFunctions::seed(world_seed);
    
    // Generate terrain
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float noise_value = UtilityFunctions::randf();
            int tile_index = determine_tile_type(noise_value);
            tile_map->set_cell(0, Vector2i(x, y), 0, Vector2i(tile_index, 0));
        }
    }
    
    // Place objects
    place_objects();
}

int WorldSystem::determine_tile_type(float noise_value) const {
    if (noise_value < 0.2f) return 0; // Water
    if (noise_value < 0.4f) return 1; // Sand
    if (noise_value < 0.7f) return 2; // Grass
    if (noise_value < 0.9f) return 3; // Forest
    return 4; // Mountain
}

void WorldSystem::place_objects() {
    if (!tile_map) return;
    
    // Place each object at its designated position
    for (const PlaceableObject& obj : placeable_objects) {
        Vector2i tile_pos = get_tile_position(obj.position);
        if (is_position_valid(obj.position)) {
            // Place object marker on tilemap
            tile_map->set_cell(1, tile_pos, 1, Vector2i(get_object_tile_index(obj.type), 0));
        }
    }
}

int WorldSystem::get_object_tile_index(const String& type) const {
    if (type == "workbench") return 0;
    if (type == "altar") return 1;
    if (type == "library") return 2;
    if (type == "laboratory") return 3;
    if (type == "ritual_circle") return 4;
    return 0;
}

Vector2i WorldSystem::get_tile_position(const Vector2& world_pos) const {
    if (!tile_map) return Vector2i(0, 0);
    return tile_map->local_to_map(world_pos);
}

Vector2 WorldSystem::get_world_position(const Vector2i& tile_pos) const {
    if (!tile_map) return Vector2(0, 0);
    return tile_map->map_to_local(tile_pos);
}

bool WorldSystem::is_position_valid(const Vector2& pos) const {
    if (!tile_map) return false;
    
    Vector2i tile_pos = get_tile_position(pos);
    return tile_map->get_cell_source_id(0, tile_pos) != -1;
}

Array WorldSystem::get_placeable_objects() const {
    Array result;
    for (const PlaceableObject& obj : placeable_objects) {
        Dictionary dict;
        dict["type"] = obj.type;
        dict["position"] = obj.position;
        dict["properties"] = obj.properties;
        result.push_back(dict);
    }
    return result;
}

Vector2 WorldSystem::get_random_spawn_point() const {
    if (spawn_points.empty()) {
        return Vector2();
    }
    int index = UtilityFunctions::randi() % spawn_points.size();
    return spawn_points[index];
}

void WorldSystem::set_seed(int p_seed) {
    current_seed = p_seed;
    rng.seed(static_cast<unsigned int>(p_seed));
}

int WorldSystem::get_seed() const {
    return current_seed;
}

void WorldSystem::set_room_size_range(int min_size, int max_size) {
    min_room_size = min_size;
    max_room_size = max_size;
}

void WorldSystem::set_room_count_range(int min_count, int max_count) {
    min_rooms = min_count;
    max_rooms = max_count;
} 