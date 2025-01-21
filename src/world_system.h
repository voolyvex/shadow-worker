#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/array.hpp>
#include <vector>
#include <random>
#include <string>

using namespace godot;

// Object types that can be placed in rooms
enum class ObjectType {
    CONTAINER,
    WORKSTATION,
    OBSTACLE,
    DECORATION,
    LIGHT_SOURCE
};

// Object placement data
struct PlaceableObject {
    String type;
    Vector2 position;
    Dictionary properties;
};

struct Room {
    Vector2i position;
    Vector2i size;
    std::vector<Vector2i> doors;
    bool connected;
    std::vector<Vector2i> occupied_positions;
};

class WorldSystem : public Node2D {
    GDCLASS(WorldSystem, Node2D)

private:
    TileMap* tile_map = nullptr;
    std::vector<Room> rooms;
    int current_seed;
    std::mt19937 rng;
    
    // Room generation parameters
    int min_room_size;
    int max_room_size;
    int min_rooms;
    int max_rooms;
    
    // Object placement parameters
    std::vector<PlaceableObject> placeable_objects;
    float object_density;  // Objects per tile
    int max_placement_attempts;
    
    // NPC placement parameters
    int min_npcs_per_room;
    int max_npcs_per_room;
    String npc_scene_path;
    
    std::vector<Vector2> spawn_points;
    std::vector<Vector2i> navigation_grid;
    Array placeable_objects_array;
    static const int FIELD_SIZE = 32;

    void generate_room_layout();
    void connect_rooms();
    void place_objects();
    void place_npcs();
    bool rooms_overlap(const Room& a, const Room& b);
    std::vector<Vector2i> find_valid_door_positions(const Room& room);
    bool is_valid_object_position(const Room& room, const Vector2i& pos, const PlaceableObject& obj);
    bool is_near_wall(const Room& room, const Vector2i& pos);
    void register_placeable_objects();
    Vector2i find_random_floor_position(const Room& room);
    void place_object_in_room(Room& room, const PlaceableObject& obj);
    void update_room_navigation(Room& room, const Vector2i& pos, bool blocks_path);
    void initialize_placeable_objects();
    void initialize_spawn_points();
    void initialize_navigation();
    Vector2 get_random_spawn_point() const;

    int determine_tile_type(float noise_value) const;
    int get_object_tile_index(const String& object_type) const;

protected:
    static void _bind_methods();

public:
    WorldSystem();
    ~WorldSystem();

    void _init();
    void _ready() override;
    
    // World generation methods
    void generate_world(const Dictionary& params);
    void clear_world();
    
    // Getters and setters
    void set_seed(int p_seed);
    int get_seed() const;
    void set_room_size_range(int min_size, int max_size);
    void set_room_count_range(int min_count, int max_count);
    void set_object_density(float density);
    void set_npc_scene(const String& scene_path);
    void set_npc_count_range(int min_count, int max_count);
    void add_placeable_object(const String& scene_path, ObjectType type, float weight, bool wall_required, bool blocks_path);
    Vector2i get_tile_position(const Vector2& world_pos) const;
    Vector2 get_world_position(const Vector2i& tile_pos) const;
    bool is_position_valid(const Vector2& pos) const;
    Array get_placeable_objects() const;
}; 