#include "world_system.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <queue>
#include <algorithm>
#include <chrono>

using namespace godot;

void WorldSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &WorldSystem::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &WorldSystem::get_seed);
    ClassDB::bind_method(D_METHOD("generate_world", "seed"), &WorldSystem::generate_world);
    ClassDB::bind_method(D_METHOD("clear_world"), &WorldSystem::clear_world);
    ClassDB::bind_method(D_METHOD("set_object_density", "density"), &WorldSystem::set_object_density);
    ClassDB::bind_method(D_METHOD("set_npc_scene", "scene_path"), &WorldSystem::set_npc_scene);
    ClassDB::bind_method(D_METHOD("set_npc_count_range", "min_count", "max_count"), &WorldSystem::set_npc_count_range);
    ClassDB::bind_method(D_METHOD("add_placeable_object", "scene_path", "type", "weight", "wall_required", "blocks_path"), &WorldSystem::add_placeable_object);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "object_density"), "set_object_density", "get_object_density");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "npc_scene"), "set_npc_scene", "get_npc_scene");
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
    tile_map = Object::cast_to<TileMap>(get_node("TileMap"));
    ERR_FAIL_NULL_MSG(tile_map, "WorldSystem requires a TileMap child node!");
}

void WorldSystem::generate_world(int seed) {
    set_seed(seed);
    clear_world();
    generate_room_layout();
    connect_rooms();
    place_objects();
    place_npcs();
}

void WorldSystem::generate_room_layout() {
    rooms.clear();
    
    // Determine number of rooms
    std::uniform_int_distribution<int> room_count_dist(min_rooms, max_rooms);
    int num_rooms = room_count_dist(rng);
    
    std::uniform_int_distribution<int> size_dist(min_room_size, max_room_size);
    std::uniform_int_distribution<int> pos_dist(-50, 50); // Adjust range as needed
    
    // Generate rooms with random sizes and positions
    for (int i = 0; i < num_rooms; i++) {
        Room room;
        bool valid_position = false;
        
        // Keep trying positions until we find a non-overlapping one
        while (!valid_position) {
            room.size = Vector2i(size_dist(rng), size_dist(rng));
            room.position = Vector2i(pos_dist(rng), pos_dist(rng));
            room.connected = false;
            
            valid_position = true;
            for (const auto& existing_room : rooms) {
                if (rooms_overlap(room, existing_room)) {
                    valid_position = false;
                    break;
                }
            }
        }
        
        rooms.push_back(room);
    }
}

bool WorldSystem::rooms_overlap(const Room& a, const Room& b) {
    // Add padding between rooms
    const int padding = 2;
    
    bool x_overlap = (a.position.x - padding < b.position.x + b.size.x + padding) &&
                    (a.position.x + a.size.x + padding > b.position.x - padding);
                    
    bool y_overlap = (a.position.y - padding < b.position.y + b.size.y + padding) &&
                    (a.position.y + a.size.y + padding > b.position.y - padding);
                    
    return x_overlap && y_overlap;
}

void WorldSystem::connect_rooms() {
    if (rooms.empty()) return;
    
    // Mark first room as connected
    rooms[0].connected = true;
    
    // Keep connecting rooms until all are connected
    while (std::any_of(rooms.begin(), rooms.end(), 
           [](const Room& r) { return !r.connected; })) {
        
        int closest_unconnected = -1;
        int closest_connected = -1;
        float min_distance = std::numeric_limits<float>::max();
        
        // Find the closest pair of connected and unconnected rooms
        for (size_t i = 0; i < rooms.size(); i++) {
            if (!rooms[i].connected) {
                for (size_t j = 0; j < rooms.size(); j++) {
                    if (rooms[j].connected) {
                        float dist = Vector2(rooms[i].position).distance_to(Vector2(rooms[j].position));
                        if (dist < min_distance) {
                            min_distance = dist;
                            closest_unconnected = i;
                            closest_connected = j;
                        }
                    }
                }
            }
        }
        
        if (closest_unconnected != -1 && closest_connected != -1) {
            // Create doors for both rooms
            auto& room1 = rooms[closest_unconnected];
            auto& room2 = rooms[closest_connected];
            
            auto doors1 = find_valid_door_positions(room1);
            auto doors2 = find_valid_door_positions(room2);
            
            if (!doors1.empty() && !doors2.empty()) {
                room1.doors.push_back(doors1[0]);
                room2.doors.push_back(doors2[0]);
                room1.connected = true;
            }
        }
    }
}

std::vector<Vector2i> WorldSystem::find_valid_door_positions(const Room& room) {
    std::vector<Vector2i> valid_positions;
    
    // Check all walls for valid door positions
    // Top and bottom walls
    for (int x = 1; x < room.size.x - 1; x++) {
        valid_positions.push_back(Vector2i(room.position.x + x, room.position.y));
        valid_positions.push_back(Vector2i(room.position.x + x, room.position.y + room.size.y - 1));
    }
    
    // Left and right walls
    for (int y = 1; y < room.size.y - 1; y++) {
        valid_positions.push_back(Vector2i(room.position.x, room.position.y + y));
        valid_positions.push_back(Vector2i(room.position.x + room.size.x - 1, room.position.y + y));
    }
    
    // Shuffle the positions
    std::shuffle(valid_positions.begin(), valid_positions.end(), rng);
    return valid_positions;
}

