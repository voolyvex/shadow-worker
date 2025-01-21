#ifndef NPC_H
#define NPC_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "personality_system.h"
#include <queue>
#include <map>

namespace godot {

// NPC behavior states
enum class NPCState {
    IDLE,
    PATROL,
    FOLLOW,
    FLEE,
    INTERACT,
    WORK
};

class NPC : public CharacterBody2D {
    GDCLASS(NPC, CharacterBody2D)

private:
    // Movement properties
    float speed;
    Vector2 velocity;
    float patrol_radius;
    Vector2 home_position;
    Vector2 target_position;
    
    // State management
    NPCState current_state;
    PersonalityProfile* personality;
    float state_timer;
    float decision_cooldown;
    std::map<NPCState, float> state_priorities;
    
    // Interaction properties
    bool is_interactable;
    Label* dialogue_display;
    Area2D* detection_area;
    float interaction_radius;
    float interaction_cooldown;
    float cooldown_timer;
    
    // Animation
    AnimatedSprite2D* sprite;
    String current_animation;
    
    // Dialogue system
    int current_dialogue_state;
    Dictionary dialogue_data;
    std::queue<String> dialogue_queue;
    
    // Internal methods
    void setup_detection_area();
    void setup_dialogue_display();
    void update_animation();
    void update_state(double delta);
    void handle_movement(double delta);
    void make_decision();
    Vector2 get_random_patrol_point() const;
    void process_personality_influence();
    float calculate_context_priority(NPCState state, const Array& nearby_entities, const Vector2& current_location);
    float calculate_emotional_priority(NPCState state, const Dictionary& emotional_state);
    void handle_event(const Dictionary& event);
    
    // State-specific handlers
    void handle_idle_state(double delta);
    void handle_patrol_state(double delta);
    void handle_follow_state(double delta);
    void handle_flee_state(double delta);
    void handle_interact_state(double delta);
    void handle_work_state(double delta);
    
    // Utility methods
    Array get_nearby_entities() const;
    bool is_work_hours() const;
    bool is_near_workstation(const Vector2& location) const;
    float get_distance_to_target() const;
    Vector2 calculate_avoidance_vector() const;
    bool has_line_of_sight(const Vector2& target) const;

protected:
    static void _bind_methods();

public:
    NPC();
    ~NPC();

    void _init();
    void _ready();
    void _process(double delta);
    void _physics_process(double delta);
    
    // Interaction methods
    void interact();
    void on_player_entered(Node* body);
    void on_player_exited(Node* body);
    void on_interaction_start();
    void on_interaction_end();
    
    // Dialogue methods
    void display_dialogue(const String& text);
    void queue_dialogue(const String& text);
    void clear_dialogue();
    void advance_dialogue_state();
    
    // State methods
    void set_state(NPCState new_state);
    NPCState get_state() const;
    
    // Personality methods
    void initialize_personality(EnneagramType type);
    void set_personality_trait(const String& trait, float value);
    float get_personality_trait(const String& trait) const;
    Dictionary get_personality_state() const;
    void handle_emotional_event(const String& emotion, float intensity);
    
    // Getters and setters
    void set_interaction_radius(float radius);
    float get_interaction_radius() const;
    void set_patrol_radius(float radius);
    float get_patrol_radius() const;
    void set_dialogue_data(const Dictionary& data);
    Dictionary get_dialogue_data() const;
    bool get_is_interactable() const;
    void set_speed(float p_speed);
    float get_speed() const;
};

}

#endif // NPC_H 