#ifndef WORLD_SYSTEM_H
#define WORLD_SYSTEM_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>
#include <random>
#include <string>

namespace godot {

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
    ObjectType type;
    String scene_path;
    float spawn_weight;
    bool requires_wall;
    bool blocks_path;
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
    TileMap* tile_map;
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

protected:
    static void _bind_methods();

public:
    WorldSystem();
    ~WorldSystem();

    void _init();
    void _ready();
    
    // World generation methods
    void generate_world(int seed);
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
};

}

#endif // WORLD_SYSTEM_H 