void WorldSystem::register_placeable_objects() {
    // Register default placeable objects
    placeable_objects = {
        {"res://scenes/objects/container.tscn", ObjectType::CONTAINER, 1.0f, true, true},
        {"res://scenes/objects/workbench.tscn", ObjectType::WORKSTATION, 0.7f, true, true},
        {"res://scenes/objects/crate.tscn", ObjectType::OBSTACLE, 0.8f, false, true},
        {"res://scenes/objects/lamp.tscn", ObjectType::LIGHT_SOURCE, 0.5f, true, false},
        {"res://scenes/objects/plant.tscn", ObjectType::DECORATION, 0.3f, false, false}
    };
}

void WorldSystem::place_objects() {
    if (rooms.empty() || placeable_objects.empty()) return;
    
    // Calculate weights for object selection
    std::vector<float> weights;
    float total_weight = 0;
    for (const auto& obj : placeable_objects) {
        total_weight += obj.spawn_weight;
        weights.push_back(total_weight);
    }
    
    // Place objects in each room
    for (auto& room : rooms) {
        int num_objects = static_cast<int>(room.size.x * room.size.y * object_density);
        
        for (int i = 0; i < num_objects; i++) {
            // Select random object based on weights
            float rand_val = Math::randf() * total_weight;
            auto it = std::lower_bound(weights.begin(), weights.end(), rand_val);
            size_t obj_index = std::distance(weights.begin(), it);
            
            if (obj_index < placeable_objects.size()) {
                place_object_in_room(room, placeable_objects[obj_index]);
            }
        }
    }
}

void WorldSystem::place_object_in_room(Room& room, const PlaceableObject& obj) {
    for (int attempt = 0; attempt < max_placement_attempts; attempt++) {
        Vector2i pos = find_random_floor_position(room);
        
        if (is_valid_object_position(room, pos, obj)) {
            // Load and instance the object scene
            Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(obj.scene_path);
            if (scene.is_valid()) {
                Node* instance = scene->instantiate();
                if (instance) {
                    add_child(instance);
                    Node2D* node_2d = Object::cast_to<Node2D>(instance);
                    if (node_2d) {
                        // Convert tilemap position to world position
                        Vector2 world_pos = tile_map->map_to_local(pos);
                        node_2d->set_position(world_pos);
                        
                        // Update room navigation data
                        update_room_navigation(room, pos, obj.blocks_path);
                        break;
                    }
                }
            }
        }
    }
}

void WorldSystem::place_npcs() {
    if (rooms.empty() || npc_scene_path.is_empty()) return;
    
    Ref<PackedScene> npc_scene = ResourceLoader::get_singleton()->load(npc_scene_path);
    if (!npc_scene.is_valid()) return;
    
    std::uniform_int_distribution<int> npc_count_dist(min_npcs_per_room, max_npcs_per_room);
    
    for (const auto& room : rooms) {
        int num_npcs = npc_count_dist(rng);
        
        for (int i = 0; i < num_npcs; i++) {
            Vector2i pos = find_random_floor_position(room);
            
            // Check if position is valid (not blocked)
            if (std::find(room.occupied_positions.begin(), room.occupied_positions.end(), pos) 
                == room.occupied_positions.end()) {
                
                Node* instance = npc_scene->instantiate();
                if (instance) {
                    add_child(instance);
                    Node2D* npc = Object::cast_to<Node2D>(instance);
                    if (npc) {
                        // Convert tilemap position to world position
                        Vector2 world_pos = tile_map->map_to_local(pos);
                        npc->set_position(world_pos);
                    }
                }
            }
        }
    }
}

Vector2i WorldSystem::find_random_floor_position(const Room& room) {
    std::uniform_int_distribution<int> x_dist(1, room.size.x - 2);
    std::uniform_int_distribution<int> y_dist(1, room.size.y - 2);
    
    return Vector2i(
        room.position.x + x_dist(rng),
        room.position.y + y_dist(rng)
    );
}

bool WorldSystem::is_valid_object_position(const Room& room, const Vector2i& pos, const PlaceableObject& obj) {
    // Check if position is already occupied
    if (std::find(room.occupied_positions.begin(), room.occupied_positions.end(), pos) 
        != room.occupied_positions.end()) {
        return false;
    }
    
    // Check wall requirement
    if (obj.requires_wall && !is_near_wall(room, pos)) {
        return false;
    }
    
    // Check if position blocks any door
    for (const auto& door : room.doors) {
        if (pos == door || pos.distance_to(door) < 2) {
            return false;
        }
    }
    
    return true;
}

bool WorldSystem::is_near_wall(const Room& room, const Vector2i& pos) {
    return pos.x == room.position.x + 1 || 
           pos.x == room.position.x + room.size.x - 2 ||
           pos.y == room.position.y + 1 || 
           pos.y == room.position.y + room.size.y - 2;
}

void WorldSystem::update_room_navigation(Room& room, const Vector2i& pos, bool blocks_path) {
    if (blocks_path) {
        room.occupied_positions.push_back(pos);
    }
}

void WorldSystem::set_object_density(float density) {
    object_density = Math::clamp(density, 0.0f, 1.0f);
}

void WorldSystem::set_npc_scene(const String& scene_path) {
    npc_scene_path = scene_path;
}

void WorldSystem::set_npc_count_range(int min_count, int max_count) {
    min_npcs_per_room = Math::max(0, min_count);
    max_npcs_per_room = Math::max(min_npcs_per_room, max_count);
}

void WorldSystem::add_placeable_object(const String& scene_path, ObjectType type, float weight, bool wall_required, bool blocks_path) {
    PlaceableObject obj{type, scene_path, weight, wall_required, blocks_path};
    placeable_objects.push_back(obj);
}

void WorldSystem::clear_world() {
    if (tile_map) {
        tile_map->clear();
    }
    rooms.clear();
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